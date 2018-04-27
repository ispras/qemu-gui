#ifndef QEMUGUI_H
#define QEMUGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>

#include "VMSettingsForm.h"
#include "CreateVMForm.h"
#include "RecordReplayTab.h"
#include "VMConfig.h"
#include "GlobalConfig.h"

class QemuGUI : public QMainWindow
{
    Q_OBJECT

public:
    QemuGUI(QWidget *parent = 0);
    ~QemuGUI();

public:

private:
    GlobalConfig *global_config;

    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    QListWidget *listVM;
    QAction *delete_act;
    QAction *exclude_act;

    QGroupBox *propBox;
    QLabel *info_lbl;
    QPushButton *edit_btn;
    QTabWidget *tab;
    QWidget *tab_info;

    VMSettingsForm *settingsWindow;
    CreateVMForm *createVMWindow;
    RecordReplayTab *recReplayTab;

private:
    void connect_signals();
    void widget_placement();
    void fill_listVM_from_config();

    private slots:
    void refresh();
    void delete_vm_ctxmenu();
    void exclude_vm_ctxmenu();
    void play_machine();
    void pause_machine();
    void stop_machine();
    void create_machine();
    void add_machine();
    void edit_settings();
    void listVM_item_selection_changed();
    void listVM_current_item_changed(QListWidgetItem *current, QListWidgetItem *previous);

};

#endif // QEMUGUI_H

