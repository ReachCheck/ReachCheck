package dependence.vo;

import dependence.util.MavenUtil;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Conflict {
    private String groupId;
    private String artifactId;

    private Set<NodeAdapter> nodes;
    private Set<DepJar> depJars;
    private DepJar usedDepJar;

    public Conflict(String groupId, String artifactId) {
        nodes = new HashSet<>();
        this.groupId = groupId;
        this.artifactId = artifactId;
    }

    /**
     * 得到使用的DepJar
     *
     * @return usedDepJar
     */
    public DepJar getUsedDepJar() {
        if (null == usedDepJar) {
            for (DepJar depJar : depJars) {
                if (depJar.isSelected()) {
                    if (null != usedDepJar)
                        MavenUtil.i().getLog()
                                .warn("duplicate used version for dependency:" + groupId + ":" + artifactId);
                    usedDepJar = depJar;
                }
            }
        }
        return usedDepJar;

    }

    /**
     * 设置usedDepJar
     */
    public void setUsedDepJar(DepJar depJar) {
        usedDepJar = depJar;
    }


    public void addNode(NodeAdapter nodeAdapter) {
        nodes.add(nodeAdapter);
    }

    /**
     * 同一个构件
     *
     * @param groupId2
     * @param artifactId2
     * @return
     */
    public boolean sameArtifact(String groupId2, String artifactId2) {
        return groupId.equals(groupId2) && artifactId.equals(artifactId2);
    }

    /**
     * get all dep jars, no parameter
     *
     * @return depJars
     */
    public Set<DepJar> getDepJars() {
        if (depJars == null) {
            depJars = new HashSet<>();
            for (NodeAdapter nodeAdapter : nodes) {
                depJars.add(nodeAdapter.getDepJar());
            }
        }
        return depJars;
    }

    /**
     * get all depjars of the new conflict after repairing
     *
     * @param depJar
     * @return
     *
     */
    public Set<DepJar> getDepJars(Set<DepJar> depJar) {
        depJars = new HashSet<>();
        for (DepJar jar : depJar) {
            if (jar.getGroupId().equals(this.getGroupId()) && jar.getArtifactId().equals(this.getArtifactId())) {
                depJars.add(jar);
            }
        }
        return depJars;
    }

    public Set<NodeAdapter> getNodeAdapters() {
        return this.nodes;
    }

    /**
     * verify whither is conflict, when size > 1, return true
     *
     * @return boolean
     */
    public boolean isConflict() {
        return getDepJars().size() > 1;
    }



    @Override
    public String toString() {
        StringBuilder str = new StringBuilder(groupId + ":" + artifactId + " conflict version:");
        for (DepJar depJar : depJars) {
            str.append(depJar.getVersion()).append(":").append(depJar.getClassifier()).append("-");
        }
        str.append("---used jar:").append(getUsedDepJar().getVersion()).append(":").append(getUsedDepJar().getClassifier());
        return str.toString();
    }

    public String getConflict() {
        return groupId + "." + artifactId + "+" + artifactId;
    }

    public String getGroupId() {
        return groupId;
    }

    public String getArtifactId() {
        return artifactId;
    }

    public String getSig() {
        return getGroupId() + ":" + getArtifactId();
    }

    /**
     * @return first version is the used version
     */
    public List<String> getVersions() {
        List<String> versions = new ArrayList<>();
        versions.add(getUsedDepJar().getVersion());
        for (DepJar depJar : depJars) {
            String version = depJar.getVersion();
            if (!versions.contains(version)) {
                versions.add("/" + version);
            }
        }
        return versions;
    }

    public int getSize() {
        return nodes.size();
    }

}
