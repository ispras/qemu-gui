#include "RecordReplayTab.h"
#include "PlatformInformationReader.h"
#include "QemuGUI.h"
#include "QemuImgLauncher.h"
#include "CommandLineParameters.h"
#include "common/FileHelpers.h"

static const char regExpForName[] = "[A-Za-z0-9_-][A-Za-z0-9_-\\s]+";

RecordReplayTab::RecordReplayTab(QWidget *parent)
    : QWidget(parent)
{
    if (RecordReplayTab::objectName().isEmpty())
        RecordReplayTab::setObjectName(QStringLiteral("RecordReplayTab"));
    resize(400, 300);
    
    executionList = new QListWidget();
    rec_btn = new QPushButton("Start record");
    rpl_btn = new QPushButton("Start replay");
    rec_btn->setEnabled(false);
    rpl_btn->setEnabled(false);
    rec_btn->setIcon(QIcon(":Resources/play.png"));
    rpl_btn->setIcon(QIcon(":Resources/replay.png"));

    renameAct = new QAction("Rename", executionList);
    deleteAct = new QAction("Delete", executionList);
    deleteAllAct = new QAction("Delete all", executionList);
    renameAct->setDisabled(true);
    deleteAct->setDisabled(true);
    deleteAllAct->setDisabled(true);

    executionList->setEditTriggers(QAbstractItemView::AnyKeyPressed | 
        QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
 
    executionList->addAction(renameAct);
    executionList->addAction(deleteAct);
    executionList->addAction(deleteAllAct);
    executionList->setContextMenuPolicy(Qt::ActionsContextMenu);

    rec_btn->setAutoDefault(true);
    rpl_btn->setAutoDefault(true);

    widget_placement();
    connect_signals();

    isNotRunning = true;
    oldRRName = "";

    pWidget = parent;
}

RecordReplayTab::~RecordReplayTab()
{

}

void RecordReplayTab::setVM(VMConfig *virtualMachine)
{
    disconnect(executionList, 0, 0, 0);

    vm = virtualMachine;
    rec_btn->setEnabled(true);
    executionList->clear();

    executionListConnectSignals();

    executionList->addItems(vm->getReplayList());
    if (executionList->count())
    {
        executionList->setCurrentRow(0);
    }
    else
    {
        rpl_btn->setEnabled(false);
    }
}

void RecordReplayTab::clearVM()
{
    vm = NULL;
    recordDeleteRecords();
}

void RecordReplayTab::connect_signals()
{
    connect(rec_btn, SIGNAL(clicked()), this, SLOT(record_execution()));
    connect(rpl_btn, SIGNAL(clicked()), this, SLOT(replay_execution()));

    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename_ctxmenu()));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(delete_ctxmenu()));
    connect(deleteAllAct, SIGNAL(triggered()), this, SLOT(deleteAllCtxmenu()));
}

void RecordReplayTab::executionListConnectSignals()
{
    connect(executionList, SIGNAL(currentRowChanged(int)),
        this, SLOT(executionListItemRowChanged(int)));
    connect(executionList, SIGNAL(itemClicked(QListWidgetItem *)),
        this, SLOT(executionListItemClicked(QListWidgetItem *)));
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

void RecordReplayTab::createXml()
{
    rrParams.setQemu(GlobalConfig::get_current_qemu());
    rrParams.createXml();
}

void RecordReplayTab::setCurrentDir(const QString &name)
{
    rrParams.setCurrentDir(vm->getPathRRDir() + "/" + name);
}

void RecordReplayTab::createDialog(const QString &caption)
{
    replayDialog = new QDialog();
    replayDialog->setWindowTitle(caption);
    replayDialog->setWindowIcon(QIcon(":Resources/settings.png"));
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

QHBoxLayout *RecordReplayTab::overlayLayout()
{
    overlayCheck = new QCheckBox();
    overlayCheck->setChecked(true);
    
    QHBoxLayout *overlayLay = new QHBoxLayout();
    overlayLay->addWidget(new QLabel("Create overlay and RR snapshot"));
    overlayLay->addWidget(overlayCheck);

    connect(overlayCheck, &QCheckBox::stateChanged,
        [=](bool state) { rrParams.setOverlayEnabled(state); });

    return overlayLay;
}

QStringList RecordReplayTab::getSnapshotInfo()
{
    QemuImgLauncher lauch(GlobalConfig::get_current_qemu_dir(), "",
        rrParams.getDummyImage());
    QStringList info = lauch.getSnapshotInformation();
    QStringList snapshotNames;
    snapshotTips.clear();
    foreach(QString str, info)
    {
        QStringList line = str.split(' ', QString::SkipEmptyParts);
        if (line.first().toInt())
        {
            if (line.at(2).compare("0"))
            {
                QString tip = " Size: " + line.at(2) + " Date: " + line.at(3)
                    + " Time: " + line.at(4);
                snapshotTips.append(tip);
                snapshotNames.append(line.at(1));
            }
        }
    }
    return snapshotNames;
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
    icountSpin->setValue(rrParams.getIcount());

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
    mainLay->addLayout(overlayLayout());
    mainLay->addLayout(periodLayout(nameEdit->width() / 2));
    mainLay->addWidget(okCancelBtn);

    replayDialog->setLayout(mainLay);
    replayDialog->show();
    QemuGUI::setWindowGeometry(replayDialog, pWidget);

    connect(okCancelBtn, &QDialogButtonBox::accepted,
        this, &RecordReplayTab::setRRNameDir);
    connect(okCancelBtn, &QDialogButtonBox::rejected,
        replayDialog, &QDialog::close);
    connect(icountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        [=](int value) { this->rrParams.setIcount(value); });
    connect(overlayCheck, SIGNAL(stateChanged(int)), this, SLOT(setAutoSnapshotEnabled(int)));
}

void RecordReplayTab::replay_execution()
{
    if (executionList->currentItem())
    {
        if (!rrParams.isOverlayEnabled())
        {
            setCurrentDir(executionList->currentItem()->text());
            emit startRR(LaunchMode::REPLAY);
        }
        else
        {
            createDialog("Auto snapshotting");
            QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
                | QDialogButtonBox::Cancel);
            
            snapshotCombo = new QComboBox();
            snapshotCombo->addItems(getSnapshotInfo());
            snapshotCombo->setFixedWidth(130);
            for (int i = 0; i < snapshotCombo->count(); i++)
            {
                snapshotCombo->setItemData(i, snapshotTips.at(i), Qt::ToolTipRole);
            }
            rrParams.setInitialSnapshot(snapshotCombo->currentText());
            
            QHBoxLayout *snapshotLay = new QHBoxLayout();
            snapshotLay->addWidget(new QLabel("Init snapshot"));
            snapshotLay->addWidget(snapshotCombo);

            QVBoxLayout *mainLay = new QVBoxLayout();
            mainLay->addLayout(snapshotLay);
            mainLay->addLayout(periodLayout(40));
            mainLay->addWidget(okCancelBtn);

            replayDialog->setLayout(mainLay);
            replayDialog->show();
            QemuGUI::setWindowGeometry(replayDialog, pWidget);

            connect(okCancelBtn, &QDialogButtonBox::accepted,
                this, &RecordReplayTab::setPeriodSnapReplay);
            connect(okCancelBtn, &QDialogButtonBox::rejected,
                replayDialog, &QDialog::close);
            connect(snapshotCombo, QOverload<int>::of(&QComboBox::activated),
                [=](int index)
                {
                    rrParams.setInitialSnapshot(snapshotCombo->itemText(index));
                    periodCheckBox->setDisabled(index != 0);
                    if (index != 0)
                    {
                        periodCheckBox->setCheckState(Qt::Unchecked);
                    }
                }
            );
        }
    }
}

void RecordReplayTab::executionListItemSelectionChanged()
{
    if (executionList->count() && isNotRunning)
    {
        setCurrentDir(executionList->currentItem()->text());
        rrParams = vm->getRRParams(executionList->currentItem()->text());
        if (rrParams.getQemu() == GlobalConfig::get_current_qemu())
        {
            rpl_btn->setEnabled(true);
        }
        else
        {
            rpl_btn->setEnabled(false);
        }
        renameAct->setDisabled(false);
        deleteAct->setDisabled(false);
        deleteAllAct->setDisabled(false);
    }
    else
    {
        rpl_btn->setEnabled(false);
        renameAct->setDisabled(true);
        deleteAct->setDisabled(true);
        deleteAllAct->setDisabled(true);
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

bool RecordReplayTab::isExecutionsExist()
{
    return executionList->count();
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
            FileHelpers::deleteDirectory(rrParams.getCurrentDir());
            disconnect(executionList, 0, 0, 0);
            QListWidgetItem *it = executionList->takeItem(executionList->currentRow());
            delete it;

            executionListConnectSignals();
            executionListItemSelectionChanged();
        }
    }
}

void RecordReplayTab::deleteAllCtxmenu()
{
    if (executionList->currentItem())
    {
        int answer = QMessageBox::question(this, "Deleting all executions", "Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            for (int i = 0; i < executionList->count(); i++)
            {
                QString name = vm->getPathRRDir() + "/" + executionList->item(i)->text();
                FileHelpers::deleteDirectory(name);
            }
            disconnect(executionList, 0, 0, 0);
            executionList->clear();
            
            executionListConnectSignals();
            executionListItemSelectionChanged();
        }
    }
}

void RecordReplayTab::setPeriodSnapReplay()
{
    if (checkPeriodSet())
    {
        rrParams.setSnapshotPeriod(periodCheckBox->isChecked()
            ? periodLineEdit->text().toInt() : 0);
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
        rrParams = vm->getRRParams(oldRRName);
        QDir dir(vm->getPathRRDir());
        if (!dir.rename(oldRRName, item->text()))
        {
            QMessageBox::critical((QWidget *) this->parent(),
                "Error", "Record was not renamed");
            executionList->currentItem()->setText(oldRRName);
            return;
        }
        oldRRName = item->text();
        item->setFlags(item->flags() & ~Qt::ItemFlag::ItemIsEditable);
        setCurrentDir(item->text());
        createXml();
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

void RecordReplayTab::setAutoSnapshotEnabled(int value)
{
    periodLineEdit->setEnabled(periodCheckBox->isChecked() && value);
    periodCheckBox->setEnabled(value);
}

void RecordReplayTab::setState(bool state)
{
    isNotRunning = state;
}

void RecordReplayTab::recordDeleteRecords()
{
    disconnect(executionList, 0, 0, 0);
    executionList->clear();
}

void RecordReplayTab::deleteRecordFolder()
{
    FileHelpers::deleteDirectory(rrParams.getCurrentDir());
    delete executionList->item(executionList->count() - 1);
    executionList->clearSelection();
    renameAct->setDisabled(true);
    deleteAct->setDisabled(true);
    deleteAllAct->setDisabled(true);
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
        rrParams.setIcount(icountSpin->value());
        rrParams.setSnapshotPeriod(periodCheckBox->isChecked()
            ? periodLineEdit->text().toInt() : 0);
        rrParams.setOverlayEnabled(overlayCheck->isChecked());

        setCurrentDir(name);
        QListWidgetItem *it = new QListWidgetItem();
        it->setText(name);
        executionList->addItem(it);
        executionList->setCurrentItem(it);
        QDir rrDir(rrParams.getCurrentDir());
        if (!rrDir.exists())
        {
            rrDir.mkdir(rrParams.getCurrentDir());
        }
        createXml();

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

