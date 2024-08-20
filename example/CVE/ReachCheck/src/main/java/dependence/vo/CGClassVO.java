package dependence.vo;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

public class CGClassVO {
    private int access;
    private String superName;
    private List<String> interfaces = new ArrayList<>();
    private String className;
    private List<CGMethodVO> methods = new ArrayList<>();
    private CGClassVO superCGClassVO;//?
    private HashSet<CGClassVO> subCGClassVO = new HashSet<>();
    public CGClassVO() {
    }

    public CGClassVO(int access, String superName, String className) {
        this.access = access;
        this.superName = superName.replaceAll("/", ".");
        this.className = className.replaceAll("/", ".");
    }

    public CGClassVO(int access, String superName, String className, List<CGMethodVO> methods) {
        this.access = access;
        this.superName = superName.replaceAll("/", ".");
        this.className = className.replaceAll("/", ".");
        this.methods = methods;
    }

    public List<String> getInterfaces() {
        return interfaces;
    }

    public void addInterface(String interfaceName) {
        interfaces.add(interfaceName.replaceAll("/", "."));
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className.replaceAll("/", ".");
    }

    public int getAccess() {
        return access;
    }

    public void setAccess(int access) {
        this.access = access;
    }

    public String getSuperName() {
        return superName;
    }

    public void setSuperName(String superName) {
        if (superName != null) {
            this.superName = superName.replaceAll("/", ".");
        }
    }

    public List<CGMethodVO> getMethods() {
        return methods;
    }

    public void setMethods(List<CGMethodVO> methods) {
        this.methods = methods;
    }

    public void addSubCGClassVO(CGClassVO CGClassVO) {
        subCGClassVO.add(CGClassVO);
    }

    public HashSet<CGClassVO> getSubCGClassVO() {
        return subCGClassVO;
    }

    public void setSuperCGClassVO(CGClassVO CGClassVO) {
        superCGClassVO = CGClassVO;
    }

    public CGClassVO getSuperCGClassVO() {
        return superCGClassVO;
    }

    @Override
    public String toString() {
        return "CGClassVO{" +
                "access=" + access +
                ", superName='" + superName + '\'' +
                ", className='" + className + '\'' +
                ", methods=" + methods +
                '}';
    }
}
