#ifndef QEMULAUNCHER_H
#define QEMULAUNCHER_H

#include <QWidget>
#include <QtWidgets>

#include "VMConfig.h"

class QemuLauncher : public QObject
{
    Q_OBJECT

public:
    QemuLauncher(const QString &qemu_install_dir_path, VMConfig *vm, QObject *parent = 0);
    ~QemuLauncher();

public:

private:
    QString qemu_dir;
    VMConfig *virtual_machine;

public slots:
    void start_qemu();
    void finish_qemu(int exitCode, QProcess::ExitStatus ExitStatus);

signals:
    void qemu_laucher_finished();

};


#endif // QEMULAUNCHER_H
