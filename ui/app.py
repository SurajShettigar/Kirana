from __future__ import annotations

import sys

import ui
from utils.path_utils import data_path

from window import AppWindow, DockType
from viewport import Viewport

from PySide2.QtCore import QSize
from PySide2.QtWidgets import QApplication

import KiranaPython


class Application:
    def __init__(self, run_core: bool = False):
        if run_core:
            self.core_app = KiranaPython.Application()
        else:
            self.core_app = None

        self.qt_app = QApplication(sys.argv)

        self.app_window = AppWindow(data_path(ui.UI_MAIN_WINDOW_FILE_PATH))
        self.app_window.add_close_listener(self.on_app_window_closed)
        if not self.app_window.is_initialized():
            print("Failed to initialize Application window")
            sys.exit(-1)

        self.viewport = Viewport(self.app_window.get_dock(DockType.VIEWPORT))
        self.viewport.add_size_change_listener(self.on_viewport_size_changed)

    def on_app_window_closed(self):
        sys.exit(0)

    def on_viewport_size_changed(self, new_size: QSize, old_size: QSize):
        pass

    def run(self) -> int:
        self.app_window.show()
        res = self.viewport.get_pixel_resolution()
        if self.core_app:
            self.core_app.run(self.viewport.get_render_area_pointer(), res[0], res[1])
        return self.qt_app.exec_()


if __name__ == "__main__":
    enable_core = True
    if len(sys.argv) > 1:
        if sys.argv[1].startswith('-vp'):
            enable_core = sys.argv[1].split('=')[1].lower() == 'true'
    app = Application(enable_core)
    sys.exit(app.run())
