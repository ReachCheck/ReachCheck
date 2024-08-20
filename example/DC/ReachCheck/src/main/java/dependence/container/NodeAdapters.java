package dependence.container;

import dependence.util.MavenUtil;
import dependence.util.NodeAdapterCollector;
import dependence.vo.DepJar;
import dependence.vo.ManageNodeAdapter;
import dependence.vo.NodeAdapter;
import org.apache.maven.shared.dependency.tree.DependencyNode;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class NodeAdapters {
    private static NodeAdapters instance;

    public static NodeAdapters i() {
        return instance;
    }

    /**
     * 初始化
     * 依赖树扁平化
     * 并且将经过dependencymanagement的节点manage之前的那个版本虚构出来（一般被manage的都是依赖的依赖，也就是说不是项目直接依赖，所以被manage了就有些难受）。
     * （代码细节）在manageNds（一个临时变量）中找，如果找到了一样的构件，说明已经虚构过，不再虚构，否则虚构，并且加入manageNds中。
     * 将manageNds中这些虚构出来的节点放进NodeAdapters的容器中去。
     *
     * @param root 根节点
     */
    public static void init(DependencyNode root) {
        instance = new NodeAdapters();
        NodeAdapterCollector visitor = new NodeAdapterCollector(instance);
        root.accept(visitor);
        // add management node
        List<NodeAdapter> manageNds = new ArrayList<>();
        for (NodeAdapter nodeAdapter : instance.container) {
            if (nodeAdapter.isVersionChanged()) {// this node have management
                if (null == instance.getNodeAdapter(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId(),
                        nodeAdapter.getManagedVersion(), nodeAdapter.getClassifier())) {
                    // this managed-version doesnt have used node,we should new a virtual node to
                    // find conflict
                    NodeAdapter manageNd = null;
                    for (NodeAdapter existManageNd : manageNds) {// find if manageNd exists
                        if (existManageNd.isSelf(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId(),
                                nodeAdapter.getManagedVersion(), nodeAdapter.getClassifier())) {
                            manageNd = existManageNd;
                            break;
                        }
                    }
                    if (null == manageNd) {//dont exist manageNd,should new and add
                        manageNd = new ManageNodeAdapter(nodeAdapter);
                        manageNds.add(manageNd);
                    }
                }
            }
        }
        for (NodeAdapter manageNd : manageNds) {
            instance.addNodeAapter(manageNd);
        }
        //}
    }

    private final List<NodeAdapter> container;

    private NodeAdapters() {
        container = new ArrayList<>();
    }

    public void addNodeAapter(NodeAdapter nodeAdapter) {
        container.add(nodeAdapter);
    }

    /**
     * 根据node获得对应的adapter
     * 这里如何判断node相等呢？只是简单根据引用相等来判断，并不参考artifactid之类的。
     *
     * @param node 依赖节点
     */
    public NodeAdapter getNodeAdapter(DependencyNode node) {
        for (NodeAdapter nodeAdapter : container) {
            if (nodeAdapter.isSelf(node))
                return nodeAdapter;
        }
        if(node == null){

        }
        MavenUtil.i().getLog().warn("cant find nodeAdapter for node:" + node.toNodeString());
        return null;
    }

    /**
     * 根据groupId，artifactId，version和classifier获得对应的adapter
     *
     * @param groupId2    : 目标groupId
     * @param artifactId2 : 目标artifactId
     * @param version2    : 目标version
     * @param classifier2 : 目标classifier
     * @return nodeAdapter
     */
    public NodeAdapter getNodeAdapter(String groupId2, String artifactId2, String version2, String classifier2) {
        for (NodeAdapter nodeAdapter : container) {
            if (nodeAdapter.isSelf(groupId2, artifactId2, version2, classifier2))
                return nodeAdapter;
        }
        MavenUtil.i().getLog().warn("cant find nodeAdapter for management node:" + groupId2 + ":" + artifactId2 + ":"
                + version2 + ":" + classifier2);
        return null;
    }

    /**
     * 根据depJar获得adapter
     *
     * @param depJar 依赖
     * @return 返回依赖所属的节点
     */
    public Set<NodeAdapter> getNodeAdapters(DepJar depJar) {
        Set<NodeAdapter> result = new HashSet<>();
        for (NodeAdapter nodeAdapter : container) {
            if (nodeAdapter.getDepJar() == depJar) {
                result.add(nodeAdapter);
            }
        }
        if (result.size() == 0)
            MavenUtil.i().getLog().warn("cant find nodeAdapter for depJar:" + depJar.toString());
        return result;
    }

    public List<NodeAdapter> getAllNodeAdapter() {
        return container;
    }

}
