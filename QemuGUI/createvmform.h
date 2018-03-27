#ifndef CREATEVMFORM_H
#define CREATEVMFORM_H

#include <QWidget>
#include <QtWidgets>

#define MAX_RAM_VALUE 32768 / 2
#define MIN_RAM_VALUE 4

class CreateVMForm : public QWidget
{
    Q_OBJECT

public:
    CreateVMForm(QWidget *parent = 0);
    ~CreateVMForm();

private:
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
    void setVisibleWidgetsForNewHDD(bool isVisible);
    void setVisibleWidgetsForExistedHDD(bool isVisible);

    private slots:
    void hdd_no(bool state);
    void hdd_exist(bool state);
    void hdd_new(bool state);
    void place_disk_or_project();

};

#endif // CREATEVMFORM_H
