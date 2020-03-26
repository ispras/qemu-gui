#ifndef QEMULIST_H
#define QEMULIST_H

#include <QtCore/QtCore>

class QemuList
{
private:
    QemuList();
    QemuList(const QemuList &) = delete;
    QemuList &operator=(const QemuList &) = delete;
    static QemuList &instance();

public:
    static void addQemuInstallation(const QString &qemu_install_path);
    static void delQemuInstallation(const QString &qemu_install_path);
    static const QStringList &getAllQemuInstallations();

private:
    void loadConfig();
    void saveConfig();

private:
    QStringList qemuList;
};

#endif // QEMULIST_H
