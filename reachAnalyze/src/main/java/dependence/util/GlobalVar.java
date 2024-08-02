package dependence.util;

import java.util.List;
import java.util.Map;

public class GlobalVar {
	public static long runTime = 0;
	public static long time2cg = 0;
	public static long time2runDog = 0;
	public static long branchTime = 0;
	public static long time2calRef = 0;
	public static long time2filterRiskMthd = 0;
	public static boolean useAllJar ;
	public static boolean useAllJarTwoThree;
	public static boolean filterSuper;
	public static String filterListPath;
	public static List<String> filterMthds;
	public static Map<String, String> riskMethodMap;
	public static long firstConflictTime = 0;
	public static boolean useTreeSet = false;
}
