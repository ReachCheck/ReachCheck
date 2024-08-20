package dependence.util;

import dependence.ConflictMojo;
import dependence.vo.NodeAdapter;
import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.resolver.ArtifactNotFoundException;
import org.apache.maven.artifact.resolver.ArtifactResolutionException;
import org.apache.maven.artifact.versioning.InvalidVersionSpecificationException;
import org.apache.maven.artifact.versioning.VersionRange;
import org.apache.maven.plugin.logging.Log;
import org.apache.maven.project.MavenProject;

import java.io.File;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class MavenUtil {
	private static final MavenUtil instance = new MavenUtil();
	private Set<String> hostClses;
	private String srcJarPath;

	public static MavenUtil i() {
		return instance;
	}

	private MavenUtil() {

	}

	/**
	 * get host classes
	 * @return Set<String> hostClses
	 */
	private Set<String> getHostClses() {
		if (hostClses == null) {
			hostClses = new HashSet<>();
			if (null != this.getSrcPaths()) {
				for (String srcDir : this.getSrcPaths()) {
					hostClses.addAll(Util.getJarClasses(srcDir));
				}
			}
		}
		return hostClses;
	}

	/**
	 * verify whether is host class
	 * @param clsSig : class signature
	 * @return boolean
	 */
	public boolean isHostClass(String clsSig) {
		return getHostClses().contains(clsSig);
	}

	private ConflictMojo mojo;

	public boolean isInner(NodeAdapter nodeAdapter) {
		return nodeAdapter.isSelf(mojo.project);
	}

	public MavenProject getMavenProject(NodeAdapter nodeAdapter) {
		for (MavenProject mavenProject : mojo.reactorProjects) {
			if (nodeAdapter.isSelf(mavenProject))
				return mavenProject;
		}
		return null;
	}

	public void setMojo(ConflictMojo mojo) {
		this.mojo = mojo;
	}

	public void resolve(Artifact artifact) throws ArtifactResolutionException, ArtifactNotFoundException {
		mojo.resolver.resolve(artifact, mojo.remoteRepositories, mojo.localRepository);
	}

	public Log getLog() {
		return mojo.getLog();
	}

	public Artifact getArtifact(String groupId, String artifactId, String versionRange, String type, String classifier,
			String scope) {
		try {
			return mojo.factory.createDependencyArtifact(groupId, artifactId,
					VersionRange.createFromVersionSpec(versionRange), type, classifier, scope);
		} catch (InvalidVersionSpecificationException e) {
			getLog().error("cant create Artifact!", e);
			return null;
		}
	}


	/**
	 * project info
	 * @return groupId:artifactId:version@filePath
	 */
	public String getProjectInfo() {
		return mojo.project.getGroupId() + ":" + mojo.project.getArtifactId() + ":" + mojo.project.getVersion() + "@"
				+ mojo.project.getFile().getAbsolutePath();
	}

	public String getProjectName() {
		return mojo.project.getName();
	}

	public String getSrcJarPath(){
		return srcJarPath;
	}

	/**
	 * 得到项目pom.xml的位置
	 * @return
	 */
	public String getProjectPom() {
		return mojo.project.getFile().getAbsolutePath();
	}

	/**
	 * @return groupId:artifactId:version
	 */
	public String getProjectCor() {
		return mojo.project.getGroupId() + ":" + mojo.project.getArtifactId() + ":" + mojo.project.getVersion();
	}

	/**
	 * @return groupId
	 */
	public String getProjectGroupId() {
		return mojo.project.getGroupId();
	}

	/**
	 * @return artifactId
	 */
	public String getProjectArtifactId() {
		return mojo.project.getArtifactId();
	}

	/**
	 * @return version
	 */
	public String getProjectVersion() {
		return mojo.project.getVersion();
	}

	/**
	 * @return ConflictMojo mojo
	 */
	public ConflictMojo getMojo() {
		return mojo;
	}	
	
	/**D:\cWS\eclipse1\testcase.top
	 * @return
	 */
	public File getBaseDir() {
		return mojo.project.getBasedir();
	}

	public File getBuildDir() {
		return mojo.buildDir;
	}

	/**
	 * get host src path
	 * @return src path
	 */
	public List<String> getSrcPaths() {
		List<String> srcPaths = new ArrayList<String>();
		if(!srcJarPath.equals("default")) {
			srcPaths.add(srcJarPath);
		}
		if(this.mojo==null) {
			return null;
		}
		for (String srcPath : this.mojo.compileSourceRoots) {
			if (new File(srcPath).exists())
				srcPaths.add(srcPath);
		}
		return srcPaths;
	}

	public void setSrcJarPath(String path){
		srcJarPath = path;
	}

	/**
	 * get maven local repository
	 * use File.separator
	 * @return local repository
	 */
	public String getMvnRep() {
		return this.mojo.localRepository.getBasedir() + File.separator;
	}
}
