#ifndef QEMUGUI_H
#define QEMUGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>

#include "VMSettingsForm.h"
#include "CreateVMForm.h"
#include "RecordReplayTab.h"

class QemuGUI : public QMainWindow
{
    Q_OBJECT

public:
    QemuGUI(QWidget *parent = 0);
    ~QemuGUI();

private:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    QListWidget *listVM;
    QListWidgetItem *selected_item;
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

    private slots:
    void play_machine();
    void pause_machine();
    void stop_machine();
    void create_machine();
    void import_machine();
    void edit_settings();
    void onListVMItemClicked(QListWidgetItem *item);

};

#endif // QEMUGUI_H
