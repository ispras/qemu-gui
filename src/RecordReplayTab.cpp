#include "RecordReplayTab.h"
#include "PlatformInformationReader.h"

static const char regExpForName[] = "[A-Za-z0-9_-][A-Za-z0-9_-\\s]+";
const QString constXmlName = "replay.xml";
const QString xml_hash = "QemuHash";
const QString xml_icount = "icount";
const QString xml_start = "replay_";

RecordReplayTab::RecordReplayTab(GlobalConfig *globalConfig, QWidget *parent)
    : QWidget(parent), globalConfig(globalConfig)
{
    if (RecordReplayTab::objectName().isEmpty())
        RecordReplayTab::setObjectName(QStringLiteral("RecordReplayTab"));
    resize(400, 300);
    
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
 
    execution_list->addAction(rename_act);
    execution_list->addAction(delete_act);
    execution_list->setContextMenuPolicy(Qt::ActionsContextMenu);

    rec_btn->setAutoDefault(true);
    rpl_btn->setAutoDefault(true);

    widget_placement();
    connect_signals();

    isNotRunning = true;
    oldRRName = "";
}

RecordReplayTab::~RecordReplayTab()
{

}

void RecordReplayTab::setRecordReplayList(VMConfig *vm)
{
    this->vm = vm;
    execution_list->clear();
    execution_list->addItems(vm->getReplayList());
    if (execution_list->count())
    {
        execution_list->setCurrentRow(0);
    }
    rpl_btn->setEnabled(false);
}

QString RecordReplayTab::getCurrentDirRR()
{
    return currentDirRR;
}

QString RecordReplayTab::getICountValue()
{
    return icountValue;
}

void RecordReplayTab::connect_signals()
{
    connect(rec_btn, SIGNAL(clicked()), this, SLOT(record_execution()));
    connect(rpl_btn, SIGNAL(clicked()), this, SLOT(replay_execution()));

    connect(execution_list, SIGNAL(itemSelectionChanged()), 
        this, SLOT(executionListItemSelectionChanged()));
    connect(execution_list, SIGNAL(itemActivated(QListWidgetItem *)),
        this, SLOT(executionListItemActivated(QListWidgetItem *)));
    //connect(execution_list, SIGNAL(itemClicked(QListWidgetItem *)),
    //    this, SLOT(executionListItemActivated(QListWidgetItem *)));

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

void RecordReplayTab::createXml(const QString &path, const QString &name)
{
    QFile file(path + "/" + constXmlName);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(xml_start + name);

        xmlWriter.writeStartElement(xml_hash);
        xmlWriter.writeCharacters(PlatformInformationReader::getQemuHash(
            globalConfig->get_current_qemu_dir()));
        xmlWriter.writeEndElement();
             
        xmlWriter.writeStartElement(xml_icount);
        xmlWriter.writeCharacters(icountValue);
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
    }
}

void RecordReplayTab::readXml(const QString &name)
{
    QFile file(currentDirRR + "/" + constXmlName);
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);
        xmlReader.readNextStartElement();
        Q_ASSERT(xmlReader.name() == xml_start + name);

        while (xmlReader.readNextStartElement())
        {
            if (xmlReader.name() == xml_hash)
            {
                qemuHash = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_icount)
            {
                icountValue = xmlReader.readElementText();
            }
        }
    }
}

void RecordReplayTab::setCurrentDir(const QString & name)
{
    currentDirRR = vm->getPathRRDir() + "/" + name;
}

void RecordReplayTab::record_execution()
{
    QDir rrDir(vm->getPathRRDir());
    if (!rrDir.exists())
    {
        rrDir.mkdir(vm->getPathRRDir());
    }

    nameDirDialog = new QDialog();
    nameDirDialog->setWindowTitle("New execution");
    nameDirDialog->setModal(true);
    nameDirDialog->setAttribute(Qt::WA_DeleteOnClose);
    nameEdit = new QLineEdit(nameDirDialog);
    nameEdit->setValidator(new QRegExpValidator(QRegExp(regExpForName), this));
    icountSpin = new QSpinBox();
    icountSpin->setMinimum(1);
    icountSpin->setMaximum(12);
    icountSpin->setValue(5);
    
    QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(new QLabel("Execution name:"));
    topLay->addWidget(nameEdit);

    QHBoxLayout *bottomLay = new QHBoxLayout();
    bottomLay->addWidget(new QLabel("icount value:"));
    bottomLay->addWidget(icountSpin);

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addLayout(topLay);
    mainLay->addLayout(bottomLay);
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
        setCurrentDir(execution_list->currentItem()->text());
        emit startRR(LaunchMode::REPLAY);
    }
}

void RecordReplayTab::executionListItemSelectionChanged()
{
    if (execution_list->count() && isNotRunning)
    {
        setCurrentDir(execution_list->currentItem()->text());
        readXml(execution_list->currentItem()->text());
        if (qemuHash.compare(PlatformInformationReader::getQemuHash(globalConfig->get_current_qemu_dir())) == 0)
        {
            rpl_btn->setEnabled(true);
            rename_act->setDisabled(false);
            delete_act->setDisabled(false);
        }
        else
        {
            rpl_btn->setEnabled(false);
        }
    }
    else
    {
        rename_act->setDisabled(true);
        delete_act->setDisabled(true);
    }
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
            vm->remove_directory_vm(currentDirRR);
            delete execution_list->currentItem();
            execution_list->clearSelection();
            rename_act->setDisabled(true);
            delete_act->setDisabled(true);
        }
    }
}

void RecordReplayTab::renameRRRecord()
{
    QListWidgetItem *item = execution_list->currentItem();
    if (QString::compare(oldRRName, item->text()) != 0)
    {
        readXml(oldRRName);
        QDir dir(currentDirRR);
        if (!dir.rename(currentDirRR, vm->getPathRRDir() + "/" + item->text()))
        {
            QMessageBox::critical((QWidget *) this->parent(),
                "Error", "Record was not renamed");
            execution_list->currentItem()->setText(oldRRName);
            return;
        }
        oldRRName = item->text();
        item->setFlags(item->flags() & ~Qt::ItemFlag::ItemIsEditable);
        setCurrentDir(item->text());
        createXml(currentDirRR, item->text());
    }
}

void RecordReplayTab::executionListItemActivated(QListWidgetItem *item)
{
    executionListItemSelectionChanged();
}

void RecordReplayTab::recordDeleteRecords()
{
    execution_list->clear();
}

void RecordReplayTab::deleteRecordFolder()
{
    QString name = execution_list->item(execution_list->count() - 1)->text();
    vm->remove_directory_vm(currentDirRR);
    delete execution_list->item(execution_list->count() - 1);
    execution_list->clearSelection();
    rename_act->setDisabled(true);
    delete_act->setDisabled(true);
}

void RecordReplayTab::enableBtns(bool state)
{
    isNotRunning = state;
    rec_btn->setEnabled(state);
    rpl_btn->setEnabled(execution_list->currentItem() && state);
}

void RecordReplayTab::setRRNameDir()
{
    if (!nameEdit->text().isEmpty())
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

        nameReplay = nameEdit->text();
        icountValue = QString::number(icountSpin->value());

        setCurrentDir(name);
        QListWidgetItem *it = new QListWidgetItem();
        it->setText(name);
        execution_list->addItem(it);
        QDir rrDir(currentDirRR);
        if (!rrDir.exists())
        {
            rrDir.mkdir(currentDirRR);
        }
        createXml(currentDirRR, name);

        nameDirDialog->close();
        emit startRR(LaunchMode::RECORD);
    }
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

