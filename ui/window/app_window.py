from __future__ import annotations
from typing import Tuple

from enum import Enum

from PySide2.QtUiTools import QUiLoader
from PySide2.QtCore import QFile, QIODevice
from PySide2.QtWidgets import QDockWidget


class DockType(Enum):
    VIEWPORT = 0,
    SCENE_HIERARCHY = 1,
    OBJECT_PROPERTIES = 2


class AppWindow:
    def __init__(self, file_path: str):
        ui_file = QFile(file_path)
        if not ui_file.open(QIODevice.ReadOnly):
            print(f"Cannot open Main Window UI file")
        loader = QUiLoader()
        self.qt_window = loader.load(ui_file)
        ui_file.close()

    def is_initialized(self) -> bool:
        return self.qt_window is not None

    def show(self):
        self.qt_window.show()

    def get_window_pointer(self) -> int:
        return self.qt_window.winId()

    def get_size(self) -> Tuple[int, int]:
        return (self.qt_window.size().width(),
                self.qt_window.size().height())

    def get_pixel_resolution(self) -> Tuple[int, int]:
        return (self.qt_window.size().width() * self.qt_window.devicePixelRatio(),
                self.qt_window.size().height() * self.qt_window.devicePixelRatio())

    def get_dock(self, dock_type: DockType) -> QDockWidget:
        if dock_type == DockType.VIEWPORT:
            return self.qt_window.findChild(QDockWidget, "viewport_dock")
        if dock_type == DockType.SCENE_HIERARCHY:
            return self.qt_window.findChild(QDockWidget, "scene_hierarchy_dock")
        if dock_type == DockType.OBJECT_PROPERTIES:
            return self.qt_window.findChild(QDockWidget, "object_properties_dock")
