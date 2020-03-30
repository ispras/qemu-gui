#ifndef QEMUINSTALLATIONSFORM_H
#define QEMUINSTALLATIONSFORM_H

#include <QWidget>
#include <QtWidgets>

class QemuInstallationsForm : public QDialog
{
    Q_OBJECT

public:
    QemuInstallationsForm(QWidget *parent = 0);

private:
    void editListItem(int item, const QString &caption,
                      const QString &name, const QString &dir);
    void reloadList();

    //QListWidget *qemu_install_dir_list;
    QTableWidget *list;
    // Add/Edit dialog widgets
    QLineEdit *nameEdit;
    QLineEdit *pathEdit;

private slots:
    void deleteInstallation();
};

#endif // QEMUINSTALLATIONSFORM_H
