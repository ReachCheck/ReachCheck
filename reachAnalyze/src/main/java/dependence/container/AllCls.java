package dependence.container;

import dependence.vo.DepJar;

import java.util.HashSet;
import java.util.Set;

/**
 *
 * FinalClasses is set of ClassVO,but AllCls is set of class signature.
 * FinalClasses是ClassVO的集合，但AllCls是类签名的集合。
 */
public class AllCls {
    private static AllCls instance;
    private final Set<String> classSet;

    public static void init(DepJars depJars) {
        if (instance == null) {
            instance = new AllCls(depJars);
        }
    }

    /**
     * 初始化的时候用其他depJar
     *
     * @param depJars
     * @param depJar
     */
    public static void init(DepJars depJars, DepJar depJar) {
        instance = new AllCls(depJars, depJar);
    }

    public static AllCls i() {
        return instance;
    }

    //构造函数
    private AllCls(DepJars depJars) {
        classSet = new HashSet<>();
        for (DepJar depJar : depJars.getAllDepJar()) {
            if (depJar.isSelected()) {     //所有被选择没有被屏蔽的jar包
                //得到depJar中所有的类
                classSet.addAll(depJar.getAllCls(false));
            }
            depJar.getClsTb();//获取cls签名对应的clsV0
        }
    }

    /**
     * 重构方法，使初始化方法有默认参数
     */
    private AllCls(DepJars depJars, DepJar usedDepJar) {
        classSet = new HashSet<>();
        for (DepJar depJar : depJars.getAllDepJar()) {
            if (depJar.isSelected()) {
                //得到depJar中所有的类
                if (depJar.isSameLib(usedDepJar)) {
                    classSet.addAll(usedDepJar.getAllCls(true));
                } else {
                    classSet.addAll(depJar.getAllCls(true));
                }
            }
        }
    }

    public Set<String> getAllCls() {
        return classSet;
    }

    //clses是否包含sls
    public boolean contains(String cls) {
        return classSet.contains(cls);
    }
}
