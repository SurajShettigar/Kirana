from __future__ import annotations
from typing import List, Tuple, Callable, Any

from PySide2.QtCore import QEvent, QSize, QObject, Qt
from PySide2.QtWidgets import QWidget, QDockWidget, QMdiArea


class Viewport(QObject):
    def __init__(self, dock_widget: QDockWidget):
        super(Viewport, self).__init__()
        self.dock = dock_widget
        self.contents: QWidget = self.dock.findChild(QWidget, "viewport_contents")
        self.render_area: QMdiArea = self.dock.findChild(QMdiArea, "viewport_render_area")

        self.render_area.installEventFilter(self)
        self._size_callbacks: List[Callable[[QSize, QSize], None]] = []
        self._active_callbacks: List[Callable[[bool], None]] = []

    def eventFilter(self, event_object: QObject, event: QEvent) -> bool:
        if event_object == self.render_area:  # Filter Render Area events
            if event.type() == QEvent.Resize:  # Filter Resize events
                for c in self._size_callbacks:
                    c(event.size(), event.oldSize())
                return True
            elif event.type() == QEvent.WindowActivate or event.type() == QEvent.WindowDeactivate:
                for c in self._active_callbacks:
                    c(event.type() == QEvent.WindowActivate)

        return False

    def get_render_area_pointer(self) -> int:
        return self.render_area.winId()

    def get_size(self) -> Tuple[int, int]:
        return (self.render_area.size().width(),
                self.render_area.size().height())

    def get_pixel_resolution(self) -> Tuple[int, int]:
        return (self.render_area.size().width() * self.render_area.devicePixelRatio(),
                self.render_area.size().height() * self.render_area.devicePixelRatio())

    # Event Listeners
    def add_size_change_listener(self, callback: Callable[[QSize, QSize], None]):
        self._size_callbacks.append(callback)

    def remove_size_change_listener(self, callback: Callable[[QSize, QSize], None]):
        if self._size_callbacks.count(callback) > 0:
            self._size_callbacks.remove(callback)

    def add_active_listener(self, callback: Callable[[bool], None]):
        self._active_callbacks.append(callback)

    def remove_active_listener(self, callback: Callable[[bool], None]):
        if self._active_callbacks.count(callback) > 0:
            self._active_callbacks.remove(callback)
