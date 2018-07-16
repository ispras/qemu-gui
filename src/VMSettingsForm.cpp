#include "VMSettingsForm.h"


VMSettingsForm::VMSettingsForm(VMConfig *vmconf, QWidget *parent)
    : QWidget(parent), vm(vmconf)
{
    if (VMSettingsForm::objectName().isEmpty())
        VMSettingsForm::setObjectName(QStringLiteral("VMSettingsForm"));
    resize(400, 300);
    setWindowTitle(QApplication::translate("VMSettingsForm", "VM Settings", Q_NULLPTR));
    setWindowModality(Qt::WindowModality::ApplicationModal);

    deviceTree = new QTreeWidget(this);
    new_component_line = new QLineEdit(this);
    new_component_btn = new QPushButton("Add");
    savecancel_btn = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);

    savecancel_btn->button(QDialogButtonBox::Save)->setDefault(true);
    savecancel_btn->button(QDialogButtonBox::Cancel)->setAutoDefault(true);

    new_component_btn->setFixedWidth(40);
    new_component_btn->setAutoDefault(true);

    deviceTree->setContextMenuPolicy(Qt::CustomContextMenu);
    deviceTree->setHeaderHidden(1);
    deviceTree->setColumnCount(1);

    initTree(deviceTree->invisibleRootItem(), vm->getSystemDevice());

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

    connect(savecancel_btn, &QDialogButtonBox::accepted, this, &VMSettingsForm::save_settings);
    connect(savecancel_btn, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(deviceTree, &QTreeWidget::customContextMenuRequested, this, &VMSettingsForm::showContextMenu);
}

void VMSettingsForm::widget_placement()
{
    QVBoxLayout *one = new QVBoxLayout(this);

    QHBoxLayout *edit_comp_lay = new QHBoxLayout();
    edit_comp_lay->addWidget(new_component_line);
    edit_comp_lay->addWidget(new_component_btn);

    one->addWidget(deviceTree);
    one->addLayout(edit_comp_lay);
    one->addSpacing(20);
    one->addWidget(savecancel_btn);
}

void VMSettingsForm::initTree(QTreeWidgetItem *item, const Device *device)
{
    item->setText(0, device->getDescription());
    foreach(Device *dev, device->getDevices())
    {
        QTreeWidgetItem *it = new QTreeWidgetItem();
        item->addChild(it);
        initTree(it, dev);
    }
}

void VMSettingsForm::save_settings()
{
    vm->save_vm_config();
    this->close();
}

void VMSettingsForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    //QMessageBox::about(this, "hello", item->text(0));
}

void VMSettingsForm::showContextMenu(const QPoint & pos)
{
    QTreeWidgetItem *item = deviceTree->itemAt(pos);
    deviceTree->setCurrentItem(item);

    // temp
    if (QString().compare(item->text(0), "ide.0", Qt::CaseInsensitive) == 0 || QString().compare(item->text(0), "ide.1", Qt::CaseInsensitive) == 0)
    {
        QAction *addDeviceAct = new QAction("Add device", this);
        addDeviceAct->setStatusTip(tr("Add device"));

        AddDeviceForm *addDev = new AddDeviceForm();
        addDev->setAttribute(Qt::WA_DeleteOnClose);

        connect(addDeviceAct, SIGNAL(triggered()), addDev, SLOT(addDevice()));
        connect(addDev, SIGNAL(deviceWantsToAdd(const QString &)), this, SLOT(addNewDevice(const QString &)));

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

