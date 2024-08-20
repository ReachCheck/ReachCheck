package dependence.vo;

import dependence.container.DepJars;
import dependence.container.NodeAdapters;
import dependence.util.ClassifierUtil;
import dependence.util.Conf;
import dependence.util.MavenUtil;
import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.resolver.ArtifactNotFoundException;
import org.apache.maven.artifact.resolver.ArtifactResolutionException;
import org.apache.maven.project.MavenProject;
import org.apache.maven.shared.dependency.tree.DependencyNode;

import java.util.*;

public class NodeAdapter {
    protected DependencyNode node;
    protected DepJar depJar;
    protected List<String> filePaths;
    //
    protected int priority;

    protected static DependencyNode tempNode;
    protected static DependencyNode self;

    public NodeAdapter(DependencyNode node) {
        this.node = node;
        if (node != null) {
            resolve();
        }
    }

    public NodeAdapter(DependencyNode node, int priority) {
        this.node = node;
        this.priority = priority;
        if (node != null) {
            resolve();
        }
    }

    public DependencyNode getNode() {
        return node;
    }

    private void resolve() {
        try {
            if (!isInnerProject()) {// inner project is target/classes
                if (null == node.getPremanagedVersion()) {
                    // artifact version of node is the version declared in pom.	节点的构件版本是POM中声明的版本。
                    if (!node.getArtifact().isResolved()) {
                        MavenUtil.i().resolve(node.getArtifact());
                    }
                } else {
                    Artifact artifact = MavenUtil.i().getArtifact(getGroupId(), getArtifactId(), getVersion(),
                            getType(), getClassifier(), getScope());
                    if (!artifact.isResolved()) {
                        MavenUtil.i().resolve(artifact);    //解析这个构件
                    }
                }
            }
        } catch (ArtifactResolutionException | ArtifactNotFoundException e) {
            MavenUtil.i().getLog().warn("cant resolve " + this.toString());
        }
    }

    public String getGroupId() {
        return node.getArtifact().getGroupId();
    }

    public String getScope() {
        return node.getArtifact().getScope();
    }

    public String getArtifactId() {
        return node.getArtifact().getArtifactId();
    }

    public int getState() {
        return node.getState();
    }

    public String getVersion() {
        if (null != node.getPremanagedVersion()) {
            return node.getPremanagedVersion();
        } else {
            return node.getArtifact().getVersion();
        }
    }

    public int getPriority() {
        return priority;
    }

    /**
     * version changes because of dependency management
     * 被dependency management更改过版本
     *
     * @return
     */
    public boolean isVersionChanged() {
        return null != node.getPremanagedVersion();
    }

    /**
     * change it from protected to public
     *
     * @return
     */
    public String getType() {
        return node.getArtifact().getType();
    }

    public String getClassifier() {
        return ClassifierUtil.transformClf(node.getArtifact().getClassifier());
    }

    /**
     * used version is select from this node,if version was from management ,this node will return false.
     * 这个版本的node是否被使用，如果被management更改过版本，将返回false
     *
     * @return
     */
    public boolean isNodeSelected() {
        if (isVersionChanged())
            return false;
        return node.getState() == DependencyNode.INCLUDED;
    }

    public String getManagedVersion() {
        return node.getArtifact().getVersion();
    }

    /**
     * @param includeSelf :whether includes self
     * @return ancestors(from down to top) 从下至上
     */
    public LinkedList<NodeAdapter> getAncestors(boolean includeSelf) {
        LinkedList<NodeAdapter> ancestors = new LinkedList<NodeAdapter>();
        if (includeSelf)
            ancestors.add(this);
        NodeAdapter father = getParent();
        while (null != father) {
            ancestors.add(father);
            father = father.getParent();
        }
        return ancestors;
    }

    /**
     * jar class paths, contains cousins
     *
     * @param includeSelf : whether includes self
     * @return List<String> jarCps
     */
    public Collection<String> getAncestorJarCps(boolean includeSelf, Map<Integer, Integer> map, Map<Integer, Integer> distantRelativesMap) {
        List<String> jarCps = new ArrayList<>();
        Map<Integer, Integer> tempMap = new HashMap<>();
        if (includeSelf)
            jarCps.addAll(this.getFilePath());
        NodeAdapter father = getParent();
        tempNode = node.getParent();
        self = node;
        int level = 0;
        while (null != father) {
            List<NodeAdapter> cousins = getCousins();
//			List<NodeAdapter> needLoads = detectCousins(cousins);
//			MavenUtil.i().getLog().warn("needLoad size : " + needLoads.size());
//			for(NodeAdapter needLoad : needLoads){
//				MavenUtil.i().getLog().warn(needLoad.getSelectedNodeWholeSig());
//			}
            tempMap.put(level++, cousins.size());
//			for(NodeAdapter needLoad : needLoads)
//				jarCps.addAll(needLoad.getFilePath());
            for (NodeAdapter cousion : cousins)
                jarCps.addAll(cousion.getFilePath());
            jarCps.addAll(father.getFilePath());
            father = father.getParent();
        }
//		MavenUtil.i().getLog().warn(node.getArtifact().getArtifactId() + ":" + node.getArtifact().getVersion());
        transformMap(tempMap, map, level);
//		MavenUtil.i().getLog().warn("cousin map");
//		printMap(map);
//		MavenUtil.i().getLog().warn("===========");
        initDistantRelativesMap(map, distantRelativesMap, level);
//		MavenUtil.i().getLog().warn("distantRelativesMap");
//		printMap(distantRelativesMap);

//		long startTime = System.currentTimeMillis();
//
//		long endTime = System.currentTimeMillis();
//		long runTime = (endTime - startTime) / 1000;
        return jarCps;
    }

    /**
     * get immediate ancestor jar class paths, don't contain cousins
     *
     * @param includeSelf : whether includes self
     * @return List<String> jarCps
     */
    public Collection<String> getImmediateAncestorJarCps(boolean includeSelf) {
        Set<NodeAdapter> loadedNodes = new HashSet<>();
        if (includeSelf) {
            loadedNodes.add(NodeAdapters.i().getNodeAdapter(node));
        }
        List<String> jarCps = new ArrayList<>();
        NodeAdapter father = getParent();
        while (null != father) {
            loadedNodes.add(father);
            father = father.getParent();
        }
        for (NodeAdapter loadedNode : loadedNodes) {
            jarCps.addAll(loadedNode.getFilePath());
        }
        return jarCps;
    }

    public List<DepJar> getImmediateAncestorJar(boolean includeSelf) {
        List<NodeAdapter> loadedNodes = new ArrayList<>();
//        if (includeSelf) {
//            loadedNodes.add(NodeAdapters.i().getNodeAdapter(node));
//        }
        List<DepJar> fatherJars = new ArrayList<>();
        NodeAdapter father = getParent();
        while (null != father) {
            loadedNodes.add(father);
            father = father.getParent();
        }
        for (int i = loadedNodes.size() - 1; i >= 0; i--) {
            fatherJars.add(loadedNodes.get(i).depJar);
        }
        if (Conf.transmitJar != null) {
            if (Conf.transmitJar.contains("@@")) {
                for (String per : Conf.transmitJar.split("@@")) {
                    String[] transmitPairs = per.split(":");
                    fatherJars.add(DepJars.i().getDepJar(transmitPairs));
                }
            } else {
                String[] transmitPairs = Conf.transmitJar.split(":");
                fatherJars.add(DepJars.i().getDepJar(transmitPairs));
            }
        }

        if (includeSelf) {
            fatherJars.add(this.depJar);
        }
        return fatherJars;
    }

    /**
     * get only father jar class paths, don't contain cousins and don't consider exclusion
     *
     * @return List<String> jarCps
     */
    public Collection<String> getOnlyFatherJarCps() {
        Set<NodeAdapter> loadedNodes = new HashSet<>();
        List<String> jarCps = new ArrayList<String>();
        NodeAdapter father = getParent();
        while (null != father) {
            loadedNodes.add(father);
            father = father.getParent();
        }
        for (NodeAdapter loadedNode : loadedNodes) {
            jarCps.addAll(loadedNode.getFilePath());
        }
        return jarCps;
    }

    /**
     * init loaded nodes, used to search excluded nodes
     *
     * @param loadedNodes
     * @return Map<String, NodeAdapter> loadedNodesMap
     */
    private Map<String, NodeAdapter> initLoadedNodesMap(Set<NodeAdapter> loadedNodes) {
        Map<String, NodeAdapter> loadedNodesMap = new HashMap<>();
        for (NodeAdapter loadedNode : loadedNodes) {
            String sig = loadedNode.getOnlySelectedNodeSig();
            loadedNodesMap.put(sig, loadedNode);
        }
        return loadedNodesMap;
    }

    /**
     * init loaded nodes, used to search excluded nodes
     *
     * @param needAddNodes
     * @return Map<String, NodeAdapter> loadedNodesMap
     */
    private Map<String, NodeAdapter> initLoadedNodesMap(List<NodeAdapter> needAddNodes) {
        Map<String, NodeAdapter> loadedNodesMap = new HashMap<>();
        for (NodeAdapter loadedNode : needAddNodes) {
            String sig = loadedNode.getOnlySelectedNodeSig();
            loadedNodesMap.put(sig, loadedNode);
        }
        return loadedNodesMap;
    }

    /**
     * if the node is excluded, add it
     *
     * @param loadedNodesMap
     * @return List<NodeAdapter> needAddNodes
     */
    private List<NodeAdapter> addExcludeNodes(Map<String, NodeAdapter> loadedNodesMap) {
        List<NodeAdapter> needAddNodes = new ArrayList<>();
        for (Map.Entry<String, List<NodeAdapter>> entry : Conf.dependencyMap.entrySet()) {
            if (loadedNodesMap.containsKey(entry.getKey())) {
                needAddNodes.addAll(entry.getValue());
            }
        }
        return needAddNodes;
    }


    /**
     * old version
     */
    private void transformMap(Map<Integer, Integer> tempMap, Map<Integer, Integer> map, int level) {
        for (int oriLevel : tempMap.keySet()) {
            map.put(level - oriLevel, tempMap.get(oriLevel));
        }
        map.put(0, 0);
    }

    /**
     * get distant relative node map 得到远房亲戚节点图， old version
     *
     * @param level 层次遍历层数
     */
    private void initDistantRelativesMap(Map<Integer, Integer> map, Map<Integer, Integer> distantRelativesMap, int level) {
        String oriSig = node.getArtifact().getGroupId() + ":" + node.getArtifact().getArtifactId();
        for (int tempLevel : map.keySet()) {
            List<String> allNode = Conf.list.get(tempLevel);
            if (!allNode.isEmpty()) {
                int cousinsIncludeSelf = map.get(tempLevel) + 1;
                int val = allNode.size() - cousinsIncludeSelf;
                if (allNode.contains(oriSig)) {
                    --val;
                }
                if (tempLevel == level) {
                    ++val;
                }
                if (val < 0) {
                    val = 0;
                }
                distantRelativesMap.put(tempLevel, val);
            } else {
                distantRelativesMap.put(tempLevel, 0);
            }
        }
    }

    /**
     * 得到父节点
     *
     * @return
     */
    public NodeAdapter getParent() {
        if (null == node.getParent())
            return null;
        return NodeAdapters.i().getNodeAdapter(node.getParent());
    }

    /**
     * get node's cousins, old version
     *
     * @return List<NodeAdapter> cousins
     */
    private List<NodeAdapter> getCousins() {
        List<NodeAdapter> cousins = new ArrayList<>();
        String oriSig = node.getArtifact().getGroupId() + ":" + node.getArtifact().getArtifactId();
        if (null == tempNode.getChildren())
            return null;
        for (DependencyNode child : tempNode.getChildren()) {
            String tempSig = child.getArtifact().getGroupId() + ":" + child.getArtifact().getArtifactId();
            if (!child.equals(self) && !oriSig.equals(tempSig) && NodeAdapters.i().getNodeAdapter(child).isNodeSelected()) {
                cousins.add(NodeAdapters.i().getNodeAdapter(child));
            }
        }
        self = tempNode;
        tempNode = tempNode.getParent();
        return cousins;
    }

    public String getOnlySelectedNodeSig() {
        return getGroupId() + ":" + getArtifactId();
    }

    /**
     * 得到文件路径
     *
     * @return
     */
    public List<String> getFilePath() {
        if (filePaths == null) {
            filePaths = new ArrayList<>();
            if (isInnerProject()) {// inner project is target/classes
//                filePaths.add(MavenUtil.i().getMavenProject(this).getBuild().getOutputDirectory());
                filePaths = MavenUtil.i().getSrcPaths();
                // filePaths = UtilGetter.i().getSrcPaths();
            } else {// dependency is repository address

                try {
                    if (null == node.getPremanagedVersion()) {
                        filePaths.add(node.getArtifact().getFile().getAbsolutePath());
                    } else {
                        Artifact artifact = MavenUtil.i().getArtifact(getGroupId(), getArtifactId(), getVersion(),
                                getType(), getClassifier(), getScope());
                        if (!artifact.isResolved())
                            MavenUtil.i().resolve(artifact);
                        filePaths.add(artifact.getFile().getAbsolutePath());
                    }
                } catch (ArtifactResolutionException | ArtifactNotFoundException e) {
                    MavenUtil.i().getLog().warn("cant resolve " + this.toString());
                }

            }
        }
        MavenUtil.i().getLog().debug("node filepath for " + toString() + " : " + filePaths);
        return filePaths;

    }

    public boolean isInnerProject() {
        return MavenUtil.i().isInner(this);
    }

    /**
     * judge whether is self
     *
     * @return boolean
     */
    public boolean isSelf(DependencyNode node2) {
        return node.equals(node2);
    }

    /**
     * judge whether is self
     *
     * @return boolean
     */
    public boolean isSelf(MavenProject mavenProject) {
        return isSelf(mavenProject.getGroupId(), mavenProject.getArtifactId(), mavenProject.getVersion(),
                ClassifierUtil.transformClf(mavenProject.getArtifact().getClassifier()));
    }

    /**
     * judge whether is self
     *
     * @return boolean
     */
    public boolean isSelf(String groupId2, String artifactId2, String version2, String classifier2) {
        return getGroupId().equals(groupId2) && getArtifactId().equals(artifactId2) && getVersion().equals(version2)
                && getClassifier().equals(classifier2);
    }

    /**
     * judge whether is the same lib
     *
     * @param nodeAdapter to be compared
     * @return
     */
    public boolean isSameLib(NodeAdapter nodeAdapter) {
        return getGroupId().equals(nodeAdapter.getGroupId()) && getArtifactId().equals(nodeAdapter.getArtifactId());
    }

    public MavenProject getSelfMavenProject() {
        return MavenUtil.i().getMavenProject(this);
    }

    public DepJar getDepJar() {
        if (depJar == null)
            depJar = DepJars.i().getDep(this);
        return depJar;
    }

    @Override
    public String toString() {
        String scope = getScope();
        if (null == scope)
            scope = "";
        return getGroupId() + ":" + getArtifactId() + ":" + getVersion() + ":" + getClassifier() + ":" + scope + " priority : " + priority;
    }

    /**
     * get the groupId and artifactId of the node in the original dependency tree
     *
     * @return
     */
    public String getSig() {
        return getGroupId() + ":" + getArtifactId();
    }


    /**
     * get invoke path included self
     *
     * @return string
     */
    public String getWholePath() {
        StringBuilder sb = new StringBuilder(toString());
        NodeAdapter father = getParent();
        while (null != father) {
            sb.insert(0, father.toString() + " + ");
            father = father.getParent();
        }
        return sb.toString();
    }

    public NodeAdapter getPreNode() {
        NodeAdapter father = getParent();
        NodeAdapter preNode = null;
        while (null != father && !father.getDepJar().isHost()) {
            preNode = father;
            father = father.getParent();
        }
        return preNode;
    }
}
