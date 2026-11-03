"""
Utility functions for reading from the data directory
"""
from pathlib import Path
from consts import CLEAN_OUTPUT_DIR, TRANSFORM_OUTPUT_DIR, CMUDICT, CMUDICT_PATTERN, CMUDICT_PATH


def get_all_file_paths(path: Path) -> [Path]:
    """Fetch all .cha file paths"""
    # Cleaner method than os.walk
    return [file for file in path.rglob("*.cha")]


def write_to_file(path: Path, content: [str]) -> None:
    """(Over)write file with content"""
    path.parent.mkdir(exist_ok=True, parents=True)
    path.write_text('\n'.join(content))


def get_clean_dir_path(path: Path) -> Path:
    """From a "data" path, get the path for clean dir"""
    index = path.parts.index("data")
    new_path = CLEAN_OUTPUT_DIR.joinpath(*path.parts[index + 1:])
    return extension_change_to_txt(new_path)


def get_transform_dir_path(path: Path) -> Path:
    """From a "clean" path, get the path for transform dir"""
    index = path.parts.index("clean")
    new_path = TRANSFORM_OUTPUT_DIR.joinpath(*path.parts[index + 1:])
    return new_path

def extension_change_to_txt(path: Path) -> Path:
    return path.with_suffix('.txt')

def read_cmudict_dict(path: Path = CMUDICT_PATH) -> int:
    """Read the cmudict and encode it as a hashmap for transforming"""
    # Need to use different encoding to read the file properly
    count = 0
    with open(path, encoding="latin-1") as file:
        #  Just skip the header (line 1-56)
        for line in file.readlines()[56:]:
            result = CMUDICT_PATTERN.match(line.strip())
            if result:
                CMUDICT[result.group(1)] = result.group(2)
                count += 1
    return count
