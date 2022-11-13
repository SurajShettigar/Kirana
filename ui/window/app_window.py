from __future__ import annotations
from typing import Tuple, Callable, List

from enum import Enum

from PySide2.QtUiTools import QUiLoader
from PySide2.QtCore import QFile, QIODevice, Qt, QObject, QEvent
from PySide2.QtWidgets import QDockWidget, QMainWindow


class DockType(Enum):
    VIEWPORT = 0,
    SCENE_HIERARCHY = 1,
    OBJECT_PROPERTIES = 2


class AppWindow(QObject):
    def __init__(self, file_path: str):
        super(AppWindow, self).__init__()
        ui_file = QFile(file_path)
        if not ui_file.open(QIODevice.ReadOnly):
            print(f"Cannot open Main Window UI file")
        loader = QUiLoader()
        self.qt_window: QMainWindow = loader.load(ui_file)
        self.qt_window.installEventFilter(self)
        ui_file.close()

        self._close_callbacks: List[Callable[[None], None]] = []

    def eventFilter(self, event_object: QObject, event: QEvent) -> bool:
        if event_object == self.qt_window:
            if event.type() == QEvent.WindowStateChange:
                if self.qt_window.isMinimized():
                    # TODO: Send minimize event
                    pass
                else:
                    # TODO: Send restore event
                    pass
                if self.qt_window.isMaximized():
                    # TODO: Send maximize event
                    pass
            elif event.type() == QEvent.Close:
                for c in self._close_callbacks:
                    c()
        return False

    def is_initialized(self) -> bool:
        return self.qt_window is not None

    # Event Listeners
    def add_close_listener(self, callback: Callable[[None], None]):
        self._close_callbacks.append(callback)

    def remove_close_listener(self, callback: Callable[[None], None]):
        if self._close_callbacks.count(callback) > 0:
            self._close_callbacks.remove(callback)

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
