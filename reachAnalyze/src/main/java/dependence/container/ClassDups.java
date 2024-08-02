package dependence.container;

import dependence.vo.ClassDup;
import dependence.vo.DepJar;

import java.util.ArrayList;
import java.util.List;

/**
 * class duplicate
 * duplicate : same classes in different jars
 */
public class ClassDups {
    private final List<ClassDup> container;

    public ClassDups(DepJars depJars) {
        container = new ArrayList<>();
        for (DepJar depJar : depJars.getAllDepJar()) {
            if (depJar.isSelected()) {
                for (String cls : depJar.getAllCls(false)) {
                    addCls(cls, depJar);
                }
            }
        }
        // delete conflict if there is only one version
        container.removeIf(conflict -> !conflict.isDup());
    }

    public List<ClassDup> getAllClsDup() {
        return container;
    }

    private void addCls(String classSig, DepJar depJar) {
        ClassDup clsDup = null;
        for (ClassDup existDup : container) {
            if (existDup.isSelf(classSig))
                clsDup = existDup;
        }
        if (null == clsDup) {
            clsDup = new ClassDup(classSig);
            container.add(clsDup);
        }
        clsDup.addDepJar(depJar);
    }
}
