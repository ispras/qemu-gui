#ifndef VMSETTINGSFORM_H
#define VMSETTINGSFORM_H

#include <QtWidgets>

class VMSettingsForm : public QWidget
{
    Q_OBJECT

public:
    VMSettingsForm(QWidget *parent = 0);
    ~VMSettingsForm();

private:
    QTreeWidget *deviceTree;
    QDialogButtonBox *savecancel_btn;
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

#endif // VMSETTINGSFORM_H
