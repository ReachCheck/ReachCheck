package dependence.vo;

import callGraph.vo.DCGClassVO;
import callGraph.vo.SourceClassManager;
import callGraph.vo.SourceMethodManager;
import dependence.ConflictMojo;
import dependence.container.AllCls;
import dependence.container.AllRefedCls;
import dependence.container.DepJars;
import dependence.container.NodeAdapters;
import dependence.util.MavenUtil;
import dependence.util.Util;
import org.apache.maven.shared.dependency.tree.DependencyNode;

import java.io.IOException;
import java.util.*;

/**
 *
 */
public class DepJar {
    private String groupId;
    private String artifactId;// artifactId
    private String version;// version
    private String classifier;
    private List<String> jarFilePaths;// host project may have multiple source.
    private Map<String, ClassVO> clsTb;// all class in jar
    private Set<NodeAdapter> nodeAdapters;// all
    private Set<String> allMthd;
    private Set<String> definitionClasses;
    private Map<String, ClassVO> allClass;// all class in jar
    private Map<String,Set<String>> class2Method;
    private int priority;
    private String repoPath;
    private List<DepJar> childJars;

    public DepJar(String groupId, String artifactId, String version, String classifier, int priority, List<String> jarFilePaths,String path) {
        this.groupId = groupId;
        this.artifactId = artifactId;
        this.version = version;
        this.classifier = classifier;
        this.priority = priority;
        this.jarFilePaths = jarFilePaths;
//        this.repoPath = path+"/"+groupId.replace('.','/')+"/"+artifactId+"/"+version+"/"+artifactId+"-"+version+".jar";
    }

    public DepJar(String groupId, String artifactId, String version, String classifier, List<String> jarFilePaths) {
        this.groupId = groupId;
        this.artifactId = artifactId;
        this.version = version;
        this.classifier = classifier;
        this.jarFilePaths = jarFilePaths;
    }

    /**
     * get jar may have risk thinking same class in different dependency,selected jar may have risk;
     * Not thinking same class in different dependency,selected jar is safe
     *
     * @return
     */
    public boolean isRisk() {
        return !this.isSelected();
    }

    /**
     * all class in jar中是不是包含某一class
     */
    public boolean containsCls(String clsSig) {
        return this.getClsTb().containsKey(clsSig);
    }


    public Set<NodeAdapter> getNodeAdapters() {
        if (nodeAdapters == null) {
            nodeAdapters = NodeAdapters.i().getNodeAdapters(this);
        }
        return nodeAdapters;
    }

    /**
     * get all dep Paths
     * notice : is different from String getAllDepPaths()
     * kernel is same, format different
     *
     * @return String path
     */
    public String getAllDepPath() {
        StringBuilder sb = new StringBuilder(toString() + ":");
        for (NodeAdapter node : getNodeAdapters()) {
            sb.append("  [");
            sb.append(node.getWholePath());
            sb.append("]");
        }
        return sb.toString();
    }


    /**
     * @return the import path of depJar.
     */
    public String getValidDepPath() {
        StringBuilder sb = new StringBuilder(toString() + ":");
        for (NodeAdapter node : getNodeAdapters()) {
            if (node.isNodeSelected()) {
                sb.append("  [");
                sb.append(node.getWholePath());
                sb.append("]");
            }
        }
        return sb.toString();

    }


    /**
     * @return whether is selected
     * 只要
     */
    public boolean isSelected() {
        for (NodeAdapter nodeAdapter : getNodeAdapters()) {
            if (nodeAdapter.isNodeSelected()) {
                return true;
            }
        }
        return false;
    }

    /**
     * 得到这个jar所有类的集合
     *
     * @return
     */
    public Map<String, ClassVO> getClsTb() {
        if (clsTb == null) {
            if (null == this.getJarFilePaths(true)) {
                // no file
                clsTb = new HashMap<>();
                MavenUtil.i().getLog().warn("can't find jarFile for:" + toString());
            } else {
                try {
                    List<String> jarFilePaths = this.getJarFilePaths(true);
                    clsTb = transformDCGClassVO(SourceClassManager.getAllClassVOFromPaths(jarFilePaths));
                    definitionClasses = SourceClassManager.getDefinitionClasses(jarFilePaths);
                    getClass2Mthod();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if (clsTb.size() == 0) {
                    MavenUtil.i().getLog().warn("get empty clsTb for " + toString());
                }
                for (ClassVO clsVO : clsTb.values()) {
                    clsVO.setDepJar(this);
                }
            }
        }
        return clsTb;
    }

    public Set<String> getDefinitionClasses(){
        return this.definitionClasses;
    }

    public Map<String,Set<String>> getClass2Mthod(){
        if(class2Method == null){
            class2Method = new HashMap<>();
            for(String cls:definitionClasses){
                ClassVO clsVO = clsTb.get(cls);
                Set<String> methodSet = new HashSet<>();
                for(MethodVO mtd:clsVO.getMthds()){
                    methodSet.add(mtd.getMthdSig());
                }
                class2Method.put(cls,methodSet);
            }
        }
        return class2Method;
    }

    public List<DepJar> getChildJars(){
        if(childJars == null){
            childJars = new ArrayList<>();
            for(NodeAdapter nodeAdapter:getNodeAdapters()){
                DependencyNode node = nodeAdapter.node;
                for(DependencyNode childNode:node.getChildren()){
                    NodeAdapter childAdapter = NodeAdapters.i().getNodeAdapter(childNode);
                    if(childAdapter == null){
                        continue;
                    }
                    childJars.add(childAdapter.depJar);
                }
            }
        }
        return childJars;
    }

    private Map<String, ClassVO> transformDCGClassVO(Map<String, DCGClassVO> dcgClassVOSet) {
        Map<String, ClassVO> result = new HashMap<>();
        for (DCGClassVO dcgClassVO : dcgClassVOSet.values()) {
            ClassVO classVO = new ClassVO(dcgClassVO.getClassName(),dcgClassVO.getSuperName(),dcgClassVO.getSubDCGClassVO());
//            ClassVO classVO = new ClassVO(dcgClassVO.getClassName());
            for (callGraph.vo.DCGMethodVO method : dcgClassVO.getMethods()) {
                classVO.addMethod(new MethodVO(method.getSig(), classVO));
            }
            result.put(classVO.getClsSig(), classVO);
        }
        return result;
    }

    public ClassVO getClassVO(String clsSig) {
        return getClsTb().get(clsSig);
    }

    /**
     * 得到这个jar的所有方法
     *
     * @return
     */
    public Set<String> getAllMthd() {
        if (allMthd == null) {
            try {
                allMthd = SourceMethodManager.getAllMethodsFromPaths(this.getJarFilePaths(true));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return allMthd;
    }

    public Set<String> getUsedMthds() {
        Set<String> allMethods = getAllMthd();
        Set<String> usedMethods = new HashSet<>();
        for (String method : allMethods) {
            if (AllRefedCls.i().contains(Util.mthdSig2cls(method))) {
                usedMethods.add(method);
            }
        }
        return usedMethods;
    }

    public boolean containsMthd(String mthd) {
        return getAllMthd().contains(mthd);
    }

    /**
     * 得到本depjar独有的cls
     *
     * @param otherJar
     * @return
     */
    public Set<String> getOnlyClses(DepJar otherJar) {
        Set<String> onlyCls = new HashSet<String>();
        Set<String> otherAll = otherJar.getAllCls(true);
        for (String clsSig : getAllCls(true)) {
            if (!otherAll.contains(clsSig)) {
                onlyCls.add(clsSig);
            }
        }
        return onlyCls;
    }

    /**
     * 得到本depjar独有的mthds
     *
     * @param otherJar
     * @return
     */
    public Set<String> getOnlyMthds(DepJar otherJar) {
        Set<String> onlyMthds = new HashSet<>();
        for (String clsSig : getClsTb().keySet()) {
            ClassVO otherCls = otherJar.getClassVO(clsSig);
            if (otherCls != null) {
                ClassVO cls = getClassVO(clsSig);
                for (MethodVO mthd : cls.getMthds()) {
                    if (!otherCls.hasMethod(mthd.getMthdSig())) {
                        onlyMthds.add(mthd.getMthdSig());
                    }
                }
            }
        }
        return onlyMthds;
    }


    @Override
    public boolean equals(Object obj) {
        if (obj instanceof DepJar) {
            return isSelf((DepJar) obj);

        }
        return false;
    }

    @Override
    public int hashCode() {
        return groupId.hashCode() * 31 * 31 + artifactId.hashCode() * 31 + version.hashCode()
                + classifier.hashCode() * 31 * 31 * 31;
    }

    /**
     * @return groupId:artifactId:version:classifier
     */
    @Override
    public String toString() {
        return groupId + ":" + artifactId + ":" + version + ":" + classifier;
    }

    /**
     * @return groupId:artifactId:version
     */
    public String getSig() {
        return groupId + ":" + artifactId + ":" + version;
    }

    /**
     * @return groupId:artifactId
     */
    public String getName() {
        return groupId + ":" + artifactId;
    }

    /**
     * @return groupId
     */
    public String getGroupId() {
        return groupId;
    }

    /**
     * @return artifactId
     */
    public String getArtifactId() {
        return artifactId;
    }

    /**
     * @return version
     */
    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    /**
     * @return classifier
     */
    public String getClassifier() {
        return classifier;
    }

    /**
     * whether is the same deoJar
     *
     * @param groupId2    : 目标groupId
     * @param artifactId2 : 目标artifactId
     * @param version2    : 目标version
     * @param classifier2 : 目标classifier
     * @return boolean
     */
    public boolean isSame(String groupId2, String artifactId2, String version2, String classifier2) {
        return groupId.equals(groupId2) && artifactId.equals(artifactId2) && version.equals(version2)
                && classifier.equals(classifier2);
    }

    /**
     * 是否为同一个
     *
     * @param dep
     * @return
     */
    public boolean isSelf(DepJar dep) {
        return isSame(dep.getGroupId(), dep.getArtifactId(), dep.getVersion(), dep.getClassifier());
    }

    /**
     * 没有比较版本
     *
     * @param depJar
     * @return
     */
    public boolean isSameLib(DepJar depJar) {
        return getGroupId().equals(depJar.getGroupId()) && getArtifactId().equals(depJar.getArtifactId());
    }


    /**
     * note:from the view of usedJar. e.g.
     * getReplaceJar().getRiskMthds(getRchedMthds());
     *
     * @param testMthds
     * @return
     */
    public Set<String> getRiskMthds(Collection<String> testMthds) {
        Set<String> riskMthds = new HashSet<>();
        for (String testMthd : testMthds) {
            if (!this.containsMthd(testMthd) && AllRefedCls.i().contains(testMthd.split(":")[0])) {
                // don't have method,and class is used. 使用这个类，但是没有方法
                if (this.containsCls(testMthd.split(":")[0])) {
                    // has class.don't have method.	有这个类，没有方法
                    riskMthds.add(testMthd);
                } else if (!AllCls.i().contains(testMthd.split(":")[0])) {
                    // This jar don't have class,and all jar don't have class.	这个jar没有这个class，所有的jar都没有
                    riskMthds.add(testMthd);
                }
            }
        }
        return riskMthds;
    }

    public Set<String> getAllClassesByDCG(boolean useTarget) {
        try {
            // class sig a.b.C
            return SourceClassManager.getAllClassesFromPaths(this.getJarFilePaths(useTarget));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new HashSet<>();
    }

    public Set<String> getAllCls(boolean useTarget) {
        return getAllClassesByDCG(useTarget);
    }

    /**
     * @param useTarget: host-class-name can get from source directory(false) or target
     *                   directory(true). using source directory: advantage: get class
     *                   before maven-package disadvantage:class can't deconstruct
     *                   ;miss class that generated.
     * @return
     */
    public List<String> getJarFilePaths(boolean useTarget) {
        if (!useTarget) {// use source directory
            // if node is inner project,will return source directory(using source directory
            // can get classes before maven-package)
            if (isHost()) {
                return MavenUtil.i().getSrcPaths();
            }
        }
        return jarFilePaths;
    }

    public boolean isHost() {
        if (getNodeAdapters().size() == 1) {
            NodeAdapter node = getNodeAdapters().iterator().next();
            if (MavenUtil.i().isInner(node))
                return true;
        }
        return false;
    }


    /**
     * 使用这个jar替代正在使用的版本
     *
     * @return jar paths
     * @throws Exception
     */
    public List<String> getRepalceCp() {
        List<String> paths = new ArrayList<>();
        paths.addAll(this.getJarFilePaths(true));
        boolean hasRepalce = false;
        for (DepJar usedDepJar : DepJars.i().getUsedDepJars()) {
            if (this.isSameLib(usedDepJar)) {// used depJar instead of usedDepJar.
                if (hasRepalce) {
                    MavenUtil.i().getLog().warn("when cg, find multiple usedLib for " + toString());    //有重复的使用路径
                }
                hasRepalce = true;
            } else {
                paths.addAll(usedDepJar.getJarFilePaths(true));
            }
        }
        if (!hasRepalce) {
            MavenUtil.i().getLog().warn("when cg,can't find mutiple usedLib for " + toString());
        }
        return paths;
    }

    /**
     * get only father jar class paths, used in pruning
     * 只获取父节点，剪枝时使用
     *
     * @param includeSelf : include self
     * @return Set<String> fatherJarCps
     */
    public List<String> getOnlyFatherJarCps(boolean includeSelf) {
        List<String> fatherJarCps = new ArrayList<>();
        for (NodeAdapter node : this.nodeAdapters) {
            fatherJarCps.addAll(node.getImmediateAncestorJarCps(includeSelf));
        }
        return fatherJarCps;
    }

    public List<List<DepJar>> getFatherDep(boolean includeSelf){
        List<List<DepJar>> fatherDeps = new ArrayList<>();
        for (NodeAdapter node : this.nodeAdapters) {
            fatherDeps.add(node.getImmediateAncestorJar(includeSelf));
        }
        return fatherDeps;
    }

    /**
     * @return scope
     */
    public String getScope() {
        String scope = null;
        for (NodeAdapter node : nodeAdapters) {
            scope = node.getScope();
            if (scope != null) {
                break;
            }
        }
        return scope;
    }

    /**
     * @return priority
     */
    public int getPriority() {
        return priority;
    }


}
