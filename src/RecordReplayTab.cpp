#include "RecordReplayTab.h"

static const char regExpForName[] = "[A-Za-z0-9_-][A-Za-z0-9_-\\s]+";

RecordReplayTab::RecordReplayTab(QWidget *parent)
    : QWidget(parent)
{
    if (RecordReplayTab::objectName().isEmpty())
        RecordReplayTab::setObjectName(QStringLiteral("RecordReplayTab"));
    resize(400, 300);
    
    //TODO: for each vm its list
    execution_list = new QListWidget();
    rec_btn = new QPushButton("Start record");
    rpl_btn = new QPushButton("Start replay");
    rpl_btn->setEnabled(false);

    rename_act = new QAction("Rename", execution_list);
    delete_act = new QAction("Delete", execution_list);
    rename_act->setDisabled(true);
    delete_act->setDisabled(true);

    execution_list->setEditTriggers(QAbstractItemView::AnyKeyPressed | 
        QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
 
    execution_list->addItem("First");
    execution_list->addItem("Second");
    execution_list->addItem("Third");

    execution_list->addAction(rename_act);
    execution_list->addAction(delete_act);
    execution_list->setContextMenuPolicy(Qt::ActionsContextMenu);

    rec_btn->setAutoDefault(true);
    rpl_btn->setAutoDefault(true);

    widget_placement();
    connect_signals();
}

RecordReplayTab::~RecordReplayTab()
{

}

void RecordReplayTab::setRecordReplayList(VMConfig * vm)
{
    this->vm = vm;
    execution_list->clear();
    execution_list->addItems(vm->getReplayList());
    rpl_btn->setEnabled(false);
}

QString RecordReplayTab::getCurrentDirRR()
{
    return currentDirRR;
}

void RecordReplayTab::connect_signals()
{
    connect(rec_btn, SIGNAL(clicked()), this, SLOT(record_execution()));
    connect(rpl_btn, SIGNAL(clicked()), this, SLOT(replay_execution()));

    connect(execution_list, SIGNAL(itemSelectionChanged()), 
        this, SLOT(execution_listItemSelectionChanged()));

    connect(rename_act, SIGNAL(triggered()), this, SLOT(rename_ctxmenu()));
    connect(delete_act, SIGNAL(triggered()), this, SLOT(delete_ctxmenu()));
}

void RecordReplayTab::widget_placement()
{
    QVBoxLayout *main_lay = new QVBoxLayout(this);

    QHBoxLayout *lay_btn = new QHBoxLayout();
    lay_btn->addWidget(rec_btn);
    lay_btn->addWidget(rpl_btn);

    main_lay->addWidget(new QLabel("List of existing executions"));
    main_lay->addWidget(execution_list);
    main_lay->addLayout(lay_btn);
}

void RecordReplayTab::record_execution()
{
    QDir rrDir(getCommonRRDir());
    if (!rrDir.exists())
    {
        rrDir.mkdir(getCommonRRDir());
    }

    nameDirDialog = new QDialog();
    nameDirDialog->setModal(true);
    nameDirDialog->setAttribute(Qt::WA_DeleteOnClose);
    nameEdit = new QLineEdit(nameDirDialog);
    nameEdit->setValidator(new QRegExpValidator(QRegExp(regExpForName), this));
    QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(new QLabel("Input name:"));
    topLay->addWidget(nameEdit);

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addLayout(topLay);
    mainLay->addWidget(okCancelBtn);

    nameDirDialog->setLayout(mainLay);
    nameDirDialog->show();

    connect(okCancelBtn, &QDialogButtonBox::accepted,
        this, &RecordReplayTab::setRRNameDir);
    connect(okCancelBtn, &QDialogButtonBox::rejected,
        nameDirDialog, &QDialog::close);
}

void RecordReplayTab::replay_execution()
{
    if (execution_list->currentItem())
    {
        currentDirRR = getCommonRRDir() + "/" + execution_list->currentItem()->text();
        emit startRR(LaunchMode::REPLAY);
    }
}

void RecordReplayTab::execution_listItemSelectionChanged()
{
    rpl_btn->setEnabled(true);
    rename_act->setDisabled(false);
    delete_act->setDisabled(false);
}

void RecordReplayTab::rename_ctxmenu()
{
    if (execution_list->currentItem())
    {
        oldRRName = execution_list->currentItem()->text();
        execution_list->currentItem()->setFlags(execution_list->currentItem()->flags()
            | Qt::ItemIsEditable);
        RecordRRDelegate *rrDelegate = new RecordRRDelegate();
        connect(rrDelegate, SIGNAL(renamingEnded()), this, SLOT(renameRRRecord()));
        execution_list->setItemDelegateForRow(execution_list->currentRow(), rrDelegate);
        execution_list->editItem(execution_list->currentItem());
    }
}

void RecordReplayTab::delete_ctxmenu()
{    
    if (execution_list->currentItem())
    {
        int answer = QMessageBox::question(this, "Deleting", "Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            QString name = execution_list->currentItem()->text();
            vm->remove_directory_vm(getCommonRRDir() + "/" + name);
            delete execution_list->currentItem();
            execution_list->clearSelection();
            rename_act->setDisabled(true);
            delete_act->setDisabled(true);
        }
    }
}

QString RecordReplayTab::getCommonRRDir()
{
    return vm->get_dir_path() + "/RecordReplay";
}

void RecordReplayTab::renameRRRecord()
{
    QListWidgetItem *item = execution_list->currentItem();
    if (QString::compare(oldRRName, item->text()) != 0)
    {
        QDir dir(getCommonRRDir() + "/" + oldRRName);
        if (!dir.rename(getCommonRRDir() + "/" + oldRRName,
            getCommonRRDir() + "/" + item->text()))
        {
            QMessageBox::critical((QWidget *) this->parent(),
                "Error", "Record was not renamed");
        }
        oldRRName = item->text();
        item->setFlags(item->flags() & ~Qt::ItemFlag::ItemIsEditable);
    }
}

void RecordReplayTab::recordDeleteRecords()
{
    execution_list->clear();
}

void RecordReplayTab::enableBtns(bool state)
{
    rec_btn->setEnabled(state);
    rpl_btn->setEnabled(state);
}

void RecordReplayTab::setRRNameDir()
{
    QString name = nameEdit->text().trimmed();
    QList <QListWidgetItem*> items = execution_list->findItems(name, 
        Qt::MatchFlag::MatchContains);

    foreach(QListWidgetItem *it, items)
    {
        if (QString::compare(name, it->text(), Qt::CaseInsensitive) == 0)
        {
            QMessageBox::critical(this, "Error", "Name " + name + " already exists");
            return;
        }
    }

    QListWidgetItem *it = new QListWidgetItem();
    it->setText(name);
    execution_list->addItem(it);
    QDir rrDir(getCommonRRDir() + "/" + name);
    if (!rrDir.exists())
    {
        rrDir.mkdir(getCommonRRDir() + "/" + name);
    }
    currentDirRR = rrDir.path();

    nameDirDialog->close();
    emit startRR(LaunchMode::RECORD);
}


/******************************************************************************
* EXECUTION_LIST DELEGATE                                                     *
******************************************************************************/

RecordRRDelegate::RecordRRDelegate(QObject *parent)
    :QItemDelegate(parent)
{
}

QWidget *RecordRRDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit* editor =  new QLineEdit(parent);
    editor->setValidator(new QRegExpValidator(QRegExp(regExpForName)));
    
    connect(editor, SIGNAL(editingFinished()),
        this, SLOT(editingFinished()));
    
    return editor;
}

void RecordRRDelegate::setEditorData(QWidget *editor, 
    const QModelIndex &index) const
{
    QString value = index.model()->data(index).toString();
    QLineEdit *name = dynamic_cast <QLineEdit*> (editor);
    name->setText(value);
}

void RecordRRDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *name = dynamic_cast<QLineEdit*> (editor);
    model->setData(index, name->text(), Qt::EditRole);
}

void RecordRRDelegate::editingFinished()
{
    emit renamingEnded();
}

