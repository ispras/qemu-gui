#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "VMConfig.h"
#include <QtWidgets>


class GlobalConfig : public QObject
{
    Q_OBJECT

public:
    GlobalConfig(QObject *parent = 0);
    ~GlobalConfig();

    QString get_home_dir();
    QList<VMConfig *> get_exist_vm();
    VMConfig *get_vm_by_name(const QString &name);
    
    bool add_exist_vm(const QString &path);
    void delete_exclude_vm(const QString &del_vm_name, bool delete_vm);
    void exclude_vm(const QString &del_vm_name);
    void add_qemu_installation_dir(const QString &qemu_install_path);
    void del_qemu_installation_dir(const QString &qemu_install_path);
    QStringList & get_qemu_installation_dirs();
    void set_current_qemu_dir(const QString &qemu_dir);
    QString & get_current_qemu_dir();
    
    void set_terminal_parameters(QColor background, QColor text_color, const QString &font_family, int font_size);
    bool is_terminal_parameters_set();
    QColor get_terminal_backgroud();
    QColor get_terminal_text_color();
    QString get_terminal_font_family();
    int get_terminal_font_size();

private:
    QList<VMConfig *> virtual_machines;
    QStringList qemu_list;
    QString terminal_parameters_background;
    QString terminal_parameters_text_color;
    QString terminal_parameters_font_family;
    QString terminal_parameters_font_size;
    QFile *vm_config_file;
    QString path_to_home_dir;
    QString current_qemu_dir;

private:
    bool save_config_file();

public slots:
    void vm_is_created(VMConfig *vm_config);


signals:
    void globalConfig_new_vm_is_complete();

};


#endif // GLOBALCONFIG_H

