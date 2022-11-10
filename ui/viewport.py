from __future__ import annotations
from typing import Tuple

from PySide2.QtWidgets import QWidget, QDockWidget, QMdiArea


class Viewport:
    def __init__(self, dock_widget: QDockWidget):
        self.dock = dock_widget
        self.contents: QWidget = self.dock.findChild(QWidget, "viewport_contents")
        self.render_area: QMdiArea = self.dock.findChild(QMdiArea, "viewport_render_area")

    def get_render_area_pointer(self) -> int:
        return self.render_area.winId()

    def get_size(self) -> Tuple[int, int]:
        return (self.render_area.size().width(),
                self.render_area.size().height())

    def get_pixel_resolution(self) -> Tuple[int, int]:
        return (self.render_area.size().width() * self.render_area.devicePixelRatio(),
                self.render_area.size().height() * self.render_area.devicePixelRatio())
