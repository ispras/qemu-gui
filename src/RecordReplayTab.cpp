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
    
    executionList = new QListWidget();
    rec_btn = new QPushButton("Start record");
    rpl_btn = new QPushButton("Start replay");
    rec_btn->setEnabled(false);
    rpl_btn->setEnabled(false);

    renameAct = new QAction("Rename", executionList);
    deleteAct = new QAction("Delete", executionList);
    renameAct->setDisabled(true);
    deleteAct->setDisabled(true);

    executionList->setEditTriggers(QAbstractItemView::AnyKeyPressed | 
        QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
 
    executionList->addAction(renameAct);
    executionList->addAction(deleteAct);
    executionList->setContextMenuPolicy(Qt::ActionsContextMenu);

    rec_btn->setAutoDefault(true);
    rpl_btn->setAutoDefault(true);

    widget_placement();
    connect_signals();

    isNotRunning = true;
    oldRRName = "";
    periodAutoSnap = "";
}

RecordReplayTab::~RecordReplayTab()
{

}

void RecordReplayTab::setRecordReplayList(VMConfig *virtualMachine)
{
    disconnect(executionList, 0, 0, 0);

    vm = virtualMachine;
    rec_btn->setEnabled(true);
    executionList->clear();

    connect(executionList, SIGNAL(currentRowChanged(int)),
        this, SLOT(executionListItemRowChanged(int)));
    connect(executionList, SIGNAL(itemClicked(QListWidgetItem *)),
        this, SLOT(executionListItemClicked(QListWidgetItem *)));

    executionList->addItems(vm->getReplayList());
    if (vm->getReplayList().count())
    {
        executionList->setCurrentRow(0);
    }
    else
    {
        rpl_btn->setEnabled(false);
    }
}

QString RecordReplayTab::getCurrentDirRR()
{
    return currentDirRR;
}

QString RecordReplayTab::getICountValue()
{
    return icountValue;
}

QString RecordReplayTab::getSnapshotPeriod()
{
    return periodAutoSnap;
}

void RecordReplayTab::setSnapshotPeriod(QString val)
{
    periodAutoSnap = val;
}

void RecordReplayTab::clearExecutionList()
{
    disconnect(executionList, 0, 0, 0);
    executionList->clear();
}

void RecordReplayTab::connect_signals()
{
    connect(rec_btn, SIGNAL(clicked()), this, SLOT(record_execution()));
    connect(rpl_btn, SIGNAL(clicked()), this, SLOT(replay_execution()));

    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename_ctxmenu()));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(delete_ctxmenu()));
}

void RecordReplayTab::widget_placement()
{
    QVBoxLayout *main_lay = new QVBoxLayout(this);

    QHBoxLayout *lay_btn = new QHBoxLayout();
    lay_btn->addWidget(rec_btn);
    lay_btn->addWidget(rpl_btn);

    main_lay->addWidget(new QLabel("List of existing executions"));
    main_lay->addWidget(executionList);
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

void RecordReplayTab::setCurrentDir(const QString &name)
{
    currentDirRR = vm->getPathRRDir() + "/" + name;
}

void RecordReplayTab::createDialog(const QString &caption)
{
    replayDialog = new QDialog();
    replayDialog->setWindowTitle(caption);
    replayDialog->setModal(true);
    replayDialog->setAttribute(Qt::WA_DeleteOnClose);
}

bool RecordReplayTab::checkPeriodSet()
{
    if (periodCheckBox->isChecked() && periodLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Field 'Period' must be filled");
        return false;
    }
    return true;
}

bool RecordReplayTab::checkReplayForm()
{
    if (nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Field 'Execution name' must be filled");
        return false;
    }
    return checkPeriodSet();
}

QHBoxLayout *RecordReplayTab::periodLayout(int width)
{
    periodCheckBox = new QCheckBox();
    periodLineEdit = new QLineEdit();
    periodCheckBox->setChecked(false);
    periodLineEdit->setEnabled(false);
    periodLineEdit->setFixedWidth(width);
    periodLineEdit->setValidator(new QRegExpValidator(QRegExp("[1-9][0-9]+"), this));

    QHBoxLayout *periodLay = new QHBoxLayout();
    periodLay->addWidget(new QLabel("Auto snapshot"));
    periodLay->addWidget(periodCheckBox);
    periodLay->addWidget(new QLabel("Period (sec)"));
    periodLay->addWidget(periodLineEdit);

    connect(periodCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(autoSnapshotEnabled(int)));

    return periodLay;
}

void RecordReplayTab::record_execution()
{
    QDir rrDir(vm->getPathRRDir());
    if (!rrDir.exists())
    {
        rrDir.mkdir(vm->getPathRRDir());
    }
    createDialog("New execution");
    nameEdit = new QLineEdit(replayDialog);
    nameEdit->setValidator(new QRegExpValidator(QRegExp(regExpForName), this));
    icountSpin = new QSpinBox();
    icountSpin->setMinimum(1);
    icountSpin->setMaximum(12);
    icountSpin->setValue(5);

    QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(new QLabel("Execution name"));
    topLay->addWidget(nameEdit);

    QHBoxLayout *bottomLay = new QHBoxLayout();
    bottomLay->addWidget(new QLabel("Icount value"));
    bottomLay->addWidget(icountSpin);

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addLayout(topLay);
    mainLay->addLayout(bottomLay);
    mainLay->addLayout(periodLayout(nameEdit->width() / 2));
    mainLay->addWidget(okCancelBtn);

    replayDialog->setLayout(mainLay);
    replayDialog->show();

    connect(okCancelBtn, &QDialogButtonBox::accepted,
        this, &RecordReplayTab::setRRNameDir);
    connect(okCancelBtn, &QDialogButtonBox::rejected,
        replayDialog, &QDialog::close);
}

void RecordReplayTab::replay_execution()
{
    if (executionList->currentItem())
    {
        createDialog("Auto snapshotting");

        QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel);

        QVBoxLayout *mainLay = new QVBoxLayout();
        mainLay->addLayout(periodLayout(40));
        mainLay->addWidget(okCancelBtn);

        replayDialog->setLayout(mainLay);
        
        periodLineEdit->setText(periodAutoSnap);
        replayDialog->show();

        connect(okCancelBtn, &QDialogButtonBox::accepted,
            this, &RecordReplayTab::setPeriodSnapReplay);
        connect(okCancelBtn, &QDialogButtonBox::rejected,
            replayDialog, &QDialog::close);
    }
}

void RecordReplayTab::executionListItemSelectionChanged()
{
    if (executionList->count() && isNotRunning)
    {
        setCurrentDir(executionList->currentItem()->text());
        readXml(executionList->currentItem()->text());
        if (qemuHash.compare(PlatformInformationReader::getQemuHash(globalConfig->get_current_qemu_dir())) == 0)
        {
            rpl_btn->setEnabled(true);
        }
        else
        {
            rpl_btn->setEnabled(false);
        }
        renameAct->setDisabled(false);
        deleteAct->setDisabled(false);
    }
    else
    {
        rpl_btn->setEnabled(false);
        renameAct->setDisabled(true);
        deleteAct->setDisabled(true);
    }
}

void RecordReplayTab::executionListItemRowChanged(int currentRow)
{
    if (executionList->count())
    {
        executionListItemSelectionChanged();
    }
}

void RecordReplayTab::replayCurrentQemuChanged()
{
    executionListItemSelectionChanged();
}

void RecordReplayTab::rename_ctxmenu()
{
    if (executionList->currentItem())
    {
        oldRRName = executionList->currentItem()->text();
        executionList->currentItem()->setFlags(executionList->currentItem()->flags()
            | Qt::ItemIsEditable);
        RecordRRDelegate *rrDelegate = new RecordRRDelegate();
        connect(rrDelegate, SIGNAL(renamingEnded()), this, SLOT(renameRRRecord()));
        executionList->setItemDelegateForRow(executionList->currentRow(), rrDelegate);
        executionList->editItem(executionList->currentItem());
    }
}

void RecordReplayTab::delete_ctxmenu()
{    
    if (executionList->currentItem())
    {
        int answer = QMessageBox::question(this, "Deleting", "Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            vm->remove_directory_vm(currentDirRR);
            disconnect(executionList, 0, 0, 0);
            QListWidgetItem *it = executionList->takeItem(executionList->currentRow());
            delete it;
            executionListItemSelectionChanged();
        }
    }
}

void RecordReplayTab::setPeriodSnapReplay()
{
    if (checkPeriodSet())
    {
        periodAutoSnap = periodLineEdit->text();
        replayDialog->close();
        setCurrentDir(executionList->currentItem()->text());
        emit startRR(LaunchMode::REPLAY);
    }
}

void RecordReplayTab::renameRRRecord()
{
    QListWidgetItem *item = executionList->currentItem();
    if (QString::compare(oldRRName, item->text()) != 0)
    {
        readXml(oldRRName);
        QDir dir(currentDirRR);
        if (!dir.rename(currentDirRR, vm->getPathRRDir() + "/" + item->text()))
        {
            QMessageBox::critical((QWidget *) this->parent(),
                "Error", "Record was not renamed");
            executionList->currentItem()->setText(oldRRName);
            return;
        }
        oldRRName = item->text();
        item->setFlags(item->flags() & ~Qt::ItemFlag::ItemIsEditable);
        setCurrentDir(item->text());
        createXml(currentDirRR, item->text());
    }
}

void RecordReplayTab::executionListItemClicked(QListWidgetItem *item)
{
    executionListItemSelectionChanged();
}

void RecordReplayTab::autoSnapshotEnabled(int state)
{
    periodLineEdit->setEnabled(state);
}

void RecordReplayTab::setState(bool state)
{
    isNotRunning = state;
}

void RecordReplayTab::recordDeleteRecords()
{
    clearExecutionList();
}

void RecordReplayTab::deleteRecordFolder()
{
    QString name = executionList->item(executionList->count() - 1)->text();
    vm->remove_directory_vm(currentDirRR);
    delete executionList->item(executionList->count() - 1);
    executionList->clearSelection();
    renameAct->setDisabled(true);
    deleteAct->setDisabled(true);
}

void RecordReplayTab::enableBtns(bool state)
{
    rec_btn->setEnabled(state);
    rpl_btn->setEnabled(executionList->currentItem() && state);
}

void RecordReplayTab::setRRNameDir()
{
    if (checkReplayForm())
    {
        QString name = nameEdit->text().trimmed();
        QList <QListWidgetItem*> items = executionList->findItems(name,
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
        periodAutoSnap = (periodCheckBox->isChecked()) ? periodLineEdit->text() : "";

        setCurrentDir(name);
        QListWidgetItem *it = new QListWidgetItem();
        it->setText(name);
        executionList->addItem(it);
        executionList->setCurrentItem(it);
        QDir rrDir(currentDirRR);
        if (!rrDir.exists())
        {
            rrDir.mkdir(currentDirRR);
        }
        createXml(currentDirRR, name);

        replayDialog->close();
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

