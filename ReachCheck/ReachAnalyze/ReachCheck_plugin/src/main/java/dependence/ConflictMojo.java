package dependence;

import dependence.container.AllCls;
import dependence.container.Conflicts;
import dependence.container.DepJars;
import dependence.container.NodeAdapters;
import dependence.util.Conf;
import dependence.util.GlobalVar;
import dependence.util.MavenUtil;
import dependence.vo.DepJar;
import detect.ProjectAna;
import org.apache.maven.artifact.factory.ArtifactFactory;
import org.apache.maven.artifact.handler.manager.ArtifactHandlerManager;
import org.apache.maven.artifact.repository.ArtifactRepository;
import org.apache.maven.artifact.resolver.ArtifactResolver;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.plugins.annotations.Component;
import org.apache.maven.plugins.annotations.Parameter;
import org.apache.maven.project.MavenProject;
import org.apache.maven.shared.dependency.tree.DependencyNode;
import org.apache.maven.shared.dependency.tree.DependencyTreeBuilder;
import org.apache.maven.shared.dependency.tree.DependencyTreeBuilderException;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public abstract class ConflictMojo extends AbstractMojo {

    @Parameter(defaultValue = "${project}", readonly = true, required = true)
    public MavenProject project;

    @Parameter(defaultValue = "${reactorProjects}", readonly = true, required = true)
    public List<MavenProject> reactorProjects;

    @Parameter(defaultValue = "${project.remoteArtifactRepositories}", readonly = true, required = true)
    public List<ArtifactRepository> remoteRepositories;

    @Parameter(defaultValue = "${localRepository}", readonly = true)
    public ArtifactRepository localRepository;

    @Component
    public DependencyTreeBuilder dependencyTreeBuilder;

    @Parameter(defaultValue = "${project.build.directory}", required = true)
    public File buildDir;

    @Component
    public ArtifactFactory factory;

    @Component
    public ArtifactHandlerManager artifactHandlerManager;
    @Component
    public ArtifactResolver resolver;
    DependencyNode root;

    @Parameter(defaultValue = "${project.compileSourceRoots}", readonly = true, required = true)
    public List<String> compileSourceRoots;

    // whether ignore test scope
    @Parameter(property = "ignoreTestScope", defaultValue = "true")
    public boolean ignoreTestScope;

    // whether ignore test classifier
    @Parameter(property = "ignoreTestClassifier", defaultValue = "false")
    public boolean ignoreTestClassifier;

    // whether ignore provided scope
    @Parameter(property = "ignoreProvidedScope", defaultValue = "false")
    public boolean ignoreProvidedScope;

    // whether ignore run time scope
    @Parameter(property = "ignoreRuntimeScope", defaultValue = "false")
    public boolean ignoreRuntimeScope;

    @Parameter(property = "append", defaultValue = "false")
    public boolean append;

    // whether load all jar
    @Parameter(property = "useAllJar", defaultValue = "false")
    public boolean useAllJar;

    @Parameter(property = "filterSuper", defaultValue = "false")
    public boolean filterSuper;

    @Parameter(property = "disDepth")
    public int disDepth = Integer.MAX_VALUE;

    @Parameter(property = "pathDepth")
    public int pathDepth = Integer.MAX_VALUE;

    @Parameter(property = "callConflict")
    public String callConflict = null;

    @Parameter(property = "callConflicts")
    public String callConflicts = null;

    //自定义输出目录
//    @Parameter(property = "resultPath")
//    public String resultPath = null;

    @Parameter(property = "projectPath",defaultValue = "default")
    public String projectPath;

    @Parameter(property = "repoPath",defaultValue = "default")
    public String repoPath;

//    //输入仓库的路径
//    @Parameter(property = "repoPath")
//    public String repoPath = null;

    @Parameter(property = "classMissing", defaultValue = "false")
    public boolean classMissing;

    //设置是否细分1234等级
    @Parameter(property = "subdivisionLevel", defaultValue = "false")
    public boolean subdivisionLevel;

    @Parameter(property = "findAllPath", defaultValue = "false")
    public boolean findAllPath;

    @Parameter(property = "targetJar",defaultValue = "default")
    public String targetJar;

    // specified filter path
    @Parameter(property = "filterListPath", defaultValue = "default")
    public String filterListPath;


    //detect target
    @Parameter(property = "target", defaultValue = "default")
    public String target;

    public int systemSize = 0;

    public long systemFileSize = 0;//byte

    public List<String> noPomPaths = new ArrayList<>();

    public long buildMapTime = 0;

    //初始化全局变量
    protected void initGlobalVar() throws Exception {
        //配置参数
        MavenUtil.i().setMojo(this); //在MavenUtil设置一下插件指向自己
        Conf.CLASS_MISSING = classMissing; //是否检测类丢失情况，false
        Conf.targetJar = targetJar; //用户是否指定待检测目标jar
        Conf.DOG_DEP_FOR_DIS = disDepth;
        Conf.DOG_DEP_FOR_PATH = pathDepth;
        Conf.callConflict = callConflict;
        Conf.callConflicts = callConflicts;
        Conf.findAllpath = findAllPath;
        Conf.repoPath = repoPath;
        Conf.localMvnRepoPath = localRepository.getBasedir();
        GlobalVar.useAllJar = useAllJar;
        GlobalVar.filterSuper = filterSuper;
        GlobalVar.filterListPath = filterListPath;
        GlobalVar.riskMethodMap = new HashMap<>();
        MavenUtil.i().setSrcJarPath(targetJar);
        //初始化NodeAdapters
        NodeAdapters.init(root);
        //初始化DepJars
        DepJars.init(NodeAdapters.i());// occur jar in tree
        validateSysSize();

        //初始化所有的类集合
//        AllCls.init(DepJars.i());

        Conf.compileSourceRoots = compileSourceRoots;

        Conflicts.init(NodeAdapters.i());// version conflict in tree	初始化树中的版本冲突
    }

    /**
     * When specifying the path of the filtering method set, verify whether the path is correct
     * 当指定过滤方法集合的路径时，验证路径是否正确
     *
     * @throws MojoExecutionException
     */
    private void verifyFilterListFileWhetherExist() throws MojoExecutionException {
        if (!GlobalVar.filterListPath.equals("default")) {
            File file = new File(GlobalVar.filterListPath);
            if (!file.exists()) {
                throw new MojoExecutionException("file doesn't exist!");
            }
        }
    }

    /**
     * init filter methods list
     * 初始化要过滤的方法集合
     */
    private void initFilterListFileList() {
        List<String> filterMthds;
        if (GlobalVar.filterListPath.equals("default")) {
            // 不指定路径时，加载系统自带的待过滤方法集合
            filterMthds = getFilterMthds();
        } else {
            // 指定路径时，加载指定路径中的待过滤方法集合
            filterMthds = getDesignatedFillterMthds(GlobalVar.filterListPath);
        }
        GlobalVar.filterMthds = filterMthds;
    }

    /**
     * When the path is not specified, load the system default filtering method
     * 不特殊指定路径时，加载系统默认的过滤方法
     *
     * @return List<String> filterMthds
     */
    public List<String> getFilterMthds() {
        List<String> filterMthds = new ArrayList<>();
        try {
            // load local filtering methods, file is in the resources, file name is FilterMethodVO.txt
            InputStream resourceAsStream = this.getClass().getClassLoader().getResourceAsStream("FilterMethodVO.txt");
            if (resourceAsStream != null) {
                BufferedReader br = new BufferedReader(new InputStreamReader(resourceAsStream));
                String str;
                while ((str = br.readLine()) != null) {
                    filterMthds.add(str);
                }
                br.close();
            }
        } catch (IOException | NullPointerException e) {
            e.printStackTrace();
        }
        return filterMthds;
    }

    /**
     * When specifying the path, load the specified file
     * 指定路径时，加载指定文件
     *
     * @param filePath : specified file path
     * @return List<String> filterMthds
     */
    public List<String> getDesignatedFillterMthds(String filePath) {
        List<String> filterMthds = new ArrayList<>();
        try {
            BufferedReader br = new BufferedReader(new FileReader(filePath));
            String str;
            while ((str = br.readLine()) != null) {
                filterMthds.add(str);
            }
            br.close();
        } catch (IOException | NullPointerException e) {
            MavenUtil.i().getLog().error("The file doesn't exist");
        }
        return filterMthds;
    }

    private void validateSysSize() {
        for (DepJar depJar : DepJars.i().getAllDepJar()) {
            if (depJar.isSelected()) {
                systemSize++;
                for (String filePath : depJar.getJarFilePaths(true)) {
                    systemFileSize = systemFileSize + new File(filePath).length();
                }
            }
        }
        MavenUtil.i().getLog().info("tree size:" + DepJars.i().getAllDepJar().size() + ", used size:" + systemSize
                + ", usedFile size:" + systemFileSize / 1000);
    }

    @Override
    public void execute() throws MojoExecutionException {
        this.getLog().info("method detect start:");
        long startTime = System.currentTimeMillis();
        //先得到项目打包类型，假如满足可检测类型"jar" "war" "maven-plugin" "bundle"，那么获得分析树并且把根节点交给root。
        String pckType = project.getPackaging();    //得到项目的打包类型
        if ("jar".equals(pckType) || "war".equals(pckType) || "maven-plugin".equals(pckType)
                || "bundle".equals(pckType)) {
            try {
                // project.
                root = dependencyTreeBuilder.buildDependencyTree(project, localRepository, null);
            } catch (DependencyTreeBuilderException e) {
                throw new MojoExecutionException(e.getMessage());
            }
            //接下来，初始化全局变量
            try {
                initGlobalVar();
            } catch (Exception e) {
                MavenUtil.i().getLog().error(e);
                System.out.println(e);
                throw new MojoExecutionException("project size error!");
            }
            // 验证当指定过滤器待过滤方法集合的路径时，路径是否正确
            try {
                verifyFilterListFileWhetherExist();
            } catch (Exception e) {
                MavenUtil.i().getLog().error(e);
                throw new MojoExecutionException("file doesn't exist!");
            }
            // 加载过滤器待过滤方法集合
            initFilterListFileList();
            run();

        } else {
            this.getLog()
                    .info("this project fail because package type is neither jar nor war:" + project.getGroupId() + ":"
                            + project.getArtifactId() + ":" + project.getVersion() + "@"
                            + project.getFile().getAbsolutePath());
        }
        GlobalVar.runTime = (System.currentTimeMillis() - startTime) / 1000;
        // 打印运行时间信息
//        printRunTime();
        this.getLog().debug("method detect end");

    }

    /**
     * transform time from seconds to suitable format
     * 把运行时间（以秒为单位）转化为合适的格式
     *
     * @param time : seconds
     * @return suitable format
     */
    private String transform(long time) {
        if (time < 60)
            return time + "";
        else if (time < 3600) {
            int min = (int) time / 60;
            int sec = (int) time % 60;
            return " (" + min + ":" + sec + "min)";
        } else {
            int hour = (int) time / 3600;
            int min = (int) time % 3600 / 60;
            return " (" + hour + ":" + min + "h)";
        }
    }

    /**
     * 打印运行时间信息
     */
    private void printRunTime() {
        this.getLog().info("first conflict time to run:" + transform(GlobalVar.firstConflictTime));
        this.getLog().info("time to run:" + transform(GlobalVar.runTime));
        this.getLog().info("time to call graph:" + transform(GlobalVar.time2cg));
        this.getLog().info("time to run dog:" + transform(GlobalVar.time2runDog));
        this.getLog().info("time to calculate branch:" + transform(GlobalVar.branchTime));
        this.getLog().info("time to calculate reference:" + transform(GlobalVar.time2calRef));
        this.getLog().info("time to filter riskMethod:" + transform(GlobalVar.time2filterRiskMthd));
    }

    public abstract void run();
}
