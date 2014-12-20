#include "metin2_patcher.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    metin2_patcher w;
    w.show();

    a.setStyleSheet("QPushButton{background-color: rgb(0, 102, 102);"
                    "border-width: 1px;"
                    "border-radius: 5;"
                    "color: white;"
                    "border-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0, stop: 0 darkGray, stop: 1 darkGray)"
                                  "qlineargradient(x1: 0.5, y1: 0.5, x2: 0.5, y2: 0.5, stop: 0 black, stop: 1 black)"
                                  "qlineargradient(x1: 1, y1: 1, x2: 1, y2: 1, stop: 0 darkGray, stop: 1 darkGray);}"
                    );

    return a.exec();
}
