#include "VMSettingsForm.h"
#include "DeviceFactory.h"

VMSettingsForm::VMSettingsForm(VMConfig *vmconf, QWidget *parent)
    : QWidget(parent), vm(vmconf)
{
    if (VMSettingsForm::objectName().isEmpty())
        VMSettingsForm::setObjectName(QStringLiteral("VMSettingsForm"));
    resize(400, 300);
    setWindowTitle(QApplication::translate("VMSettingsForm", "VM Settings", Q_NULLPTR));
    setWindowModality(Qt::WindowModality::ApplicationModal);

    deviceTree = new QTreeWidget();
    savecancel_btn = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);

    savecancel_btn->button(QDialogButtonBox::Save)->setDefault(true);
    savecancel_btn->button(QDialogButtonBox::Cancel)->setAutoDefault(true);

    deviceTree->setContextMenuPolicy(Qt::CustomContextMenu);
    deviceTree->setHeaderHidden(1);
    deviceTree->setColumnCount(1);


    foreach(Device *dev, vm->getSystemDevice()->getDevices())
    {
        DeviceTreeItem *it = new DeviceTreeItem(dev);
        deviceTree->invisibleRootItem()->addChild(it);
    }

    connect_signals();
    widget_placement();
    show();
}

VMSettingsForm::~VMSettingsForm()
{

}

void VMSettingsForm::connect_signals()
{
    connect(deviceTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this,
        SLOT(onDeviceTreeItemClicked(QTreeWidgetItem *, int)));

    connect(savecancel_btn, &QDialogButtonBox::accepted,
        this, &VMSettingsForm::save_settings);
    connect(savecancel_btn, &QDialogButtonBox::rejected,
        this, &QWidget::close);

    connect(deviceTree, &QTreeWidget::customContextMenuRequested,
        this, &VMSettingsForm::showContextMenu);
}

QWidget* VMSettingsForm::emptyForm()
{
    QGroupBox *deviceInfoGroup = new QGroupBox();
    QHBoxLayout *infoLay = new QHBoxLayout();
    QLabel *infoLbl = new QLabel("<No information>", deviceInfoGroup);
    infoLay->addStretch(50);
    infoLay->addWidget(infoLbl);
    infoLay->addStretch(50);
    deviceInfoGroup->setLayout(infoLay);
    return deviceInfoGroup;
}

Device * VMSettingsForm::isDevicesValid(Device *device)
{
    Device *retDevice = NULL;
    foreach(Device *dev, device->getDevices())
    {
        if (!dev->isDeviceValid())
        {
            retDevice = dev;
        } 
        if (!retDevice)
        {
            retDevice = isDevicesValid(dev);
        }
    }
    return retDevice;
}

void VMSettingsForm::widget_placement()
{
    splitter = new QSplitter();
    splitter->addWidget(deviceTree);
    splitter->addWidget(emptyForm());

    splitter->setSizes(QList<int>{150, 250});

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addWidget(splitter);
    main->addWidget(savecancel_btn);
}

void VMSettingsForm::save_settings()
{
    Device *dev = isDevicesValid(vm->getSystemDevice());
    if (!dev)
    {
        int answer = QMessageBox::question(this, "Saving",
            "All recorded executions will be removed. Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            vm->save_vm_config();
            vm->remove_directory_vm(vm->getPathRRDir());
            emit settingsDeleteRecords();
            close();
        }
    }
    else
    {
        QMessageBox::critical(this, "Error",
            "Device " + dev->getDescription() + " has invalid information");
    }
}

void VMSettingsForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    DeviceTreeItem *devItem = dynamic_cast<DeviceTreeItem*>(item);
    Q_ASSERT(devItem);

    Device *dev = devItem->getDevice();
    QWidget *form = dev->getEditorForm();

    delete splitter->widget(1);
    if (form)
    {
        splitter->addWidget(form);
    }
    else
    {
        splitter->addWidget(emptyForm());
    }
    splitter->setSizes(QList<int>{150, 250});
}

void VMSettingsForm::showContextMenu(const QPoint &pos)
{
    // trash all
    QTreeWidgetItem *item = deviceTree->itemAt(pos);
    if (item)
    {
        deviceTree->setCurrentItem(item);
        QMenu menu(this);

        DeviceTreeItem *devItem = dynamic_cast<DeviceTreeItem*>(item);
        Q_ASSERT(devItem);
        Device *dev = devItem->getDevice();

        Devices addDevices = DeviceFactory::getDevicesForBus(dev->providesBus());
        if (!addDevices.isEmpty())
        {
            QAction *addDeviceAct = new QAction("Add device", this);
            addDeviceAct->setStatusTip(tr("Add device"));

            AddDeviceForm *addDev = new AddDeviceForm(addDevices);
            addDev->setAttribute(Qt::WA_DeleteOnClose);

            connect(addDeviceAct, SIGNAL(triggered()), addDev, SLOT(addDevice()));
            connect(addDev, SIGNAL(deviceWantsToAdd(const QString &)),
                this, SLOT(addNewDevice(const QString &)));

            menu.addAction(addDeviceAct);
        }

        bool canRemove = true;
        foreach(Device *device, vm->getSystemDevice()->getDevices())
        {
            if (dev == device)
                canRemove = false;
        }
        // TODO: protect bus items from removing
        if (canRemove)
        {
            QAction *removeDeviceAct = new QAction("Remove device", this);
            removeDeviceAct->setStatusTip(tr("Remove device"));
            connect(removeDeviceAct, SIGNAL(triggered()), this, SLOT(removeDevice()));
            menu.addAction(removeDeviceAct);
        }
        if (!menu.isEmpty())
        {
            menu.exec(deviceTree->mapToGlobal(pos));
        }

        foreach(Device *device, addDevices)
        {
            delete device;
        }
    }
    /*
    TODO
    else
    {
        qDebug() << "add add add";
        QAction *addDeviceAct = new QAction("Add system device (dont use it)", this);
        addDeviceAct->setStatusTip(tr("Add system device"));

        AddDeviceForm *addSystemDev = NULL;//new AddDeviceForm(QStringList({ "Usb", "...", }));
        addSystemDev->setAttribute(Qt::WA_DeleteOnClose);

        connect(addDeviceAct, SIGNAL(triggered()), addSystemDev, SLOT(addDevice()));
        connect(addSystemDev, SIGNAL(deviceWantsToAdd(const QString &)),
            this, SLOT(addNewSystemDevice(const QString &)));

        QMenu menu(this);
        menu.addAction(addDeviceAct);

        menu.exec(deviceTree->mapToGlobal(pos));
    }
    */
}

void VMSettingsForm::addNewDevice(const QString &devName)
{
    DeviceTreeItem *devItem = dynamic_cast<DeviceTreeItem*>(deviceTree->currentItem());
    Q_ASSERT(devItem);
    Device *dev = devItem->getDevice();
    if (devItem->childCount() < 2) /* temp */
    {
        Device *newDev = NULL;
        if (!devName.compare("DeviceIdeCdrom"))
        {
            newDev = new DeviceIdeCdrom("", dev);
        }
        else if (!devName.compare("DeviceIdeHd"))
        {
            newDev = new DeviceIdeHd("", dev);
        }
        DeviceTreeItem *it = new DeviceTreeItem(newDev);
        deviceTree->currentItem()->addChild(it);
        deviceTree->setCurrentItem(it);
        onDeviceTreeItemClicked(it, 0);
    }
    else
    {
        QMessageBox::critical(this, "Error", "Too much devices");
    }
}

void VMSettingsForm::addNewSystemDevice(const QString &devName)
{
    if (QString::compare(devName, "Usb") == 0)
    {
        vm->addUsbDevice();
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, "Usb");
        deviceTree->invisibleRootItem()->addChild(item);
    }
}

void VMSettingsForm::removeDevice()
{
    DeviceTreeItem *devItem = dynamic_cast<DeviceTreeItem *>(deviceTree->currentItem());
    Q_ASSERT(devItem);
    Device *dev = devItem->getDevice();
    int answer = QMessageBox::question(this, "Removing",
        "Device " + dev->getDescription() + " will be removed. Are you sure?",
        QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        Device *devParent = (Device *)dev->parent();
        devParent->removeDevice(dev);
        deviceTree->setCurrentItem(devItem->parent());
        onDeviceTreeItemClicked(devItem->parent(), 0);
        delete devItem;        
    }
}

/***************************************************************************
 * DeviceTreeItem                                                          *
 ***************************************************************************/

DeviceTreeItem::DeviceTreeItem(Device *dev)
{
    initDevice(dev);
}

void DeviceTreeItem::initDevice(Device *dev)
{
    device = dev;
    setText(0, device->getDescription());
    foreach(Device *dev, device->getDevices())
    {
        DeviceTreeItem *it = new DeviceTreeItem(dev);
        addChild(it);
    }
}

Device *DeviceTreeItem::getDevice()
{
    return device;
}

