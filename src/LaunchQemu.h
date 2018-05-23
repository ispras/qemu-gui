#ifndef LAUNCHQEMU_H
#define LAUNCHQEMU_H

#include <QWidget>
#include <QtWidgets>

#include "VMConfig.h"

class LaunchQemu : public QObject
{
    Q_OBJECT

public:
    LaunchQemu(const QString &qemu_exe, VMConfig *vm, QObject *parent = 0);
    ~LaunchQemu();

public:

private:

};


#endif // LAUNCHQEMU_H
