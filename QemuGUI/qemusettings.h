#ifndef QEMUSETTINGS_H
#define QEMUSETTINGS_H

#include <QtWidgets>
#include "ui_qemusettings.h"

class QemuSettings : public QWidget
{
	Q_OBJECT

public:
	QemuSettings(QWidget *parent = 0);
	~QemuSettings();

private:
	Ui::QemuSettings ui;

	QTreeWidget *deviceTree;
	QDialogButtonBox *saveCancelBtn;
	QLineEdit *new_component_line;
	QPushButton *new_component_btn;

	QList<QTreeWidgetItem *> components;

private:
	void connect_signals();
	void widget_placement();

private slots:
	void save_settings();
	void onDeviceTreeItemClicked(QTreeWidgetItem *item, int column);
};

#endif // QEMUSETTINGS_H
