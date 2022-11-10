from __future__ import annotations

import io
import os
import sys
import tempfile

import pathlib
from pathlib import Path


def get_str_literals(string: str) -> str:
    new_string = str(string)
    if not new_string.startswith('"'):
        new_string = f'"{new_string}'
    if not new_string.endswith('"'):
        new_string = f'{new_string}"'
    return new_string


def get_name_from_path(path: str | Path) -> str:
    return Path(path).name


def get_name_from_label(label: str) -> str:
    return label.lower().replace(" ", "_")


def get_extension(path: str | Path) -> str:
    return Path(path).suffix


def data_path(path: str | Path) -> str:
    if hasattr(sys, '_MEIPASS'):
        new_path = Path(sys._MEIPASS) / Path(path)
    else:
        new_path = Path(path)
    return new_path.resolve().as_posix()


def get_absolute_path(relative_path: str | Path, cwd: str | Path | None = None) -> str:
    path = Path(relative_path)
    if cwd is not None:
        if Path(cwd).is_file():
            path = Path(cwd).parent / Path(relative_path)
        else:
            path = Path(cwd) / Path(relative_path)
    return path.resolve().as_posix()


def list_files(path: str | Path, recursive: bool = True, extensions: list[str] | None = None) -> list[str]:
    # Convert to Path object
    path = Path(path)

    # clean extensions
    if extensions is not None:
        for i in range(len(extensions)):
            if extensions[i].startswith('.'):
                extensions[i] = extensions[i][1:]

    # Check if already a file
    if path.is_file():
        ext = path.suffix.lower()[1:]
        # Check if the file has given extension or else return None
        if extensions is not None and ext in extensions:
            return [str(path.resolve().as_posix())]
        else:
            return []

    # find files of given extension
    found_files = []

    if extensions is None:
        if recursive:
            for p in path.rglob('*.*'):
                found_files.append(str(p.resolve().as_posix()))
        else:
            for p in path.glob('*.*'):
                found_files.append(str(p.resolve().as_posix()))
    else:
        for e in extensions:
            if recursive:
                for p in path.rglob('*.{}'.format(e)):
                    found_files.append(str(p.resolve().as_posix()))
            else:
                for p in path.glob('*.{}'.format(e)):
                    found_files.append(str(p.resolve().as_posix()))

    return found_files


def read_file(path: str | Path) -> str:
    path = Path(path)
    contents = ""
    if path.exists():
        contents = path.read_text() or ""
    return contents


def get_temp_file(file_contents: str, name: str | None = None, extension: str = ".txt"):
    if not extension.startswith('.'):
        extension = '.' + extension

    file = tempfile.NamedTemporaryFile(prefix=name, suffix=extension, delete=False)
    file.write(file_contents.encode('utf-8'))
    file.seek(0)
    return file


def delete_temp_file(file: io.TextIOWrapper) -> bool:
    file.close()
    os.unlink(file.name)
    return not os.path.exists(file.name)
