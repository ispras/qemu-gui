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
    typedef QMap<QString, QString> Items;

    static void addQemuInstallation(const QString &name, const QString &path);
    static void delQemuInstallation(const QString &name);
    static QString getQemuDir(const QString &name);
    static QString getQemuProfilePath(const QString &name);
    static const Items &getAllQemuInstallations();

private:
    void loadConfig();
    void saveConfig();

private:
    Items qemuList;
};

#endif // QEMULIST_H
