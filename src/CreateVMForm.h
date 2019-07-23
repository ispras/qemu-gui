#ifndef CREATEVMFORM_H
#define CREATEVMFORM_H

#include <QWidget>
#include <QtWidgets>
#include "VMConfig.h"
#include "QemuImgLauncher.h"
#include "QemuGUICommon.h"
#include "VMPropertiesForm.h"


class CreateVMForm : public QWidget
{
    Q_OBJECT

public:
    CreateVMForm(const QString &home_dir = "", const QString &qemu_dir = "");
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
    QComboBox *machineCombo;
    QComboBox *cpuCombo;
    QComboBox *platformCombo;
    VMPropertiesForm *kernelForm;


    QString default_path;
    QString qemu_dir;
    QString path_to_vm;
    QString platformName;

    VMConfig *configVM;
    QProgressDialog *imgCreationDlg;

    QString platformDirPath;


private:
    void connect_signals();
    void widget_placement();
    void set_visible_widgets_for_new_hdd(bool isVisible);
    void set_visible_widgets_for_existed_hdd(bool isVisible);
    void show_error_message(const QString &error_text);

    private slots:
    void hdd_no(bool state);
    void hdd_exist(bool state);
    void hdd_new(bool state);
    void place_disk();
    void create_vm();
    void select_dir();
    void change_path(const QString &name);
    QString set_path_to_vm(const QString &home_path);
    bool input_verification(const QString &path, const QString &name);
    QStringList getInformationFromQemu(const QString &cmd);

private slots:
    void changePlatform(const QString &text);

public slots:
    void finish_qemu_img(int exitCode);

signals:
    void createVM_new_vm_is_complete(VMConfig *);

};

#endif // CREATEVMFORM_H

