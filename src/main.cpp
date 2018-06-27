#include "QemuGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);
    QemuGUI w;
    w.setWindowIcon(QIcon(":Resources/qemu.png"));
    w.show();
    return a.exec();
}
