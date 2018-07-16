#include "AddDeviceForm.h"


AddDeviceForm::AddDeviceForm(QWidget *parent)
    : QWidget(parent)
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
    deviceList->addItems(QStringList({ "device0", "device1", "device2" }));
    deviceList->setCurrentRow(0);

    QVBoxLayout *mainLay = new QVBoxLayout(this);
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addWidget(addBtn);
    btnLay->addWidget(cancelBtn);

    mainLay->addWidget(deviceList);
    mainLay->addLayout(btnLay);

    setLayout(mainLay);

    connect(addBtn, &QPushButton::clicked, this, &AddDeviceForm::addNewDevice);
    connect(cancelBtn, &QPushButton::clicked, this, &QWidget::close);
}

AddDeviceForm::~AddDeviceForm()
{

}

void AddDeviceForm::addDevice()
{
    show();
}

void AddDeviceForm::addNewDevice()
{
    emit deviceWantsToAdd(deviceList->currentItem()->text());
    close();
}

