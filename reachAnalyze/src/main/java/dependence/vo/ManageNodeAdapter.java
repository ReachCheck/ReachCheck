package dependence.vo;

import dependence.util.MavenUtil;
import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.resolver.ArtifactNotFoundException;
import org.apache.maven.artifact.resolver.ArtifactResolutionException;
import org.apache.maven.shared.dependency.tree.DependencyNode;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * some depjar may be from dependency management instead of dependency tree.We
 * design ManageNodeAdapter for depJar of this type.
 */
public class ManageNodeAdapter extends NodeAdapter {
    private String groupId;
    private String artifactId;// artifactId
    private String version;// version
    private String classifier;
    private String type;
    private String scope;
    private Artifact artifact;

    public ManageNodeAdapter(NodeAdapter nodeAdapter) {
        super(null);
        groupId = nodeAdapter.getGroupId();
        artifactId = nodeAdapter.getArtifactId();
        version = nodeAdapter.getManagedVersion();
        classifier = nodeAdapter.getClassifier();
        type = nodeAdapter.getType();
        scope = nodeAdapter.getScope();

        try {
            artifact = MavenUtil.i().getArtifact(getGroupId(), getArtifactId(), getVersion(), getType(),
                    getClassifier(), getScope());
            if (!artifact.isResolved())
                MavenUtil.i().resolve(artifact);

        } catch (ArtifactResolutionException | ArtifactNotFoundException e) {
            MavenUtil.i().getLog().warn("cant resolve " + this.toString());
        }
    }

    public String getGroupId() {
        return groupId;
    }

    public String getArtifactId() {
        return artifactId;
    }

    public String getVersion() {
        return version;
    }

    public String getClassifier() {
        return classifier;
    }

    public boolean isNodeSelected() {
        return true;
    }

    public boolean isVersionSelected() {
        return true;
    }

    public String getManagedVersion() {
        return version;
    }

    @Override
    public Collection<String> getOnlyFatherJarCps() {
        return super.getOnlyFatherJarCps();
    }

    public NodeAdapter getParent() {
        return null;
    }

    public String getType() {
        return type;
    }

    public String getScope() {
        return scope;
    }

    public boolean isVersionChanged() {
        return false;
    }

    public List<String> getFilePath() {
        if (filePaths == null) {
            filePaths = new ArrayList<String>();
            if (isInnerProject()) {// inner project is target/classes
                // filePaths = UtilGetter.i().getSrcPaths();
                filePaths.add(MavenUtil.i().getMavenProject(this).getBuild().getOutputDirectory());
            } else {// dependency is repository address
                String path = artifact.getFile().getAbsolutePath();
                filePaths.add(path);
            }
        }
        MavenUtil.i().getLog().debug("node filepath for " + toString() + " : " + filePaths);
        return filePaths;
    }

    public boolean isSelf(DependencyNode node2) {
        return false;
    }
}
