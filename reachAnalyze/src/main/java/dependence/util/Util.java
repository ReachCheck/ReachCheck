package dependence.util;

import callGraph.vo.SourceClassManager;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 *
 */
public class Util {

    /**
     * @param mthdSig e.g.:conflict.vo.DepJar: getAllDepPath()Ljava/lang/String;
     * @return e.g.: getAllDepPath()Ljava/lang/String;
     */
    public static String mthdSig2name(String mthdSig) {
        return mthdSig.substring(mthdSig.indexOf(":") + 1);
    }

    /**
     * @param mthdSig e.g.:conflict.vo.DepJar: getAllDepPath()Ljava/lang/String;
     * @return e.g.:conflict.vo.DepJar
     */
    public static String mthdSig2cls(String mthdSig) {
        return mthdSig.substring(0, mthdSig.indexOf(":"));
    }


    public static Set<String> getJarClasses(String path) {
        if (new File(path).exists()) {
            if (!path.endsWith("tar.gz") && !path.endsWith(".pom") && !path.endsWith(".war")) {
                //temp
                List<String> paths = new ArrayList<>();
                paths.add(path);
                //
                try {
                    return SourceClassManager.getAllClassesFromPaths(paths);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                MavenUtil.i().getLog().warn(path + "is illegal classpath");
            }
        } else {
            MavenUtil.i().getLog().warn(path + "doesn't exist in local");
        }
        return new HashSet<>();
    }
}
