"""Simple helper script to delete output files"""
from shutil import rmtree
from consts import CLEAN_OUTPUT_DIR, TRANSFORM_OUTPUT_DIR

# Should still work on lab machine
if CLEAN_OUTPUT_DIR.exists(): rmtree(CLEAN_OUTPUT_DIR)
if TRANSFORM_OUTPUT_DIR.exists(): rmtree(TRANSFORM_OUTPUT_DIR)
