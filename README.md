# ReachCheck Overview

## Files and directories Description

**ReachCheck** directory contains Reachcheck's project code, including how to get summaries and reachable analysis.

**example** directory contains the examples for downstream tasks.

**graph_index_tools** directory contains comparison tool code for experimental evaluation. Before using the comparison tool code in the **graph_index_tools** directory, please replace the `exp_path` variable in the code with the actual path to the **dataset** directory and compile the code.

**reportData** directory contains the usefulness reports.

**dataset.tar.zst** contains the dataset used for experimental testing. Inside the compressed package, there is a file named  **query_project.txt** , which lists the experimental test projects.

**conflict_detect.tar** and **vul_detect.tar** contain test cases for dependency conflict detection and vulnerability detection, respectively. The `repository` directory inside the compressed package contains third-party library JAR files with generated call edges. To conduct the tests, please copy the call edge analysis files and JAR files from the `repository` directory to your local Maven repository.

## Downstream Task Evaluation

Dependency Conflict Detection:

`mvn -f=<pomPath> -DtargetJar=<classesDirPath> -Dmaven.test.skip=true org.example:ReachCheck_plugin:1.0:detectConflict -e`

`example: mvn -f=/conflict_detect/test_project/pom.xml -DtargetJar=/conflict_detect/test_project/target/classes -Dmaven.test.skip=true org.example:ReachCheck_plugin:1.0:detectConflicte -e`

Vulnerability Reachability Analysis:

`mvn -f=<pomPath> -DtargetJar=<classesDirPath> -Dmaven.test.skip=true org.example:ReachCheck_plugin:1.0:detectVul -e`

## Executable Examples for Downstream Tasks

We provide executable examples for the two downstream tasks: Dependency Conflict Detection and Vulnerability Reachability Analysis. The examples are located in the `example` folder.
