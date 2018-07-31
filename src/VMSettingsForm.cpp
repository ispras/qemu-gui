#include "VMSettingsForm.h"


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
    vm->save_vm_config();
    close();
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

void VMSettingsForm::showContextMenu(const QPoint & pos)
{
    QTreeWidgetItem *item = deviceTree->itemAt(pos);
    deviceTree->setCurrentItem(item);

    // temp
    if (QString().compare(item->text(0), "ide.0", Qt::CaseInsensitive) == 0 ||
        QString().compare(item->text(0), "ide.1", Qt::CaseInsensitive) == 0)
    {
        QAction *addDeviceAct = new QAction("Add device", this);
        addDeviceAct->setStatusTip(tr("Add device"));

        AddDeviceForm *addDev = new AddDeviceForm();
        addDev->setAttribute(Qt::WA_DeleteOnClose);

        connect(addDeviceAct, SIGNAL(triggered()), addDev, SLOT(addDevice()));
        connect(addDev, SIGNAL(deviceWantsToAdd(const QString &)),
            this, SLOT(addNewDevice(const QString &)));

        QMenu menu(this);
        menu.addAction(addDeviceAct);

        QPoint pt(pos);
        menu.exec(deviceTree->mapToGlobal(pos));
    }
}

void VMSettingsForm::addNewDevice(const QString &devName)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, devName);

    QList<QTreeWidgetItem *> children = deviceTree->currentItem()->takeChildren();
    foreach(QTreeWidgetItem *it, children)
    {
        if (QString().compare(it->text(0), devName) == 0)
        {
            deviceTree->currentItem()->addChildren(children);
            QMessageBox::critical(this, "Error", "Device already exist");
            return;
        }
    }

    deviceTree->currentItem()->addChildren(children);
    deviceTree->currentItem()->addChild(item);
    deviceTree->currentItem()->setExpanded(true);
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

