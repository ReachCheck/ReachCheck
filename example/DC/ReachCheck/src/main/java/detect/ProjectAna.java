package detect;

import dependence.vo.*;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.tree.*;
import dependence.edge.MethodCall;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Modifier;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.io.*;

public class ProjectAna {
    String jarPath;
    Map<String, CGClassVO> classVOMap = new HashMap<>();
    List<InputStream> classesStream = new ArrayList<>();
    Set<String> defMtds = new HashSet<>();
    static Set<String> jdkClass = new HashSet<>();
    Set<String> calledMtds = new HashSet<>();

    public ProjectAna(String path){
        this.jarPath = path;
        readJdkClass();
    }

    public Set<String> getLibMtds() throws IOException {
        analyzeCG();
        calledMtds.removeAll(defMtds);
        return calledMtds;
    }

    List<InputStream> getAllClassesStream() throws IOException {
        ArrayList<InputStream> classesStream = new ArrayList<>();
        if (jarPath.endsWith("/classes") || new File(jarPath).isDirectory()) {
            classesStream.addAll(findClassFiles(new File(jarPath)));
        } else {
            JarFile jarFile = new JarFile(jarPath);
            Enumeration<JarEntry> jarEntryEnumeration = jarFile.entries();
            while (jarEntryEnumeration.hasMoreElements()) {
                JarEntry jarEntry = jarEntryEnumeration.nextElement();
                if (jarEntry.getName().endsWith(".class")) {
                    classesStream.add(jarFile.getInputStream(jarEntry));
                }
            }
        }
        return classesStream;
    }

    private static List<InputStream> findClassFiles(File file) {
        ArrayList<InputStream> classesStream = new ArrayList<InputStream>();
        try {
            File[] files = file.listFiles();
            for (File subFile : files != null ? files : new File[0]) {
                if (subFile.isDirectory()) classesStream.addAll(findClassFiles(subFile));
                else {
                    if (subFile.getName().endsWith(".class")) {
                        classesStream.add(new FileInputStream(subFile));
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return classesStream;
    }

    void getAllClassVOFromPath() throws IOException {
        List<InputStream> classesStream = getAllClassesStream();
        for (InputStream classInputStream : classesStream) {
            ClassReader classReader = new ClassReader(classInputStream);
            ClassNode classNode = new ClassNode();
            classReader.accept(classNode, 0);
            String className = classNode.name.replaceAll("/", ".");
            CGClassVO cgClassVO = classVOMap.computeIfAbsent(className, k -> new CGClassVO());
            cgClassVO.setAccess(classNode.access);
            cgClassVO.setClassName(classNode.name);
            cgClassVO.setSuperName(classNode.superName);
            for (String interfaceName : classNode.interfaces) {
                cgClassVO.addInterface(interfaceName);
            }
            List<CGMethodVO> cgMethodVOList = new ArrayList<>();
            for (MethodNode methodNode : classNode.methods) {
                CGMethodVO cgMethodVO = new CGMethodVO(methodNode.access, classNode.name, methodNode.name, methodNode.desc);
                cgMethodVOList.add(cgMethodVO);
            }
            cgClassVO.setMethods(cgMethodVOList);
            if (classNode.superName != null) {
                addSuperCGClassVO(cgClassVO, classNode.superName.replaceAll("/", "."));
            }
            if (!classNode.interfaces.isEmpty()) {
                addInterfaceCGClassVO(cgClassVO, cgClassVO.getInterfaces());
            }
        }
    }

    void analyzeCG() throws IOException {
        classesStream = getAllClassesStream();
        getAllClassVOFromPath();
        for (InputStream classInputStream : classesStream) {
            ClassReader classReader = new ClassReader(classInputStream);
            ClassNode classNode = new ClassNode();
            classReader.accept(classNode, 0);
            for (MethodNode methodNode : classNode.methods) {
                CGMethodVO callerCGMethodVO = new CGMethodVO(methodNode.access, classNode.name, methodNode.name, methodNode.desc);
                defMtds.add(callerCGMethodVO.getSig());
                ListIterator<AbstractInsnNode> instructions = methodNode.instructions.iterator();
                int lineNumber = -1;
                while (instructions.hasNext()) {
                    AbstractInsnNode node = instructions.next();
                    if (node instanceof MethodInsnNode) {
                        CGMethodVO calledCGMethodVO = new CGMethodVO(node.getOpcode(), ((MethodInsnNode) node).owner,
                                ((MethodInsnNode) node).name, ((MethodInsnNode) node).desc);
                        if(jdkClass.contains(calledCGMethodVO.getClassName())){
                            continue;
                        }
                        MethodCall methodCall = new MethodCall(callerCGMethodVO, calledCGMethodVO, lineNumber);
                        calledMtds.add(methodCall.getCalledMethodSig());
                        addDynamicMethodCall(methodCall);
                    }
                    if (node instanceof LineNumberNode) {
                        lineNumber = ((LineNumberNode) node).line;
                    }
                }
            }
        }
    }

    void addSuperCGClassVO(CGClassVO cgClassVO, String superClassName) {
        CGClassVO superCGClassVO = classVOMap.computeIfAbsent(superClassName, k -> new CGClassVO());
        superCGClassVO.setClassName(superClassName);
        superCGClassVO.addSubCGClassVO(cgClassVO);
        cgClassVO.setSuperCGClassVO(superCGClassVO);
    }

    void addInterfaceCGClassVO(CGClassVO dcgClassVO, List<String> interfaces) { //接口名称为什么不替换/为.
        for (String interfaceName : interfaces) {
            CGClassVO interFaceCGClassVO = classVOMap.computeIfAbsent(interfaceName, k -> new CGClassVO());
            interFaceCGClassVO.setClassName(interfaceName);
            interFaceCGClassVO.addSubCGClassVO(dcgClassVO);
        }
    }

    private void addDynamicMethodCall(MethodCall methodCall) {
        if (Modifier.isPublic(methodCall.getCalledMethod().getAccess())
                || Modifier.isAbstract(methodCall.getCalledMethod().getAccess())
                || Modifier.isProtected(methodCall.getCalledMethod().getAccess())
                || Modifier.isInterface(methodCall.getCalledMethod().getAccess())) {
            String calledCGMethodVOClassName = methodCall.getCalledMethod().getClassName();
            if(jdkClass.contains(calledCGMethodVOClassName)){
                return;
            }
            if (classVOMap.containsKey(calledCGMethodVOClassName)) {
                HashSet<CGClassVO> set = classVOMap.get(calledCGMethodVOClassName).getSubCGClassVO();
                add(methodCall, set);
            }
        }
    }

    void readJdkClass(){
        if(jdkClass.size()!=0){
            return;
        }
        try {
            ClassLoader classLoader = ProjectAna.class.getClassLoader();
            InputStream inputStream = classLoader.getResourceAsStream("classlist");

            if (inputStream != null) {
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream, StandardCharsets.UTF_8);
                BufferedReader reader = new BufferedReader(inputStreamReader);

                String line;
                while ((line = reader.readLine()) != null) {
                    jdkClass.add(line);
                }
                reader.close();
            } else {
                System.out.println("资源文件不存在或无法读取。");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void add(MethodCall methodCall, Set<CGClassVO> cgClassVOS) {
        for (CGClassVO subCGClassVO : cgClassVOS) {
            if (subCGClassVO.getSubCGClassVO().size() > 0) {
                add(methodCall, subCGClassVO.getSubCGClassVO());
            }
            CGMethodVO calledMethod = methodCall.getCalledMethod();
            CGMethodVO dynamicCGMethodVO = new CGMethodVO(calledMethod.getAccess(), subCGClassVO.getClassName(),
                    calledMethod.getMethodName(), calledMethod.getDesc());
            if (dynamicCGMethodVO.getSig().equals(methodCall.getCallerMethod().getSig())) continue; //这里是为什么
            MethodCall dynamicMethodCall = new MethodCall(methodCall.getCallerMethod(), dynamicCGMethodVO,
                    methodCall.getLineNumber());
            calledMtds.add(dynamicMethodCall.getCalledMethodSig());
        }
    }
}
