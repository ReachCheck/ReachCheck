package dependence.vo;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class ClassDup {
    private String clsSig;
    private List<DepJar> depJars;

    public List<DepJar> getDepJars() {
        return depJars;
    }

    public ClassDup(String clsSig) {
        this.clsSig = clsSig;
        depJars = new ArrayList<>();
    }

    public boolean isSelf(String otherSig) {
        return clsSig.equals(otherSig);
    }

    public String getClsSig() {
        return clsSig;
    }

    public String getRiskString() {
        StringBuilder sb = new StringBuilder("risk for class:" + clsSig);
        for (String mthd : getAllMthd()) {
            String mthdStr = getMthdStr(mthd);
            if (null != mthdStr)
                sb.append("\n m:").append(mthdStr).append(" ");
        }
        return sb.toString();
    }

    /**
     * @return union of class method (class in jar1,class in jar2)
     */
    private Set<String> getAllMthd() {
        Set<String> allMthds = new HashSet<>();
        for (DepJar depJar : depJars) {
            ClassVO clsVO = depJar.getClassVO(clsSig);
            if (clsVO != null) {
                for (MethodVO mthd : clsVO.getMthds()) {
                    allMthds.add(mthd.getMthdSig());
                }
            }
        }
        return allMthds;
    }

    private String getMthdStr(String mthdSig) {
        List<DepJar> yesJars = new ArrayList<>();
        List<DepJar> noJars = new ArrayList<>();
        for (DepJar depJar : depJars) {
            ClassVO clsVO = depJar.getClassVO(clsSig);
            if (clsVO != null) {
                if (clsVO.hasMethod(mthdSig))
                    yesJars.add(depJar);
                else
                    noJars.add(depJar);
            }
        }
        if (noJars.size() == 0)// both have
            return null;
        else {
            StringBuilder sb = new StringBuilder("risk for method" + mthdSig);
            sb.append("\nhave jar:");
            for (DepJar yesJar : yesJars) {
                sb.append("\n").append(yesJar.getValidDepPath());
            }
            sb.append("\nhaven't jar:");
            for (DepJar noJar : noJars) {
                sb.append("\n").append(noJar.getValidDepPath());
            }
            return sb.toString();
        }

    }

    public void addDepJar(DepJar depJar) {
        depJars.add(depJar);
    }

    public boolean isDup() {
        return depJars.size() > 1;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(clsSig + " exist in :");
        for (DepJar depJar : depJars) {
            sb.append(" (").append(depJar.toString()).append(")");
        }
        return sb.toString();
    }

    /**
     * 把list中test范围的jar包删除，如果全部删除则返回true
     *
     * @return
     */
    public boolean isAllTestScope() {
        depJars.removeIf(depJar -> depJar.getScope().equals("test"));
        if (depJars.size() <= 1) {
            return true;
        }
        return false;
    }
}
