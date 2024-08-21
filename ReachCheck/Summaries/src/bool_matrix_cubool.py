import os
import pycubool as cb
import zstandard as zstd
from loguru import logger

from log_controller import setup_logger
import time
from concurrent.futures import ProcessPoolExecutor, as_completed


# read mapping file and return edges
def read_mapping(edge_file_path):
    edges = []
    with open(edge_file_path, 'r') as f:
        for line in f:
            source, target = map(int, line.strip().split(','))
            edges.append((source, target))
    return edges


def generate_matrix(edge_file_path, def_methods_size):
    edges = read_mapping(edge_file_path)
    logger.info(f"def methods size : {str(def_methods_size)}, edges szie : {str(len(edges))}")
    bool_matrix = cb.Matrix.empty(shape=(def_methods_size, def_methods_size))
    if def_methods_size == 0:
        return None, {}
    tpl_dict = {}
    for source, target in edges:
        if source < def_methods_size and target < def_methods_size:
            bool_matrix[source, target] = True
        else:
            if source in tpl_dict:
                tpl_dict[source].add(target)
            else:
                tpl_dict[source] = {target}
    logger.debug(f"Matrix filling complete, tpl_dict size : {str(len(tpl_dict))}")
    return bool_matrix, tpl_dict


def calculate_matrix_reachable(mapping_file_path, def_methods_size):
    logger.info(f"Start matrix calculation")
    try:
        matrix, tpl_dict = generate_matrix(mapping_file_path, def_methods_size)
        if matrix:
            total = 0  # Current number of values
            i = 0
            start_time = time.time()
            while total != matrix.nvals:
                total = matrix.nvals
                matrix.mxm(matrix, out=matrix, accumulate=True)  # t += t * t
                i = i + 1
            end_time = time.time()
            logger.info(f"Matrix calculation time : {end_time - start_time}")
            return matrix, tpl_dict, end_time - start_time
        else:
            return cb.Matrix.empty(shape=(0, 0)), {}, 0
    except Exception as e:
        logger.error(str(e))
        return cb.Matrix.empty(shape=(0, 0)), {}, 0


def read_def_methods_num(project_dir):
    output_asm_path = os.path.join(project_dir, "output_asm_optimize.txt")
    if not os.path.exists(output_asm_path):
        output_asm_path = os.path.join(project_dir, "output_asm.txt")
    try:
        with open(output_asm_path, 'r') as file:
            first_line = file.readline().strip()
            number = int(first_line)
            return number

    except (IOError, ValueError) as e:
        logger.error(f"Error:{e}")
        return 0


def read_file(file_name):
    try:
        with open(file_name, "r") as f:
            lines = set(int(line.strip()) for line in f)
    except FileNotFoundError as e:
        logger.error(f"Error:{e}")
        open(file_name, "w").close()
        lines = set()

    return lines


def process_row(row, tpl_dict, current_row_value, public_methods_size):
    inner_graph_row_data = set()
    outer_graph_row_data = set()
    inner_graph = []
    outer_graph = []
    current_row = current_row_value
    for r, c in row:
        if r < public_methods_size:
            if r == current_row:
                inner_graph_row_data.add(c)
                outer_graph_row_data.update(tpl_dict.get(c, set()))
                outer_graph_row_data.update(tpl_dict.get(r, set()))
            else:
                if current_row is not None:
                    inner_graph.append((current_row, sorted(inner_graph_row_data)))
                    outer_graph.append((current_row, sorted(outer_graph_row_data)))
                current_row = r
                inner_graph_row_data = {c}
                outer_graph_row_data = set()
                outer_graph_row_data.update(tpl_dict.get(c, set()))
                outer_graph_row_data.update(tpl_dict.get(r, set()))

    inner_graph.append((current_row, sorted(inner_graph_row_data)))
    outer_graph.append((current_row, sorted(outer_graph_row_data)))
    return inner_graph, outer_graph


def save_tpl_to_txt(tpl_dict, project_path):
    logger.info(f"save graph : {project_path}")
    outer_graph_lines = []
    start = time.time()
    for key in tpl_dict:
        data = tpl_dict[key]
        if data:
            outer_graph_lines.append(f"{str(key)},{','.join(map(str, sorted(data)))}")
    outer_graph_save = "\n".join(outer_graph_lines)
    write_to_zstd_file(None, outer_graph_save, project_path)
    logger.info(f"The graph is saved, time : {time.time() - start}")


def save_matrix_to_txt(matrix, tpl_dict, project_path, public_methods_size):
    logger.info(f"save graph : {project_path}")
    max_works = 32
    start = time.time()

    rows, cols = matrix.to_lists()
    if not rows:
        return

    inner_graph = []
    outer_graph = []

    with ProcessPoolExecutor() as executor:
        futures = []

        chunk_start = 0
        chunk_size = len(rows) // max_works
        for i in range(max_works):
            chunk_end = chunk_start + chunk_size
            if chunk_end > len(rows):
                chunk_end = len(rows)
            else:
                for j in range(chunk_end, len(rows) - 1):
                    if rows[j] != rows[j + 1]:
                        chunk_end = j
                        break
                chunk_end = chunk_end + 1
            chunk_rows = rows[chunk_start:chunk_end]
            chunk_cols = cols[chunk_start:chunk_end]
            futures.append(executor.submit(process_row, zip(chunk_rows, chunk_cols), tpl_dict, chunk_rows[0],
                                           public_methods_size))

            chunk_start = chunk_end
            if chunk_start >= len(rows):
                break

        for future in as_completed(futures):
            inner_graph_data, outer_graph_data = future.result()
            inner_graph.extend(inner_graph_data)
            outer_graph.extend(outer_graph_data)

    inner_graph_lines = [f"{str(r)},{','.join(map(str, data))}" for r, data in inner_graph if len(data) > 0]
    r_id_set = set()
    outer_graph_lines = []
    for r, data in outer_graph:
        if len(data) > 0:
            r_id_set.add(r)
            outer_graph_lines.append(f"{str(r)},{','.join(map(str, data))}")
    for tpl in tpl_dict:
        if tpl >= public_methods_size:
            continue
        if tpl not in r_id_set:
            outer_graph_lines.append(f"{str(tpl)},{','.join(map(str, sorted(tpl_dict.get(tpl))))}")

    inner_graph_save = "\n".join(inner_graph_lines)
    outer_graph_save = "\n".join(outer_graph_lines)
    write_to_zstd_file(inner_graph_save, outer_graph_save, project_path)
    logger.info(f"The graph is saved, time : {time.time() - start}")


def write_to_zstd_file(inner_graph, outer_graph, project_path):
    output_path = os.path.join(project_path, "reachable_graph")

    if not os.path.exists(output_path):
        os.mkdir(output_path)

    cctx = zstd.ZstdCompressor(level=22, threads=-1)
    start = time.time()

    if inner_graph:
        inner_graph_file = os.path.join(output_path, "inner_graph.zst")
        inner_compressed_data = cctx.compress(inner_graph.encode('utf-8'))
        with open(inner_graph_file, 'wb', buffering=65536) as f:
            f.write(inner_compressed_data)
        logger.debug(f"Write matrix to file : {inner_graph_file}")

    if outer_graph:
        outer_graph_file = os.path.join(output_path, "outer_graph.zst")
        outer_compressed_data = cctx.compress(outer_graph.encode('utf-8'))
        with open(outer_graph_file, 'wb', buffering=65536) as f:
            f.write(outer_compressed_data)
        logger.debug(f"Write matrix to file : {outer_graph_file}")
    logger.debug(f"Total time : {time.time() - start}")


def get_lines_num(file_path):
    try:
        with open(file_path, 'r') as file:
            lines = file.readlines()
            return len(lines)
    except FileNotFoundError as e:
        print(f"Error: File not found - {file_path}, {str(e)}")
        return 0


def start_bool_matrix_calculate(project_path):
    project_path = project_path.strip()
    logger.info(f"handle : {project_path}")
    edge_file_path = os.path.join(project_path, "edges.txt")
    public_methods_file_path = os.path.join(project_path, "public_methods.txt")
    public_methods_size = get_lines_num(public_methods_file_path)
    not_public_methods_file_path = os.path.join(project_path, "not_public_methods.txt")
    not_public_methods_size = get_lines_num(not_public_methods_file_path)
    if public_methods_size:
        def_methods_size = public_methods_size + not_public_methods_size
        matrix, tpl_dict, gpu_time = calculate_matrix_reachable(edge_file_path, def_methods_size)
        if matrix.nvals > 0:
            save_matrix_to_txt(matrix, tpl_dict, project_path, public_methods_size)
        elif len(tpl_dict) > 0:
            save_tpl_to_txt(tpl_dict, project_path)


if __name__ == '__main__':
    current_folder = os.path.dirname(os.path.abspath(__file__)) + '/'
    script_name = os.path.splitext(os.path.basename(__file__))[0]
    setup_logger(script_name)
    # example
    start_bool_matrix_calculate("~/.m2/repository/org/apache/package/version/")
