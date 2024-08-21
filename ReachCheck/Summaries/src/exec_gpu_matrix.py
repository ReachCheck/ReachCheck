from loguru import logger

from bool_matrix_cubool import save_matrix_to_txt, calculate_matrix_reachable
from log_controller import setup_logger

import os


def read_file(file_name):
    try:
        with open(file_name, "r") as file:
            all_lines = file.readlines()
    except FileNotFoundError as e:
        logger.error(f"Error: {e}")
        all_lines = set()
    return all_lines


def write_to_file(file_path, content):
    with open(file_path, "a") as f:
        f.write(content + "\n")


def read_record_file(file_name):
    try:
        with open(file_name, "r") as f:
            processed_projects = set(line.strip() for line in f)
    except FileNotFoundError:
        open(file_name, "w").close()
        processed_projects = set()

    return processed_projects


def sort_key(path):
    return int(path.split('@@')[1])


def get_lines_num(file_path):
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            return len(lines)
    except FileNotFoundError:
        print(f"Error: File not found - {file_path}")
        return None


def main():
    for jar_path in read_file(all_jar_paths_file):
        project_path = jar_path.split("@@")[0]
        try:
            if (project_path in all_processed
                    or (os.path.exists(os.path.join(project_path, "reachable_graph", "inner_graph.zst"))
                        and os.path.exists(os.path.join(project_path, "reachable_graph", "outer_graph.zst")))):
                continue
            if os.path.exists(project_path):
                logger.info("start to handle " + project_path)
                edge_file_path = os.path.join(project_path, "edges.txt")
                public_methods_file_path = os.path.join(project_path, "public_methods.txt")
                public_methods_size = get_lines_num(public_methods_file_path)
                not_public_methods_file_path = os.path.join(project_path, "not_public_methods.txt")
                not_public_methods_size = get_lines_num(not_public_methods_file_path)
                def_methods_size = public_methods_size + not_public_methods_size
                if def_methods_size > 0:
                    matrix, tpl_dict, gpu_time = calculate_matrix_reachable(edge_file_path, def_methods_size)
                    if matrix:
                        save_matrix_to_txt(matrix, tpl_dict, project_path, public_methods_size)
                        write_to_file(processed_gpu_cubool, project_path)
                        write_to_file(result_gpu_cubool, project_path + "@@" + str(gpu_time))
        except Exception as e:
            logger.error(str(e))
            write_to_file(processed_gpu_cubool, project_path)
            write_to_file(result_gpu_cubool, project_path + "@@" + "error")


if __name__ == '__main__':
    setup_logger(os.path.splitext(os.path.basename(__file__))[0])
    current_folder = os.path.dirname(os.path.abspath(__file__)) + '/'
    all_jar_paths_file = os.path.join(current_folder, "all_jar_paths_with_size.txt")
    processed_gpu_cubool = os.path.join(current_folder, "cubool_processed_gpu.txt")
    result_gpu_cubool = os.path.join(current_folder, "cubool_result_gpu.txt")
    all_processed = read_record_file(processed_gpu_cubool)
    main()
    logger.info("All processed！")
