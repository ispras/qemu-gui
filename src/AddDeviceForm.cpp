#include "AddDeviceForm.h"
#include "DeviceFactory.h"


AddDeviceForm::AddDeviceForm(const Device *device)
{
    if (AddDeviceForm::objectName().isEmpty())
        AddDeviceForm::setObjectName(QStringLiteral("AddDeviceForm"));
    resize(140, 200);
    setWindowTitle(QApplication::translate("AddDeviceForm", "Add device", Q_NULLPTR));
    setWindowModality(Qt::WindowModality::ApplicationModal);

    deviceList = new QListWidget();
    QPushButton *addBtn = new QPushButton("Add");
    QPushButton *cancelBtn = new QPushButton("Cancel");

    addBtn->setDefault(true);
    cancelBtn->setAutoDefault(true);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addWidget(addBtn);
    btnLay->addWidget(cancelBtn);

    mainLay->addWidget(deviceList);
    mainLay->addLayout(btnLay);

    setLayout(mainLay);

    connect(addBtn, &QPushButton::clicked, this, &AddDeviceForm::addNewDevice);
    connect(cancelBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(deviceList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), 
        this, SLOT(addNewDeviceDblClick(QListWidgetItem *)));

    addDevices = DeviceFactory::getDevicesForBus(device->providesBus());
    foreach(auto dev, addDevices)
    {
        devices.insert(deviceList->count(), dev);
        deviceList->addItem(dev->getDeviceTypeName());
    }
}

AddDeviceForm::~AddDeviceForm()
{
    foreach(auto dev, addDevices)
    {
        delete dev;
    }
}

int AddDeviceForm::getAddDevicesCount()
{
    return deviceList->count();
}

void AddDeviceForm::addNewDeviceDblClick(QListWidgetItem *item)
{
    addNewDevice();
}

void AddDeviceForm::addDevice()
{
    deviceList->setCurrentRow(0);
    show();
}

void AddDeviceForm::addNewDevice()
{
    Device *newDevice = devices.value(deviceList->currentRow());
    addDevices.removeOne(newDevice);
    emit deviceWantsToAdd(newDevice);
    close();
}

