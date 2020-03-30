#ifndef VMSETTINGSFORM_H
#define VMSETTINGSFORM_H

#include <QtWidgets>
#include "VMConfig.h"
#include "AddDeviceForm.h"
#include "DeviceStorage.h"
#include "DeviceUsb.h"
#include "VMPropertiesForm.h"

class VMSettingsForm : public QWidget
{
    Q_OBJECT

public:
    VMSettingsForm(VMConfig *vmconf,
        const QString &qemuDir, QWidget *parent = 0);
    ~VMSettingsForm();

public slots:

private:
    VMConfig *vm;
    AddDeviceForm *addDev;
    QList<Device *> addedDevices;
    QString pathToPlatformInfo;

    QTreeWidget *deviceTree;
    QDialogButtonBox *savecancel_btn;

    VMPropertiesForm *kernelForm;
    QLineEdit *addCmdLineParamsEdit;
    QSplitter *splitter;
    QMenu menu;

private:
    void connect_signals();
    void widget_placement();
    QWidget *emptyForm();
    Device *isDevicesValid(Device *device);
    void closeEvent(QCloseEvent *event);
    void removingDevFromDevices(Device *dev);
    bool changesVerification();

private slots:
    bool applySettings();
    void save_settings();
    void deviceTreeItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void showContextMenu(const QPoint &pos);
    void addNewDevice(Device *newDevice);
    void addNewSystemDevice(const QString &devName);
    void removeDevice();
    void menuClose();
    void closeForm();

signals:
    void settingsDeleteRecords();
    void updateVMInfo();
    bool isExecutionListNotEmpty();

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
