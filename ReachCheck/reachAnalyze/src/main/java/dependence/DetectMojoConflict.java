package dependence;

import dependence.container.Conflicts;
import dependence.util.Conf;
import dependence.vo.Conflict;
import dependence.util.MavenUtil;
import dependence.vo.DepJar;
import detect.*;
import org.apache.maven.plugins.annotations.LifecyclePhase;
import org.apache.maven.plugins.annotations.Mojo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

@Mojo(name = "detectConflict", defaultPhase = LifecyclePhase.VALIDATE)
public class DetectMojoConflict extends ConflictMojo {
    Map<String, Analyze2> jarMap = new HashMap<>();
    long totalTime = 0;

    @Override
    public void run() {
        if(Conflicts.i().getConflicts().size() == 0){
            MavenUtil.i().getLog().info("The project has no dependency conflicts.");
        }
        try {
            long startTimeMillis = System.currentTimeMillis();
            ProjectAna projectAna = new ProjectAna(Conf.targetJar);
            Set<String> libMtds = projectAna.getLibMtds();
            totalTime += System.currentTimeMillis()-startTimeMillis;
            for (Conflict conflict : Conflicts.i().getConflicts()) {
                DepJar useJar = conflict.getUsedDepJar();
                Set<DepJar> conflictJars = conflict.getDepJars();
                MavenUtil.i().getLog().info("JAR packages loaded by Maven facing dependency conflicts："+useJar.getSig());
                judgeConflict(conflictJars,useJar,libMtds);
            }
            MavenUtil.i().getLog().info("The time consumed for detecting dependency conflicts :"+(totalTime));
        }
        catch (IOException e){
            e.printStackTrace();
        }
    }

    void judgeConflict(Set<DepJar>conflictJars,DepJar useJar,Set<String>inputApis){
        for(DepJar conflictJar:conflictJars){
            long startTimeMillis = System.currentTimeMillis();
            if(!conflictJar.getSig().equals(useJar.getSig())){
                MavenUtil.i().getLog().info("==================================================================");
                MavenUtil.i().getLog().info("Conflict jar: "+conflictJar.getSig());
                if(isApiSame(conflictJar,useJar)){
                    totalTime += System.currentTimeMillis()-startTimeMillis;
                    MavenUtil.i().getLog().info("This conflict will not have any impact.");
                    continue;
                }
                Set<String> conflictApis = getConflictApis(useJar,conflictJar);
                List<List<DepJar>> parentJars = conflictJar.getFatherDep(false);
                Set<String> temp = new HashSet<>(inputApis);
                temp = getReachApis(temp,parentJars);
                if(temp == null || temp.size() == 0){
                    totalTime += System.currentTimeMillis()-startTimeMillis;
                    MavenUtil.i().getLog().info("This conflict will not have any impact.");
                    continue;
                }
                Analyze2 ana = jarMap.computeIfAbsent(conflictJar.getSig(), key -> new Analyze2(conflictJar.getGroupId(), conflictJar.getArtifactId(), conflictJar.getVersion(),getJarPath(conflictJar)));
                Set<String> reachVulApi;
                if(ana.reachInfoExist(false)){
                    reachVulApi = ana.query(temp,conflictApis);
                }
                else{
                    MavenUtil.i().getLog().info("Local reachability index for targetJar:"+useJar.getSig()+" does not exist, unable to perform reachability analysis.");
                    return;
                }
                totalTime += System.currentTimeMillis()-startTimeMillis;
                if(reachVulApi == null || reachVulApi.size() == 0){
                    totalTime += System.currentTimeMillis()-startTimeMillis;
                    MavenUtil.i().getLog().info("This conflict will not have any impact.");
                }
                else{
                    for(String conflictApi:reachVulApi){
                        MavenUtil.i().getLog().info("The project calls conflicting API: "+conflictApi);
                    }
                }
            }
        }
        MavenUtil.i().getLog().info("******************************************************************");
    }

    String getJarPath(DepJar jar){
        File jarFile = new File(jar.getJarFilePaths(true).get(0));
        return jarFile.getParent();
    }

    Set<String> getReachApis(Set<String> inputs,List<List<DepJar>> depJars){
        Set<String> reachApis = new HashSet<>();
        for(List<DepJar>parentJars:depJars){
            Set<String>temp = new HashSet<>(inputs);
            List<DepJar>removes = new ArrayList<>();
            for(int i=1;i<parentJars.size();i++){ // skip hostJar
                DepJar jar = parentJars.get(i);
                Analyze2 ana = jarMap.computeIfAbsent(jar.getSig(), key -> new Analyze2(jar.getGroupId(), jar.getArtifactId(), jar.getVersion(),getJarPath(jar)));
                if(!ana.reachInfoExist(true)){
                    if(ana.emptyFlag){
                        removes.add(jar);
                        continue;
                    }
                    MavenUtil.i().getLog().info("Local reachability index for "+jar.getSig()+" does not exist, unable to perform reachability analysis.");
                    return null;
                }
                ana.getOutput(temp);
                if(temp.size() == 0){
                    break;
                }
            }
            for(DepJar jar:removes){
                parentJars.remove(jar);
            }
            reachApis.addAll(temp);
        }
        return reachApis;
    }

    boolean isApiSame(DepJar conflictJar,DepJar useJar){
        String useJarPath = getPath(useJar);
        String conflictJarPath = getPath(conflictJar);
        String useJarHashVal = getHashValue(useJarPath);
        String conflictJarHashVal = getHashValue(conflictJarPath);
        if(useJarHashVal.equals(conflictJarHashVal)){
            return true;
        }
        return false;
    }

    String getPath(DepJar jar){
        return Conf.localMvnRepoPath + "/" + jar.getGroupId().replace(".","/") + "/" + jar.getArtifactId() + "/" + jar.getVersion();
    }

    String getHashValue(String path){
        File file = new File(path+"/public_methods.txt.sha1");
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String hashValue = reader.readLine();
            if (hashValue != null) {
                return hashValue;
            }
        }catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    Set<String> getConflictApis(DepJar useJar,DepJar conflictJar){
        Set<String> useJarApis = getPublicApis(getPath(useJar));
        Set<String> conflictJarApis = getPublicApis(getPath(conflictJar));
        conflictJarApis.removeAll(useJarApis);
        return conflictJarApis;
    }

    Set<String> getPublicApis(String path){
        Set<String> publicApis = new HashSet<>();
        File file = new File(path+"/public_methods.txt");
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                publicApis.add(line);
            }
        }catch (IOException e) {
            e.printStackTrace();
        }
        return publicApis;
    }

}
