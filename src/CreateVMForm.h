#ifndef CREATEVMFORM_H
#define CREATEVMFORM_H

#include <QWidget>
#include <QtWidgets>
#include "VMConfig.h"


#define MAX_RAM_VALUE 32768 / 2
#define MIN_RAM_VALUE 4

enum HDD_STATE {NO_HDD, EXIST_HDD, NEW_HDD};


class CreateVMForm : public QWidget
{
    Q_OBJECT

public:
    CreateVMForm(const QString &home_dir = "");
    ~CreateVMForm();

private:
    QLineEdit *name_edit;
    QLineEdit *pathtovm_edit;
    QPushButton *pathtovm_btn;
    QComboBox *typeOS_combo;
    QComboBox *verOS_combo;
    QSlider *ram_slider;
    QSpinBox *ram_spin;
    QComboBox *format_combo;
    QSlider *hddsize_slider;
    QSpinBox *hddsize_spin;
    QLineEdit *imageplace_edit;
    QPushButton *imageplace_btn;
    QLabel *imageplace_lbl, *format_lbl, *hddsize_lbl;
    QRadioButton *hdd_no_rb;
    QRadioButton *hdd_exist_rb;
    QRadioButton *hdd_new_rb;
    QDialogButtonBox *okcancel_btn;
    QLabel *error_lbl;

    QString default_path;
    QString path_to_vm;


private:
    void connect_signals();
    void widget_placement();
    void set_visible_widgets_for_new_hdd(bool isVisible);
    void set_visible_widgets_for_existed_hdd(bool isVisible);
    void show_error_message(QString error_text);

    private slots:
    void hdd_no(bool state);
    void hdd_exist(bool state);
    void hdd_new(bool state);
    void place_disk_or_project();
    void create_vm();
    void select_dir();
    void change_path(const QString &name);


signals:
    void createVM_new_vm_is_complete(VMConfig *);

};

#endif // CREATEVMFORM_H

