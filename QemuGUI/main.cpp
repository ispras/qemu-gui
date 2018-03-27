#include "qemugui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QemuGUI w;
    w.setWindowIcon(QIcon("Resources/qemu.png"));
    w.show();
    return a.exec();
}
