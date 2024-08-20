package dependence.util;

import dependence.vo.NodeAdapter;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Conf {

    public static int DOG_DEP_FOR_DIS;//final path may be larger than PATH_DEP when child book is existed.
    public static int DOG_DEP_FOR_PATH;//final path may be larger than PATH_DEP when child book is existed.
    public static String callConflict;
    public static String callConflicts;
    public static boolean findAllpath;

    public static String transmitJar;

    public static boolean ONLY_GET_SIMPLE = false;

    public static boolean DEL_LONGTIME = true;

    public static boolean DEL_OPTIONAL = false;

    //Host path
    public static List<String> compileSourceRoots;

    //检测是不是存在类丢失的情况
    public static boolean CLASS_MISSING = false;

    //有目标的检测jar包
    public static String targetJar = null;

    public static String localMvnRepoPath = null;

    public static Map<String, Map<String, Integer>> purEditionMap = new HashMap<>();

    public static String repoPath = null;

    //dependency node record used String
    public static List<List<String>> list = new ArrayList<>();

    //dependency node tree level
    public static int depNodeTreeLevel = 0;

    // to record exclude
    public static Map<String, List<NodeAdapter>> dependencyMap = new HashMap<>();//key是依赖名，value是依赖树中所有把key排除的依赖节点。

    //处理结果输出路径
    private static String outDir = ".";

    public static String getOutDir() {
        return outDir;
    }

    public static void setOutDir(String outDir) {
        Conf.outDir = outDir;
        File file = new File(outDir);
        if (!file.exists()) {
            file.mkdirs();
        }
    }
}