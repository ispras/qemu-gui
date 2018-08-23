#include "AddDeviceForm.h"


AddDeviceForm::AddDeviceForm(const QStringList &listDevice)
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

    deviceList->addItems(listDevice);
    deviceList->setCurrentRow(0);
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

