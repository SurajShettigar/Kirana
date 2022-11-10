from __future__ import annotations

import sys

import ui
from utils.path_utils import data_path

from window import AppWindow, DockType
from viewport import Viewport

from PySide2.QtWidgets import QApplication

import KiranaPython


class Application:
    def __init__(self, run_core: bool = False):
        self.qt_app = QApplication(sys.argv)

        self.app_window = AppWindow(data_path(ui.UI_MAIN_WINDOW_FILE_PATH))
        if not self.app_window.is_initialized():
            print("Failed to initialize Application window")
            sys.exit(-1)

        self.viewport = Viewport(self.app_window.get_dock(DockType.VIEWPORT))
        if run_core:
            self.core_app = KiranaPython.Application()
        else:
            self.core_app = None

    def run(self) -> int:
        self.app_window.show()
        res = self.viewport.get_pixel_resolution()
        print(f"Resolution: {res}")

        if self.core_app:
            self.core_app.run(self.viewport.get_render_area_pointer(), res[0], res[1])

        return self.qt_app.exec_()


if __name__ == "__main__":
    app = Application(True)
    sys.exit(app.run())
