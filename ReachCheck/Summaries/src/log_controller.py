import os
from loguru import logger


def setup_logger(script_name):
    current_folder = os.path.dirname(os.path.abspath(__file__)) + '/'
    log_folder = os.path.join(current_folder, 'log')
    os.makedirs(log_folder, exist_ok=True)
    logger.add(os.path.join(log_folder, f"{script_name}_debug.log"), rotation="100 MB", level="DEBUG", enqueue=True)

    logger.add(os.path.join(log_folder, f"{script_name}_info.log"), rotation="100 MB", level="INFO", enqueue=True)
