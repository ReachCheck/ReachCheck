package detect;

import com.github.luben.zstd.ZstdInputStream;
import java.io.*;
import java.util.*;

public class Analyze2 {
    Map<String,Integer> defMap;
    Map<String,Integer> libMap;
    List<String> apis;
    int [][] innerMatrix = null;
    int [][] outerMatrix = null;
    ArrayList<Integer> entrance;
    Set<String> outputApi;
    public String jarPath;
    public String G_A_V;
    public boolean emptyFlag;
    int id = 0;
    int tplStartId;
    int publicMethodLength;

    public Analyze2(String groupId,String artifactId,String version,String jarPath){
        this.G_A_V = groupId+":"+artifactId+":"+version;
        this.jarPath = jarPath;
    }

    public boolean reachInfoExist(boolean isOuter){
        if(isEmpty()){
            return false;
        }
        String tplPath = jarPath+"/tpls_methods.txt";
        String innerMatrixPath = jarPath+"/reachable_graph/inner_graph.zst";
        String outerMatrixPath = jarPath+"/reachable_graph/outer_graph.zst";
        File f1 = new File(tplPath);
        File f2 = new File(innerMatrixPath);
        File f3 = new File(outerMatrixPath);
        System.out.println(jarPath);
        if (f1.exists() && defMap == null) {
            readDefAndLib(jarPath);
        }
        if (f1.exists()) {
            if (isOuter && outerMatrix == null) {
                return readMatrixDataIfExists(f3, outerMatrixPath, isOuter);
            }
            else if(!isOuter && innerMatrix == null) {
                return readMatrixDataIfExists(f2, innerMatrixPath, isOuter);
            }
            else{
                return true;
            }
        } else {
            return false;
        }
    }

    boolean isEmpty(){
        File file = new File(jarPath+"/public_methods.txt");
        if (file.length() == 0) {
            emptyFlag = true;
        }
        return emptyFlag;
    }

    boolean readMatrixDataIfExists(File file, String matrixPath, boolean isOuter) {
        if (file.exists()) {
            readMatrixData(matrixPath, isOuter);
            return true;
        } else {
            return false;
        }
    }

    public void getOutput(Set<String> inputApi){
        entrance = new ArrayList<>();
        Iterator<String> iterator = inputApi.iterator();
        while(iterator.hasNext()){
            String api = iterator.next();
            if(defMap.containsKey(api)){
                entrance.add(defMap.get(api));
                iterator.remove();
            }
        }
        if(entrance.size() == 0){
            return;
        }
        outputApi = new HashSet<>();
        multiplyMatrices2Reduced(entrance,outerMatrix);
        inputApi.addAll(outputApi);
    }

    public void multiplyMatrices1Reduced(ArrayList<Integer> A,int [][]B){
        for(int i=0;i<defMap.size();i++){
            for (int x : A) {
                if (B[x][i] == 1) {
                    outputApi.add(apis.get(i));
                    break;
                }
            }
        }
    }

    public void multiplyMatrices2Reduced(ArrayList<Integer> A,int [][]B){
        for(int i=0;i<libMap.size();i++){
            for(int x:A){
                if(B[x][i] == 1){
                    outputApi.add(apis.get(i+tplStartId));
                    break;
                }
            }
        }
    }

    public void readMatrixData(String path,boolean isOuter){ //
        if(isOuter){
            outerMatrix = new int[publicMethodLength][libMap.size()];
            readZstdFile(outerMatrix,path,tplStartId);
        }
        else{
            innerMatrix = new int[publicMethodLength][defMap.size()];
            readZstdFile(innerMatrix,path,0);
        }
    }

    void readZstdFile(int [][] matrix,String path,int startId){
        try (FileInputStream fileInputStream = new FileInputStream(path)) {
            ZstdInputStream zstdInputStream = new ZstdInputStream(fileInputStream);
            InputStreamReader inputStreamReader = new InputStreamReader(zstdInputStream, "UTF-8");
            BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                String[] edges = line.split(",");
                int srcId = Integer.parseInt(edges[0]);
                for (int i = 1; i < edges.length; i++) {
                    int tarId = Integer.parseInt(edges[i])-startId;
                    matrix[srcId][tarId] = 1;
                }
            }
            bufferedReader.close();
            inputStreamReader.close();
            zstdInputStream.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    void readData(Map<String,Integer> map,String filePath){
        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            while ((line = reader.readLine()) != null) {
                map.put(line,id++);
                apis.add(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void readDefAndLib(String path){
        apis = new ArrayList<>();
        defMap = new TreeMap<>();
        libMap = new TreeMap<>();
        String notPublicFilePath = path + "/not_public_methods.txt";
        String publicFilePath = path +"/public_methods.txt";
        String tplFilePath = path + "/tpls_methods.txt";
        readData(defMap,publicFilePath);
        publicMethodLength = id;
        readData(defMap,notPublicFilePath);
        tplStartId = id;
        readData(libMap,tplFilePath);
    }

    public Set<String> query(Set<String>src,Set<String> target){
        outputApi = new HashSet<>();
        entrance = new ArrayList<>();
        for(String api: src){
            if(defMap.containsKey(api)){
                entrance.add(defMap.get(api));
                outputApi.add(api);
            }
        }
        if(entrance.size() == 0){
            return null;
        }
        multiplyMatrices1Reduced(entrance,innerMatrix);
        target.retainAll(outputApi);
        return target;
    }
}
