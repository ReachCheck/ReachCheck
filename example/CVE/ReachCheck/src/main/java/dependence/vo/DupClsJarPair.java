package dependence.vo;

import dependence.container.Conflicts;
import dependence.util.Conf;
import dependence.util.GlobalVar;
import dependence.util.MavenUtil;
import dependence.util.Util;

import java.util.*;

/**
 * two jar that have different name and same class.
 */
public class DupClsJarPair {
    private DepJar jar1;
    private DepJar jar2;
    private Set<String> clsSigs;

    public DupClsJarPair(DepJar jarA, DepJar jarB) {
        jar1 = jarA;
        jar2 = jarB;
        clsSigs = new HashSet<>();
    }

    public String getSig() {
        return jar1.toString() + "-" + jar2.toString();
    }

    public Set<String> getClsSigs() {
        return clsSigs;
    }

    public boolean isInDupCls(String rhcedMthd) {
        return clsSigs.contains(Util.mthdSig2cls(rhcedMthd));
    }

    public void addClass(String clsSig) {
        clsSigs.add(clsSig);
    }

    /**
     * judge jarA whether is the same jar with jarB
     *
     * @param jarA DepJar
     * @param jarB DepJar
     * @return boolean
     */
    public boolean isSelf(DepJar jarA, DepJar jarB) {
        return (jar1.equals(jarA) && jar2.equals(jarB)) || (jar1.equals(jarB) && jar2.equals(jarA));
    }

    public DepJar getJar1() {
        return jar1;
    }

    public DepJar getJar2() {
        return jar2;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("    jar1 : ").append(getJar1().getSig()).append(" ").append(getJar1().getAllDepPath()).append("\n");
        sb.append("    jar2 : ").append(getJar2().getSig()).append(" ").append(getJar2().getAllDepPath()).append("\n");
        sb.append("classes : \n");
        for (String clsSig : getClsSigs()) {
            sb.append("    ").append(clsSig).append("\n");
        }
        return sb.toString();
    }

    /**
     * get jar1 and jar2 risk method
     *
     * @return string
     */
    public String getRiskString() {
        return "classConflict:" + "<" + jar1.toString() + ">" +
                "<" + jar2.toString() + ">\n" +
                getJarString(jar1, jar2) +
                getJarString(jar2, jar1);
    }

    /**
     * get total depJar only methods
     *
     * @param total
     * @param some
     * @return string
     */
    private String getJarString(DepJar total, DepJar some) {
        StringBuilder sb = new StringBuilder();
        Set<String> onlyMthds = getOnlyMethod(total, some);
        sb.append("   methods that only exist in ").append(total.getValidDepPath()).append("\n");
        if (onlyMthds.size() > 0) {
            for (String onlyMthd : onlyMthds) {
                sb.append(onlyMthd).append("\n");
            }
        }
        return sb.toString();
    }

    /**
     * get total depJar only methods
     *
     * @param total
     * @param some
     * @return Set<String> total depJar onlyMthds
     */
    private Set<String> getOnlyMethod(DepJar total, DepJar some) {
        Set<String> onlyMthds = new HashSet<>();
        for (String clsSig : clsSigs) {
            ClassVO classVO = total.getClassVO(clsSig);
            if (classVO != null) {
                for (MethodVO mthd : classVO.getMthds()) {
                    if (!some.getClassVO(clsSig).hasMethod(mthd.getMthdSig()))
                        onlyMthds.add(mthd.getMthdSig());
                }
            }
        }
        return onlyMthds;
    }


    /**
     * get jar classPaths to get call graph
     *
     * @return List<String> classpaths
     * @throws Exception
     */
    public Collection<String> getPrcDirPaths() throws Exception {
        List<String> classpaths;
        if (!Conf.CLASS_MISSING) {
            if (jar1.getPriority() < jar2.getPriority() && jar1.getPriority() != -1) {
                if (GlobalVar.useAllJarTwoThree) {
                    classpaths = jar2.getRepalceCp();
                } else {
                    classpaths = getJarClassPathPruning(this.jar2);
                }
            } else {
                if (GlobalVar.useAllJarTwoThree) {
                    classpaths = jar1.getRepalceCp();
                } else {
                    classpaths = getJarClassPathPruning(this.jar1);
                }
            }
        } else {
            classpaths = getClassMissingPaths();
        }
        return classpaths;
    }

    public List<String> getClassMissingPaths() {
        List<String> classpath = new ArrayList<>();
        Set<String> paths = new HashSet<>();
        if (jar1.getPriority() < jar2.getPriority() && jar1.getPriority() != -1) {
            paths.addAll(getJarClassPathPruning(jar1));
            Conflict conflict = null;
            for (Conflict conflict1 : Conflicts.i().getConflicts()) {
                if (conflict1.getSig().equals(jar2.getName())) {
                    conflict = conflict1;
                    break;
                }
            }
            if (conflict != null) {
                for (DepJar depJar : conflict.getDepJars()) {
                    paths.addAll(getJarClassPathPruning(depJar));
                    paths.removeAll(depJar.getJarFilePaths(true));
                }
            }
        } else {
            paths.addAll(getJarClassPathPruning(jar2));
            Conflict conflict = null;
            for (Conflict conflict1 : Conflicts.i().getConflicts()) {
                if (conflict1.getSig().equals(jar1.getName())) {
                    conflict = conflict1;
                    break;
                }
            }
            if (conflict != null) {
                for (DepJar depJar : conflict.getDepJars()) {
                    paths.addAll(getJarClassPathPruning(depJar));
                    paths.removeAll(depJar.getJarFilePaths(true));
                }
            }
        }
        classpath.addAll(paths);
        return classpath;
    }


    private List<String> getJarClassPathPruning(DepJar depJar) {
        List<String> classpaths = new ArrayList<>();
        MavenUtil.i().getLog().info("not add all jar to process");
        try {
            classpaths.addAll(depJar.getJarFilePaths(true));
            classpaths.addAll(depJar.getOnlyFatherJarCps(true));
        } catch (NullPointerException e) {
            classpaths = new ArrayList<>();
        }
        return classpaths;
    }
}
