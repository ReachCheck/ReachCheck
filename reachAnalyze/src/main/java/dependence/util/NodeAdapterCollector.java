package dependence.util;

import dependence.container.NodeAdapters;
import dependence.vo.NodeAdapter;
import org.apache.maven.shared.dependency.tree.DependencyNode;
import org.apache.maven.shared.dependency.tree.traversal.DependencyNodeVisitor;

import java.util.HashSet;
import java.util.Set;

public class NodeAdapterCollector implements DependencyNodeVisitor {
    private static Set<String> longTimeLib;// lib that takes a long time to get call-graph.

    static {
        longTimeLib = new HashSet<String>();
        longTimeLib.add("org.scala-lang:scala-library");
        longTimeLib.add("org.clojure:clojure");
    }

    private NodeAdapters nodeAdapters;

    private static int priority = 0;

    public NodeAdapterCollector(NodeAdapters nodeAdapters) {
        this.nodeAdapters = nodeAdapters;
    }

    public boolean visit(DependencyNode node) {

        MavenUtil.i().getLog().debug(node.toNodeString() + " type:" + node.getArtifact().getType() + " version"
                + node.getArtifact().getVersionRange() + " selected:" + (node.getState() == DependencyNode.INCLUDED));

        if (Conf.DEL_LONGTIME) {
            if (longTimeLib.contains(node.getArtifact().getGroupId() + ":" + node.getArtifact().getArtifactId())) {
                return false;
            }
        }

        if (Conf.DEL_OPTIONAL) {
            if (node.getArtifact().isOptional()) {
                return false;
            }
        }

        // 是否过滤掉scope为provided的
        if (MavenUtil.i().getMojo().ignoreProvidedScope) {
            if ("provided".equals(node.getArtifact().getScope())) {
                return false;
            }
        }

        // 是否过滤掉scope为test的
        if (MavenUtil.i().getMojo().ignoreTestScope) {
            if ("test".equals(node.getArtifact().getScope())) {
                return false;
            }
        }

        // 是否过滤掉classifier为test的
        if (MavenUtil.i().getMojo().ignoreTestClassifier) {
            if (ClassifierUtil.transformClf(node.getArtifact().getClassifier()).contains("test")){
                return false;
            }
        }

        // 是否过滤掉scope为runtime的
        if (MavenUtil.i().getMojo().ignoreRuntimeScope) {
            if ("runtime".equals(node.getArtifact().getScope())) {
                return false;
            }
        }

        if (node.getState() == 0) {
            /*这里的优先级是加载类的优先级，将会在检测完全限定名相同时用到*/
            nodeAdapters.addNodeAapter(new NodeAdapter(node, priority));
            priority++;
        } else {
            nodeAdapters.addNodeAapter(new NodeAdapter(node, -1));
        }
        return true;
    }

    public boolean endVisit(DependencyNode node) {
        return true;
    }
}
