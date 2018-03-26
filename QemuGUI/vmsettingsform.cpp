#include "VMSettingsForm.h"


VMSettingsForm::VMSettingsForm(QWidget *parent)
    : QWidget(parent)
{
    if (VMSettingsForm::objectName().isEmpty())
        VMSettingsForm::setObjectName(QStringLiteral("VMSettingsForm"));
    resize(400, 300);
    setWindowTitle(QApplication::translate("VMSettingsForm", "VM Settings", Q_NULLPTR));
    setWindowModality(Qt::WindowModality::ApplicationModal);

    deviceTree = new QTreeWidget(this);
    new_component_line = new QLineEdit(this);
    new_component_btn = new QPushButton("Add");
    new_component_btn->setFixedWidth(40);
    saveCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    deviceTree->setHeaderHidden(1);
    deviceTree->setColumnCount(1);

    QStringList list = { "CPU", "Memory", "Machine", "IDE" };

    for (int i = 0; i < list.count(); i++)
    {
        components.append(new QTreeWidgetItem((QTreeWidget *)0, QStringList(list.at(i))));
    }
    deviceTree->insertTopLevelItems(0, components);

    connect_signals();
    widget_placement();
    show();
}

VMSettingsForm::~VMSettingsForm()
{

}

void VMSettingsForm::connect_signals()
{
    connect(deviceTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onDeviceTreeItemClicked(QTreeWidgetItem *, int)));

    connect(saveCancelBtn, &QDialogButtonBox::accepted, this, &VMSettingsForm::save_settings);
    connect(saveCancelBtn, &QDialogButtonBox::rejected, this, &QWidget::close);
}

void VMSettingsForm::widget_placement()
{
    QVBoxLayout *one = new QVBoxLayout(this);

    QHBoxLayout *edit_comp_lay = new QHBoxLayout(this);
    edit_comp_lay->addWidget(new_component_line);
    edit_comp_lay->addWidget(new_component_btn);

    one->addWidget(deviceTree);
    one->addLayout(edit_comp_lay);
    one->addSpacing(20);
    one->addWidget(saveCancelBtn);
}

void VMSettingsForm::save_settings()
{
    QMessageBox::about(this, "Hello", "Its OK");
    this->close();
}

void VMSettingsForm::onDeviceTreeItemClicked(QTreeWidgetItem *item, int column)
{
    QMessageBox::about(this, "hello", item->text(0));
}