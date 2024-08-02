package dependence.container;

import dependence.util.Conf;
import dependence.vo.Conflict;
import dependence.vo.NodeAdapter;

import java.util.ArrayList;
import java.util.List;

public class Conflicts {
    private static Conflicts instance;

    public static void init(NodeAdapters nodeAdapters) {
        instance = new Conflicts(nodeAdapters);
    }

    public static Conflicts i() {
        return instance;
    }

    private final List<Conflict> container;

    /**
     * must initial NodeAdapters before this construct
     */
    private Conflicts(NodeAdapters nodeAdapters) {
        container = new ArrayList<>();
        for (NodeAdapter node : nodeAdapters.getAllNodeAdapter()) {
            addNodeAdapter(node);
        }
        //如果这个方法不是需要的冲突
        container.removeIf(conflict -> !conflict.isConflict() || !wantCal(conflict));
    }

    /**
     *
     * @param conflict 指定检测的冲突
     * @return 是否查找到
     */
    private boolean wantCal(Conflict conflict) {
        if (Conf.callConflict != null && !"".equals(Conf.callConflict)) {
            return conflict.getSig().equals(Conf.callConflict.replace("+", ":"));
        } else {
            return true;
        }
    }

    public List<Conflict> getConflicts() {
        return container;
    }

    /**
     * 如果容器中已经存在一个conflict和本nodeAdapter是相同的构件
     * 则为这个conflict添加本节点适配器
     * 如果容器中不存在
     * 则本nodeAdapter作为一个conflict加入容器
     * 然后为这个conflict加入本节点
     *
     * @param nodeAdapter 节点
     */
    private void addNodeAdapter(NodeAdapter nodeAdapter) {
        Conflict conflict = null;
        for (Conflict existConflict : container) {
            if (existConflict.sameArtifact(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId())) {
                conflict = existConflict;
            }
        }
        if (null == conflict) {
            conflict = new Conflict(nodeAdapter.getGroupId(), nodeAdapter.getArtifactId());
            container.add(conflict);
        }
        conflict.addNode(nodeAdapter);
    }

    @Override
    public String toString() {
        StringBuilder str = new StringBuilder("project has " + container.size() + " conflict-dependency:+\n");
        for (Conflict conflictDep : container) {
            str.append(conflictDep.toString()).append("\n");
        }
        return str.toString();
    }
}
