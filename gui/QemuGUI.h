#ifndef QEMUGUI_H
#define QEMUGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>

#include "VMSettingsForm.h"
#include "CreateVMForm.h"
#include "RecordReplayTab.h"
#include "VMConfig.h"
#include "GlobalConfig.h"
#include "QemuLauncher.h"
#include "QMPInteraction.h"
#include "TerminalSettingsForm.h"
#include "TerminalTab.h"
#include "ConsoleTab.h"
#include "ConnectionSettingsForm.h"
#include "PlatformInformationReader.h"
#include "QemuRunOptions.h"

enum VMState {None, Running, Stopped};

class QemuGUI : public QMainWindow
{
    Q_OBJECT

public:
    QemuGUI(QWidget *parent = 0);
    ~QemuGUI();

    static void setWindowGeometry(QWidget *window, QWidget *parent);

protected:
    bool eventFilter(QObject *target, QEvent *event);

private:
    QemuLauncher *launch_qemu;
    QemuRunOptions *runOptions;

    VMState vm_state;
    QTcpSocket monitorSocket;
    
    QDialog *qemu_install_dir_settings;
    QListWidget *qemu_install_dir_list;
    
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QToolBar *vmToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    QDialog *runOptionsDlg;
    QCheckBox *debugCheckBox;
    QCheckBox *snapshotCheckBox;
    QCheckBox *qemuStoppedCheckBox;
    QLineEdit *cmdLineAdditionalLineEdit;
    QLineEdit *logfileNameLineEdit;
    QList<QCheckBox *> logCheckBox;
    QString logFileName;
    QStringList logOptions;

    QAction *qemu_play;
    QAction *qemu_pause;
    QAction *qemu_stop;
    QAction *editVMAct;
    
    QComboBox *qemu_install_dir_combo;

    QListWidget *listVM;
    QAction *delete_act;
    QAction *exclude_act;

    QGroupBox *propBox;
    QTextEdit *vmInfoTextEdit;
    QTabWidget *tab;
    QWidget *tab_info;
    QMPInteraction *qmp;

    CreateVMForm *createVMWindow;
    RecordReplayTab *rec_replay_tab;
    TerminalTab *terminal_tab;
    ConsoleTab *consoleTab;


    QString qmp_port = "";
    QString monitor_port = "";

    LaunchMode launchMode = LaunchMode::NORMAL;

    PlatformInformationReader *platformInfo;


private:
    QIcon set_button_icon_for_state(const QString &normal_icon, const QString &disable_icon);
    void create_qemu_install_dir_dialog();
    void createRunOptionsDialog();
    void connect_signals();
    void widget_placement();
    void fill_listVM_from_config();
    void fill_qemu_install_dir_from_config();
    void checkQemuCompatibility();
    void runOptionPrepare();

public slots:
    void setButtonsState(bool runningState);
    void stop_qemu_btn_state();
    void resume_qemu_btn_state();
    void set_connection_settings(const QString &qmp, const QString &monitor);

    void platformInfoReady();

private slots:

    void currentTabChanged(int index);
    void set_qemu_install_dir();
    void setQemuInstallDirFromForm(QListWidgetItem *item);
    void add_qemu_install_dir_btn();
    void del_qemu_install_dir_btn();
    void refresh();
    QString delete_exclude_vm(bool delete_vm);
    void delete_vm_ctxmenu();
    void exclude_vm_ctxmenu();
    void play_machine();
    void play_machine(LaunchMode mode);
    void finish_qemu(int exitCode);
    void pause_machine();
    void stop_machine();
    void create_machine();
    void add_machine();
    void setRunOptions();
    void edit_settings();
    void listVM_item_selection_changed();
    void listVM_current_item_changed(QListWidgetItem *current, QListWidgetItem *previous);
    void qemu_install_dir_combo_activated(int index);
    void qemu_install_dir_combo_index_changed(int index);
    void set_terminal_settings();
    void launch_settings();
    void overlayFailed();

signals:
    void qmpSendCommand(QMPCommands);
    void monitor_connect(int);
    void monitor_abort();
    void recordReplayEnableBtns(bool);
    void recordReplayStateVM(bool);
    void deleteRecord();
    void currentQemuChanged();

};

#endif // QEMUGUI_H

