#ifndef QEMUGUI_H
#define QEMUGUI_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets>
#include "ui_qemugui.h"

#include "qemusettings.h"
#include "createvm.h"
#include "recordreplay.h"

class QemuGUI : public QMainWindow
{
	Q_OBJECT

public:
	QemuGUI(QWidget *parent = 0);
	~QemuGUI();

private:
	Ui::QemuGUIClass ui;

	QemuSettings *settingsWindow;
	CreateVM *createVMDlg;
	RecordReplay *recReplay;

	QListWidget *listVM;
	QGroupBox *propBox;
	QLabel *info_lbl;
	QPushButton *edit_btn;
	QTabWidget *tab;
	QWidget *tab_info;


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
