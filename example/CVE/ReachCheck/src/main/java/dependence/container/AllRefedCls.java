package dependence.container;

import dependence.util.GlobalVar;
import dependence.util.MavenUtil;
import dependence.vo.DepJar;
import javassist.ClassPool;

import java.util.HashSet;
import java.util.Set;

/**
 * 所有被引用的cls
 */
public class AllRefedCls {
    private static AllRefedCls instance;
    private final Set<String> reefedClasses;

    private AllRefedCls() {
        long start = System.currentTimeMillis();
        reefedClasses = new HashSet<>();
        try {
            ClassPool pool = new ClassPool();
            for (String path : DepJars.i().getUsedJarPaths()) {
                pool.appendClassPath(path);
            }
            for (String cls : AllCls.i().getAllCls()) {
                reefedClasses.add(cls);
                if (pool.getOrNull(cls) != null) {
                    //
                    reefedClasses.addAll(pool.get(cls).getRefClasses());
                } else {
                    MavenUtil.i().getLog().warn("can't find " + cls + " in pool when form reference.");
                }
            }
        } catch (Exception e) {
            MavenUtil.i().getLog().error("get refedCls error:", e);
        }
        long runtime = (System.currentTimeMillis() - start) / 1000;
        GlobalVar.time2calRef += runtime;
    }

    private AllRefedCls(DepJar depJar) {
        long start = System.currentTimeMillis();
        reefedClasses = new HashSet<>();
        try {
            ClassPool pool = new ClassPool();
            for (String path : DepJars.i().getUsedJarPaths(depJar)) {
                pool.appendClassPath(path);
            }
            for (String cls : AllCls.i().getAllCls()) {
                reefedClasses.add(cls);
                if (pool.getOrNull(cls) != null) {
                    reefedClasses.addAll(pool.get(cls).getRefClasses());
                } else {
                    MavenUtil.i().getLog().warn("can't find " + cls + " in pool when form reference.");
                }
            }
        } catch (Exception e) {
            MavenUtil.i().getLog().error("get refedCls error:", e);
        }
        long runtime = (System.currentTimeMillis() - start) / 1000;
        GlobalVar.time2calRef += runtime;
    }

    public static AllRefedCls i() {
        if (instance == null) {
            instance = new AllRefedCls();
        }
        return instance;
    }

    public static void init(DepJar depJar) {
        instance = new AllRefedCls(depJar);
    }

    public static AllRefedCls i(DepJar depJar) {
        instance = new AllRefedCls(depJar);
        return instance;
    }

    public boolean contains(String cls) {
        return reefedClasses.contains(cls);
    }

}
