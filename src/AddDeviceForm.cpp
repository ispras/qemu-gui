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

    addDevices = DeviceFactory::getDevicesForBus(device->providesBus());
    foreach(auto dev, addDevices)
    {
        deviceList->addItem(dev->getDeviceTypeName());
        devices.insert(dev->getDeviceTypeName(), dev);
    }
}

AddDeviceForm::~AddDeviceForm()
{
    if (!isDeleted)
    {
        foreach(auto dev, addDevices)
        {
            delete dev;
        }
    }
}

int AddDeviceForm::getAddDevicesCount()
{
    return deviceList->count();
}

void AddDeviceForm::addDevice()
{
    deviceList->setCurrentRow(0);
    show();
}

void AddDeviceForm::addNewDevice()
{
    Device *newDevice = devices.value(deviceList->currentItem()->text());
    foreach(auto dev, addDevices)
    {
        if (dev != newDevice)
            delete dev;
    }
    isDeleted = true;
    emit deviceWantsToAdd(newDevice);
    close();
}

