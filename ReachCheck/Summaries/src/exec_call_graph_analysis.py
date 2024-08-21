import hashlib
import multiprocessing
import os
import subprocess
import time


def calculate_sha1(file_path):
    sha1 = hashlib.sha1()
    with open(file_path, 'rb') as file:
        for chunk in iter(lambda: file.read(4096), b""):
            sha1.update(chunk)
    return sha1.hexdigest()


def save_sha1(file_path):
    sha1_value = calculate_sha1(file_path)
    sha1_file_path = file_path + ".sha1"
    with open(sha1_file_path, 'w') as sha1_file:
        sha1_file.write(sha1_value)


def compute_hash(output_file):
    public_methods_file = os.path.join(output_file, "public_methods.txt")
    if os.path.exists(public_methods_file):
        save_sha1(public_methods_file)


record_file = "processed_asm_cg_maven.txt"


def run_jar_command(jar_path, analysis_path):
    output_file = analysis_path[0]
    input_file = analysis_path[1]
    print("output_file path :" + output_file)
    try:
        command = ["java", "-Xmx256G", "-jar", jar_path, "-jarFile", input_file, "-output", output_file]
        print(" ".join(command))
        start = time.time()
        process = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True,
                                 check=True)
        compute_hash(output_file)
        write_to_record_file(input_file)
        print(output_file + " has been completed@@" + str(time.time() - start))
        return input_file, "Success", process.stdout
    except subprocess.CalledProcessError as e:
        return input_file, f"Error: {e}", ""


def handle_result(result):
    input_file, status, stdout = result
    print(f"Process for {input_file} completed with status: {status}")
    # if stdout:
    #     print(f"Standard Output:\n{stdout}")


def get_all_package_path(data_path):
    all_jar_path = []
    for root, dirs, files in os.walk(data_path):
        for dir_path in dirs:
            jar_file = find_jar_file_in_path(os.path.join(root, dir_path))
            if jar_file and jar_file not in all_records:
                print(jar_file + " is not exist, need process")
                all_jar_path.append((os.path.join(root, dir_path), jar_file))
    return all_jar_path


def find_jar_file_in_path(dir_path):
    for file in os.listdir(dir_path):
        if file.endswith(".jar"):
            return os.path.join(dir_path, file)
    return None


def read_jar_path_file(file_path):
    jar_paths = []
    with open(file_path, "r") as f:
        for line in f.readlines():
            jar_paths.append(line.strip().split("@@"))
    return jar_paths


def single_main():
    analysis_paths = read_jar_path_file(jar_paths_file)
    for analysis_path in analysis_paths[::-1]:
        if os.path.exists(os.path.join(analysis_path[0], "public_methods.txt.sha1")):
            continue
        input_file, status, stdout = run_jar_command(asmCallGraph_jar_file, analysis_path)
        print(f"input_file:{input_file},status:{status}, stdout:{stdout}")


def multi_main():
    analysis_paths = read_jar_path_file(jar_paths_file)
    num_processes = int(multiprocessing.cpu_count() / 2)
    pool = multiprocessing.Pool(processes=num_processes)

    results = []

    for analysis_path in analysis_paths:
        if os.path.exists(os.path.join(analysis_path[0], "public_methods.txt.sha1")):
            continue
        result = pool.apply_async(run_jar_command, args=(asmCallGraph_jar_file, analysis_path),
                                  callback=handle_result)
        results.append(result)

    pool.close()
    pool.join()

    for result in results:
        result.wait()

    print("All processes completed.")


def write_to_record_file(content):
    with open(record_file, "a") as f:
        f.write(content + "\n")


def read_record_file(file_name):
    try:
        with open(file_name, "r") as f:
            processed_projects = set(line.strip() for line in f)
    except FileNotFoundError:
        open(file_name, "w").close()
        processed_projects = set()

    return processed_projects


if __name__ == '__main__':
    current_folder = os.path.dirname(os.path.abspath(__file__)) + '/'
    asmCallGraph_jar_file = os.path.join(current_folder, '../', 'asmCallGraph.jar')
    maven_repo = "~/.m2/repository/"
    record_file = "processed_asm_cg_maven.txt"
    jar_paths_file = current_folder + "all_jar_paths.txt"
    all_records = read_record_file(record_file)
    # multi_main()
    single_main()
