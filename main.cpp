#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qApp->setStyleSheet(R"(
        QWidget {
            background-color: #f4f6f9;
            font-family: Segoe UI;
            font-size: 14px;
        }
        QPushButton {
            background-color: #2d89ef;
            color: white;
            border-radius: 8px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #1b5fa7;
        }
        QLineEdit {
            border: 1px solid #b0b0b0;
            border-radius: 6px;
            padding: 4px;
        }
    )");
    

    QApplication::setApplicationName("DSA Social Media");
    QApplication::setOrganizationName("MyCompany");
    QApplication::setApplicationVersion("1.0");

    MainWindow w;
    w.show();
    return a.exec();
}
