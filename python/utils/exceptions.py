# utils/exceptions.py
import sys
import traceback
from PySide6.QtWidgets import QApplication, QMessageBox


def global_exception_handler(exc_type, exc_value, exc_traceback):
    """Catch unhandled exceptions → log to console + show dialog if possible."""
    error_msg = "".join(traceback.format_exception(exc_type, exc_value, exc_traceback))
    print("UNHANDLED EXCEPTION:\n" + error_msg)

    app = QApplication.instance()
    if app and not app.closingDown():
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Critical)
        msg.setWindowTitle("Critical Error")
        msg.setText("An unexpected error occurred.\nThe application may be unstable.")
        msg.setDetailedText(error_msg[:2000])
        msg.setStandardButtons(QMessageBox.Ok)
        msg.exec()

    # Let default handler finish (crash with info)
    sys.__excepthook__(exc_type, exc_value, exc_traceback)


def install_global_exception_handler():
    sys.excepthook = global_exception_handler