#ifndef QEMUGUI_H
#define QEMUGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include <QTcpSocket>

#include "VMSettingsForm.h"
#include "CreateVMForm.h"
#include "RecordReplayTab.h"
#include "VMConfig.h"
#include "GlobalConfig.h"
#include "QemuLauncher.h"

enum VMState {None, Running, Stopped};

class QemuGUI : public QMainWindow
{
    Q_OBJECT

public:
    QemuGUI(QWidget *parent = 0);
    ~QemuGUI();

private:
    GlobalConfig *global_config;
    QemuLauncher *launch_qemu;

    VMState vm_state;
    QTcpSocket monitorSocket;

    QDialog *qemu_install_dir_settings;
    QListWidget *qemu_install_dir_list;
    
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    QAction *qemu_play;
    QAction *qemu_pause;
    QAction *qemu_stop;

    QComboBox *qemu_install_dir_combo;

    QListWidget *listVM;
    QAction *delete_act;
    QAction *exclude_act;

    QGroupBox *propBox;
    QLabel *info_lbl;
    QPushButton *edit_btn;
    QTabWidget *tab;
    QWidget *tab_info;
    QWidget *tab_terminal;
    QTextEdit *terminal_text;

    VMSettingsForm *settingsWindow;
    CreateVMForm *createVMWindow;
    RecordReplayTab *rec_replay_tab;

private:
    void create_qemu_install_dir_dialog();
    void connect_signals();
    void widget_placement();
    void fill_listVM_from_config();
    void fill_qemu_install_dir_from_config();

    private slots:
    void set_qemu_install_dir();
    void add_qemu_install_dir_btn();
    void del_qemu_install_dir_btn();
    void refresh();
    QString delete_exclude_vm(bool delete_vm);
    void delete_vm_ctxmenu();
    void exclude_vm_ctxmenu();
    void play_machine();
    void finish_qemu();
    void pause_machine();
    void stop_machine();
    void create_machine();
    void add_machine();
    void edit_settings();
    void listVM_item_selection_changed();
    void listVM_current_item_changed(QListWidgetItem *current, QListWidgetItem *previous);
    void qemu_install_dir_combo_activated(int index);
    void qemu_install_dir_combo_index_changed(int index);
    void read_terminal();


};

#endif // QEMUGUI_H

