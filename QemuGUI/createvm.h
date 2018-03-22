#ifndef CREATEVM_H
#define CREATEVM_H

#include <QWidget>
#include <QtWidgets>
#include "ui_createvm.h"

#define MAX_RAM_VALUE 32768 / 2
#define MIN_RAM_VALUE 4

class CreateVM : public QWidget
{
	Q_OBJECT

public:
	CreateVM(QWidget *parent = 0);
	~CreateVM();

private:
	Ui::CreateVM ui;

	QLineEdit *name_edit;
	QComboBox *typeOS_combo;
	QComboBox *verOS_combo;
	QSlider *ram_slider;
	QSpinBox *ram_spin;
	QComboBox *format_combo;
	QSpinBox *hdsize_spin;
	QLineEdit *place_edit;
	QPushButton *place_btn;
	QLabel *place_lbl, *format_lbl, *hddsize_lbl;
	QRadioButton *hdd_no_rb;
	QRadioButton *hdd_exist_rb;
	QRadioButton *hdd_new_rb;
	QDialogButtonBox *okCancelBtn;

private:
	void connect_signals();
	void widget_placement();
	void setVisibleWidgets(bool w1, bool w2, bool w3, bool w4, bool w5, bool w6, bool w7);

private slots:
	void hdd_no(bool state);
	void hdd_exist(bool state);
	void hdd_new(bool state);
	void place_disk_or_project();

};

#endif // CREATEVM_H
