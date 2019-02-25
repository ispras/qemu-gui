#ifndef VMSETTINGSFORM_H
#define VMSETTINGSFORM_H

#include <QtWidgets>
#include "VMConfig.h"
#include "AddDeviceForm.h"
#include "DeviceStorage.h"
#include "DeviceUsb.h"

class VMSettingsForm : public QWidget
{
    Q_OBJECT

public:
    VMSettingsForm(VMConfig *vmconf, QWidget *parent = 0);
    ~VMSettingsForm();

public slots:

private:
    VMConfig *vm;
    AddDeviceForm *addDev;

    QTreeWidget *deviceTree;
    QDialogButtonBox *savecancel_btn;

    QLineEdit *addCmdLineParamsEdit;
    QSplitter *splitter;
    QMenu menu;

private:
    void connect_signals();
    void widget_placement();
    QWidget *emptyForm();
    Device *isDevicesValid(Device *device);

private slots:
    void save_settings();
    void onDeviceTreeItemClicked(QTreeWidgetItem *item, int column);
    void showContextMenu(const QPoint &pos);
    void addNewDevice(Device *newDevice);
    void addNewSystemDevice(const QString &devName);
    void removeDevice();
    void menuClose();

signals:
    void settingsDeleteRecords();

};

class DeviceTreeItem : public QTreeWidgetItem
{
public:
    DeviceTreeItem(Device *dev);

    void initDevice(Device *dev);
    Device *getDevice();

private:
    Device *device;
};

#endif // VMSETTINGSFORM_H
