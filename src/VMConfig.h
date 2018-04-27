#ifndef VMCONFIG_H
#define VMCONFIG_H

#include <QWidget>
#include <QtWidgets>

class VMConfig : public QObject
{
    Q_OBJECT

public:
    VMConfig(QObject *parent = 0);
    ~VMConfig();

public:
    bool save_vm_config(QString path);
    void set_vm_info_from_xml(QString path);
    void set_name(QString name_vm_);
    void set_dir_path(QString dir_path_);
    void add_image_path(QString image_path_);
    void set_hdd(int hdd);

    QString get_name();
    QString get_dir_path();

    void remove_directory_vm(QString path);

private:
    QFile *list_of_vm_file = NULL;

    QString name_vm;
    QString dir_path;
    QString image_path;

};

#endif VMCONFIG_H

