package dependence;

import dependence.container.DepJars;
import dependence.util.Conf;
import dependence.util.MavenUtil;
import dependence.vo.DepJar;
import detect.Analyze2;
import detect.ProjectAna;
import org.apache.maven.plugins.annotations.LifecyclePhase;
import org.apache.maven.plugins.annotations.Mojo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

@Mojo(name = "detectVul", defaultPhase = LifecyclePhase.VALIDATE)
public class DetectMojoVul extends ConflictMojo {
    Map<String, Analyze2> jarMap = new HashMap<>();
    Map<DepJar,Set<String>>vulJarApis = new HashMap<>();
    long totalTime = 0;
    @Override
    public void run() {
        Set<DepJar> depJars = DepJars.i().getAllDepJar();
        Map<DepJar, List<List<DepJar>>>  vulJars = new HashMap<>();
        for(DepJar jar:depJars){
            if(jar.isHost()){
                continue;
            }
            String groupId = String.join("/", jar.getGroupId().split("\\."));
            String artifactId = jar.getArtifactId();
            String version = jar.getVersion();
            String jarDir = MavenUtil.i().getMvnRep() + groupId + "/" + artifactId + "/" + version;
            File vulFile = new File(jarDir+"/cve_apis.txt");
            if(vulFile.exists()){
                vulJars.put(jar,jar.getFatherDep(false));
                readVulApis(jarDir+"/cve_apis.txt",jar);
            }
        }
        if(vulJars.size() == 0){
            MavenUtil.i().getLog().info("The project's dependent JAR packages have no vulnerabilities.");
        }
        else{
            try {
                long startTimeMillis = System.currentTimeMillis();
                ProjectAna projectAna = new ProjectAna(Conf.targetJar);
                Set<String> libMtds = projectAna.getLibMtds();
                totalTime += System.currentTimeMillis()-startTimeMillis;
                for (Map.Entry<DepJar, List<List<DepJar>>> entry : vulJars.entrySet()) {
                    DepJar vulJar = entry.getKey();
                    List<List<DepJar>> parentJars = entry.getValue();
                    judgeVul(libMtds,vulJar,parentJars);
                }
                MavenUtil.i().getLog().info("The time consumed for detecting vul Apis : "+(totalTime)+"ms");
            }
            catch (Exception e){
                e.printStackTrace();
            }
        }
    }

    void judgeVul(Set<String>inputApis,DepJar vulJar,List<List<DepJar>>parentJars){
        long startTimeMillis = System.currentTimeMillis();
        Set<String> vulApis = vulJarApis.get(vulJar);
        Set<String> temp = new HashSet<>(inputApis);
        temp = getReachApis(temp,parentJars);
        if(temp == null || temp.size() == 0){
            totalTime += System.currentTimeMillis()-startTimeMillis;
            MavenUtil.i().getLog().info("The project does not call vulnerable APIs of "+vulJar.getSig());
            return;
        }
        Analyze2 ana = jarMap.computeIfAbsent(vulJar.getSig(), key -> new Analyze2(vulJar.getGroupId(), vulJar.getArtifactId(), vulJar.getVersion(),getJarPath(vulJar)));
        Set<String> reachVulApi;
        if(ana.reachInfoExist(false)){
            reachVulApi = ana.query(temp,vulApis);
        }
        else{
            MavenUtil.i().getLog().info("Local reachability index for targetJar:"+vulJar.getSig()+" does not exist, unable to perform reachability analysis.");
            return;
        }
        totalTime += System.currentTimeMillis()-startTimeMillis;
        if(reachVulApi == null || reachVulApi.size() == 0){
            totalTime += System.currentTimeMillis()-startTimeMillis;
            MavenUtil.i().getLog().info("The project does not call vulnerable APIs of "+vulJar.getSig());
        }
        else{
            for(String vulApi:reachVulApi){
                MavenUtil.i().getLog().info("The project calls vul API of "+vulJar.getSig()+":  "+vulApi);
            }
        }
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
            for(int i=1;i<parentJars.size();i++){
                DepJar jar = parentJars.get(i);
                Analyze2 ana = jarMap.computeIfAbsent(jar.getSig(), key -> new Analyze2(jar.getGroupId(), jar.getArtifactId(), jar.getVersion(),getJarPath(jar)));
                if(!ana.reachInfoExist(true)){
                    if(ana.emptyFlag){
                        removes.add(jar);
                        continue;
                    }
                    MavenUtil.i().getLog().info("Local reachability index for"+jar.getSig()+" does not exist, unable to perform reachability analysis.");
                    continue;
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

    void readVulApis(String path,DepJar jar){
        Set<String> apis = new HashSet<>();
        vulJarApis.put(jar,apis);
        try (BufferedReader reader = new BufferedReader(new FileReader(path))) {
            String line;
            while ((line = reader.readLine()) != null) {
                vulJarApis.get(jar).add(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}