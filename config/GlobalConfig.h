#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <QtCore/QtCore>

#include "VMConfig.h"

class GlobalConfig
{
    GlobalConfig();
    GlobalConfig(const GlobalConfig &) = delete;
    GlobalConfig &operator=(const GlobalConfig &) = delete;

    static GlobalConfig &instance();

public:
    static QString get_home_dir();
    static QList<VMConfig *> get_exist_vm();
    static VMConfig *get_vm_by_name(const QString &name);
    
    static bool add_exist_vm(const QString &path);
    static void delete_exclude_vm(const QString &del_vm_name, bool delete_vm);
    static void exclude_vm(const QString &del_vm_name);
    static void set_current_qemu(const QString &qemu);
    static QString get_current_qemu();
    static QString get_current_qemu_dir();
    
    static void set_terminal_parameters(QColor background, QColor text_color, const QString &font_family, int font_size);
    static QColor get_terminal_backgroud();
    static QColor get_terminal_text_color();
    static QString get_terminal_font_family();
    static int get_terminal_font_size();

    static QString get_port_qmp();
    static QString get_port_monitor();
    static void set_port_qmp(const QString &port);
    static void set_port_monitor(const QString &port);

    static void vm_is_created(VMConfig *vm_config);

private:
    QList<VMConfig *> virtual_machines;
    QString terminal_parameters_background;
    QString terminal_parameters_text_color;
    QString terminal_parameters_font_family;
    QString terminal_parameters_font_size;
    QFile *vm_config_file;
    QString current_qemu;
    QString port_qmp;
    QString port_monitor;

private:
    bool save_config_file();
};

#endif // GLOBALCONFIG_H
