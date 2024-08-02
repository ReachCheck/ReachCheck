package dependence.container;

import dependence.util.MavenUtil;
import dependence.vo.DepJar;
import dependence.vo.NodeAdapter;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class DepJars {
    private static DepJars instance;

    public static DepJars i() {
        return instance;
    }

    public static void init(NodeAdapters nodeAdapters) {
        if (instance == null) {
            instance = new DepJars(nodeAdapters);
        }
    }

    private final Set<DepJar> container;
    private DepJar hostDepJar;
    private String repoPath;

    private DepJars(NodeAdapters nodeAdapters) {
        container = new HashSet<>();
        for (NodeAdapter nodeAdapter : nodeAdapters.getAllNodeAdapter()) {
            container.add(new DepJar(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId(), nodeAdapter.getVersion(),
                    nodeAdapter.getClassifier(), nodeAdapter.getPriority(), nodeAdapter.getFilePath(),repoPath));
        }
    }

    /**
     * get used dep jars
     *
     * @return Set<DepJar> usedDepJars
     * 获得所有使用的依赖的DepJar
     */
    public Set<DepJar> getUsedDepJars() {
        Set<DepJar> usedDepJars = new HashSet<>();
        for (DepJar depJar : container) {
            if (depJar.isSelected()) { //这个DepJar只要有一个版本被使用了都算isSelected
                usedDepJars.add(depJar); //那就把这个jar加入Set准备返回吧
            }
        }
        return usedDepJars;
    }

    /**
     * get dep jar belonged to hots
     *
     * @return DepJar hostDepJar
     */
    public DepJar getHostDepJar() {
        if (hostDepJar == null) {

            for (DepJar depJar : container) {
                if (depJar.isHost()) {
                    if (hostDepJar != null) {
                        MavenUtil.i().getLog().warn("multiple dependency jar for host ");
                    }
                    hostDepJar = depJar;
                }
            }
            MavenUtil.i().getLog().info("dependency jar host is " + hostDepJar.toString());
        }
        return hostDepJar;
    }

    /**
     * use groupId, artifactId, version and classifier to find the same DepJar
     *
     * @return same depJar or null
     */
    public DepJar getDep(String groupId, String artifactId, String version, String classifier) {
        for (DepJar dep : container) {
            if (dep.isSame(groupId, artifactId, version, classifier)) {
                return dep;
            }
        }
        MavenUtil.i().getLog().warn("cant find dep:" + groupId + ":" + artifactId + ":" + version + ":" + classifier);
        return null;
    }

    public Set<DepJar> getAllDepJar() {
        return container;
    }

    /**
     * use nodeAdapter to find the same DeoJar
     * kernel is getDep(String groupId, String artifactId, String version, String classifier)
     *
     * @return same depJar or null
     */
    public DepJar getDep(NodeAdapter nodeAdapter) {
        return getDep(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId(), nodeAdapter.getVersion(),
                nodeAdapter.getClassifier());
    }

    /**
     * get all used dep jar's file path
     *
     * @return 所有的jar path
     */
    public List<String> getUsedJarPaths() {
        List<String> usedJarPaths = new ArrayList<>();
        for (DepJar depJar : DepJars.i().getAllDepJar()) {
            if (depJar.isSelected()) {
                usedJarPaths.addAll(depJar.getJarFilePaths(true));
            }
        }
        return usedJarPaths;
    }

    /**
     * 将输入的依赖作为项目中使用的依赖，替换原本正在使用的依赖
     *
     * @param entryDepJar 输入的依赖
     * @return 所有的jar path
     */
    public List<String> getUsedJarPaths(DepJar entryDepJar) {
        List<String> usedJarPaths = new ArrayList<>();
        for (DepJar depJar : DepJars.i().getAllDepJar()) {
            if (depJar.isSelected()) {
                if (!depJar.isSameLib(entryDepJar)) {
                    usedJarPaths.addAll(depJar.getJarFilePaths(true));
                }
            }
            usedJarPaths.addAll(entryDepJar.getJarFilePaths(true));
        }
        return usedJarPaths;
    }

    /**
     * @param cls 类sig
     * @return usedDepJar that has class.
     */
    public DepJar getClassJar(String cls) {
        for (DepJar depJar : DepJars.i().getAllDepJar()) {
            if (depJar.isSelected()) {
                if (depJar.containsCls(cls))
                    return depJar;
            }
        }
        return null;
    }

    public DepJar getDepJar(String[] nodeInfo) {
        DepJar targetDepJar = null;
        for (DepJar depJar : container) {
            if (depJar.getGroupId().equals(nodeInfo[0])
                    && depJar.getArtifactId().equals(nodeInfo[1])
                    && depJar.getVersion().equals(nodeInfo[2])) {
                targetDepJar = depJar;
                break;
            }
        }
        return targetDepJar;
    }

    public void setRepoPath(String path){
        this.repoPath =path;
    }

}
