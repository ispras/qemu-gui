#include "QemuGUI.h"
#include "PlatformInfo.h"
#include "QemuList.h"
#include "GlobalConfig.h"
#include "QemuInstallationsForm.h"


QemuGUI::QemuGUI(QWidget *parent)
    : QMainWindow(parent)
{
    if (QemuGUI::objectName().isEmpty())
        QemuGUI::setObjectName(QStringLiteral("QemuGUI"));
    setWindowTitle(QApplication::translate("QemuGUIClass", "QemuGUI", Q_NULLPTR));
    resize(600, 400);

    runOptions = new QemuRunOptions();

    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    setMenuBar(menuBar);
    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
    mainToolBar->setIconSize(QSize(30, 30));
    addToolBar(mainToolBar);
    vmToolBar = new QToolBar(this);
    vmToolBar->setIconSize(QSize(30, 30));

    this->addToolBarBreak();
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, vmToolBar);

    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    setCentralWidget(centralWidget);
    statusBar = new QStatusBar(this);
    statusBar->setObjectName(QStringLiteral("statusBar"));
    setStatusBar(statusBar);

    vm_state = VMState::None;

    qmp_port = GlobalConfig::get_port_qmp();
    monitor_port = GlobalConfig::get_port_monitor();
    runOptions->setQmpPort(qmp_port);
    runOptions->setMonitorPort(monitor_port);


    //main menu
    QMenu *settings_menu = new QMenu("Settings", this);
    menuBar->addMenu(settings_menu);
    QMenu *service_menu = new QMenu("Service", this);
    menuBar->addMenu(service_menu);

    settings_menu->addAction("QEMU installations", [=]() { createQemuListEditor(); });
    settings_menu->addAction("Connection settings", this, SLOT(launch_settings()));

    service_menu->addAction("Terminal Settings", this, SLOT(set_terminal_settings()));

    // tool menu
    qemu_install_dir_combo = new QComboBox(this);
    qemu_install_dir_combo->setMinimumWidth(150);

    qemu_play = mainToolBar->addAction(set_button_icon_for_state(":Resources/play.png",
        ":Resources/play_disable.png"), "Run VM");
    connect(qemu_play, SIGNAL(triggered()), this, SLOT(play_machine()));
    qemu_play->setEnabled(false);

    qemu_pause = mainToolBar->addAction(set_button_icon_for_state(":Resources/pause.png",
        ":Resources/pause_disable.png"), "Pause VM");
    connect(qemu_pause, SIGNAL(triggered()), this, SLOT(pause_machine()));
    qemu_pause->setEnabled(false);

    qemu_stop = mainToolBar->addAction(set_button_icon_for_state(":Resources/stop.png",
        ":Resources/stop_disable.png"), "Stop VM");
    connect(qemu_stop, SIGNAL(triggered()), this, SLOT(stop_machine()));
    qemu_stop->setEnabled(false);

    QAction *runOptionsAct = mainToolBar->addAction(set_button_icon_for_state(":Resources/run_options.png",
        ":Resources/create_disable.png"), "Run options");
    connect(runOptionsAct, SIGNAL(triggered()), this, SLOT(setRunOptions()));

    mainToolBar->addWidget(qemu_install_dir_combo);
    //mainToolBar->addSeparator();
    editVMAct = vmToolBar->addAction(set_button_icon_for_state(":Resources/settings.png",
        ":Resources/settings_disable.png"), "Edit virtial machine");
    connect(editVMAct, SIGNAL(triggered()), this, SLOT(edit_settings()));
    vmToolBar->addSeparator();
    QAction *createAct = vmToolBar->addAction(set_button_icon_for_state(":Resources/create.png",
        ":Resources/create_disable.png"), "Create machine");
    connect(createAct, SIGNAL(triggered()), this, SLOT(create_machine()));
    QAction *addAct = vmToolBar->addAction(set_button_icon_for_state(":Resources/import.png",
        ":Resources/import_disable.png"), "Add existing machine");
    connect(addAct, SIGNAL(triggered()), this, SLOT(add_machine()));

    // tab widget
    tab = new QTabWidget(centralWidget);
    tab->setMinimumWidth(400);
    tab_info = new QWidget(centralWidget);
    tab->addTab(tab_info, "Information about VM");
    rec_replay_tab = new RecordReplayTab(this);
    tab->addTab(rec_replay_tab, "Record/Replay");
    terminal_tab = new TerminalTab(this);
    terminal_tab->setObjectName("Terminal");
    tab->addTab(terminal_tab, terminal_tab->objectName());
    consoleTab = new ConsoleTab();
    tab->addTab(consoleTab, "Console");

    // info tab
    propBox = new QGroupBox(tab_info);
    vmInfoTextEdit = new QTextEdit(propBox);
    vmInfoTextEdit->setReadOnly(true);

    vmInfoTextEdit->setStyleSheet("background-color: white; color: darkblue; border: 1px; height: 14px;");

    propBox->setMinimumWidth(300);
    propBox->setVisible(false);

    listVM = new QListWidget();
    listVM->setMaximumWidth(500);
    listVM->setUniformItemSizes(true);
    QFont listVMfont;
    listVMfont.setPointSize(10);
    listVM->setFont(listVMfont);
    listVM->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);

    delete_act = new QAction("Delete VM", listVM);
    exclude_act = new QAction("Exclude VM", listVM);
    listVM->addAction(exclude_act);
    listVM->addAction(delete_act);
    listVM->setContextMenuPolicy(Qt::ActionsContextMenu);

    createRunOptionsDialog();
    connect_signals();
    fill_listVM_from_config();
    fill_qemu_install_dir_from_config();
    checkQemuCompatibility();

    widget_placement();

    QRect desktopRect = frameGeometry();
    desktopRect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(desktopRect.topLeft());
}

QemuGUI::~QemuGUI()
{
    delete runOptionsDlg;
}


bool QemuGUI::eventFilter(QObject *obj, QEvent *event)
{
    return QMainWindow::eventFilter(obj, event);
}


void QemuGUI::fill_listVM_from_config()
{
    QList<VMConfig *> exist_vm = GlobalConfig::get_exist_vm();

    foreach(VMConfig *vm, exist_vm)
    {
        listVM->addItem(vm->get_name());
        listVM->item(listVM->count() - 1)->setSizeHint(QSize(0, 20));
    }
    listVM->setCurrentRow(0);
    listVM->setFocus();
    if (listVM->count())
    {
        qemu_play->setEnabled(true);
    }
    else
    {
        editVMAct->setVisible(false);
    }
}

void QemuGUI::widget_placement()
{
    QVBoxLayout *infoGroupLay = new QVBoxLayout(propBox);
    infoGroupLay->addWidget(vmInfoTextEdit);

    QVBoxLayout *infoLay = new QVBoxLayout(tab_info);
    infoLay->addWidget(propBox);

    QHBoxLayout *one = new QHBoxLayout(centralWidget);

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(listVM);
    splitter->addWidget(tab);

    one->addWidget(splitter);
}

void QemuGUI::fill_qemu_install_dir_from_config()
{
    qemu_install_dir_combo->clear();
    QemuList::Items qemu_list = QemuList::getAllQemuInstallations();
    qemu_install_dir_combo->addItems(qemu_list.keys());
    qemu_install_dir_combo->addItem("Edit qemu list...");
    if (GlobalConfig::get_current_qemu() != "")
    {
        qemu_install_dir_combo->setCurrentText(GlobalConfig::get_current_qemu());
    }
    else if (qemu_install_dir_combo->count() != 1)
    {
        GlobalConfig::set_current_qemu(qemu_install_dir_combo->currentText());
    }
}

void QemuGUI::checkQemuCompatibility()
{
    if (listVM->currentItem())
    {
        VMConfig *vm = GlobalConfig::get_vm_by_name(listVM->currentItem()->text());
        QString path = QemuList::getQemuProfilePath(GlobalConfig::get_current_qemu())
            + "/" + vm->getPlatform();
        PlatformInfo platformInfo(path);
        QStringList machines = platformInfo.getMachines();
        bool qemuIsCompatible = machines.contains(vm->getMachine());
        if (vm_state == VMState::None)
        {
            qemu_play->setEnabled(qemuIsCompatible);
            editVMAct->setEnabled(qemuIsCompatible);
            emit recordReplayEnableBtns(qemuIsCompatible);
        }
    }
}

void QemuGUI::setWindowGeometry(QWidget *window, QWidget *parent)
{
    int x = parent->geometry().center().x() - window->width() / 2;
    int y = parent->geometry().center().y() - window->height() / 2;

    window->setGeometry(x, y, window->width(), window->height());
}

void QemuGUI::setButtonsState(bool runningState)
{
    qemu_play->setEnabled(!runningState);
    qemu_pause->setEnabled(launchMode != LaunchMode::RECORD && runningState);
    qemu_stop->setEnabled(true);
    if (!runningState)
    {
        vm_state = VMState::Stopped;
    }
}

void QemuGUI::stop_qemu_btn_state()
{
    vm_state = VMState::Stopped;
    qemu_play->setEnabled(true);
    qemu_pause->setEnabled(false);
}

void QemuGUI::resume_qemu_btn_state()
{
    vm_state = VMState::Running;
    qemu_play->setEnabled(false);
    qemu_pause->setEnabled(launchMode != LaunchMode::RECORD && true);
}

QIcon QemuGUI::set_button_icon_for_state(const QString &normal_icon,
    const QString &disable_icon)
{
    QIcon icon;
    QPixmap pix_normal, pix_disable;
    pix_normal.load(normal_icon);
    pix_disable.load(disable_icon);
    icon.addPixmap(pix_normal, QIcon::Normal, QIcon::On);
    icon.addPixmap(pix_disable, QIcon::Disabled, QIcon::On);
    return icon;
}

void QemuGUI::createQemuListEditor()
{
    QDialog *form = new QemuInstallationsForm(this);
    connect(form, &QDialog::finished,
        [=](int result) { fill_qemu_install_dir_from_config(); });

    form->open();
}

void QemuGUI::createRunOptionsDialog()
{
    QStringList logOptions = { "out_asm", "in_asm", "op", "op_opt", "op_ind",
        "int", "exec", "cpu", "fpu", "mmu", "pcall", "cpu_reset", "unimp",
        "guest_error", "page", "nochain" };

    runOptionsDlg = new QDialog();
    runOptionsDlg->setWindowTitle("Run options");
    runOptionsDlg->setWindowIcon(QIcon(":Resources/run_options_mini.png"));
    runOptionsDlg->setModal(true);

    debugCheckBox = new QCheckBox("Debug enable");
    snapshotCheckBox = new QCheckBox("Snapshot enable");
    qemuStoppedCheckBox = new QCheckBox("Stopped Qemu");
    cmdLineAdditionalLineEdit = new QLineEdit();
    logfileNameLineEdit = new QLineEdit();

    foreach(QString op, logOptions)
    {
        QCheckBox *check = new QCheckBox(op);
        check->setObjectName(op);
        logCheckBox.append(check);
    }

    QGroupBox *logGroup = new QGroupBox("Logging options");
    QGroupBox *commonGroup = new QGroupBox("Common options");

    QDialogButtonBox *okBtn = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(okBtn, &QDialogButtonBox::accepted, runOptionsDlg, &QDialog::close);

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *chkLay = new QHBoxLayout();
    chkLay->addWidget(debugCheckBox);
    chkLay->addWidget(snapshotCheckBox);
    chkLay->addWidget(qemuStoppedCheckBox);
    QHBoxLayout *cmdLay = new QHBoxLayout();
    cmdLay->addWidget(new QLabel("Command line options"));
    cmdLay->addWidget(cmdLineAdditionalLineEdit);

    QVBoxLayout *logLay = new QVBoxLayout();
    QHBoxLayout *logFileLay = new QHBoxLayout();
    logFileLay->addWidget(new QLabel("Name of logfile"));
    logFileLay->addWidget(logfileNameLineEdit);
    QGridLayout *logOpLay = new QGridLayout();
    for (int k = 0; k < logCheckBox.count(); k++)
    {
        logOpLay->addWidget(logCheckBox.at(k), k / 4, k % 4);
    }

    logLay->addLayout(logFileLay);
    logLay->addLayout(logOpLay);

    commonGroup->setLayout(chkLay);
    logGroup->setLayout(logLay);

    mainLay->addWidget(commonGroup);
    mainLay->addWidget(logGroup);
    mainLay->addLayout(cmdLay);
    mainLay->addWidget(okBtn);

    runOptionsDlg->setLayout(mainLay);
}

void QemuGUI::connect_signals()
{
    /* list of machines */
    connect(listVM, SIGNAL(itemSelectionChanged()),
        this, SLOT(listVM_item_selection_changed()));
    connect(listVM, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
        this, SLOT(listVM_current_item_changed(QListWidgetItem *, QListWidgetItem *)));
    /* delete VM */
    connect(delete_act, SIGNAL(triggered()), this, SLOT(delete_vm_ctxmenu()));
    /* exclude VM */
    connect(exclude_act, SIGNAL(triggered()), this, SLOT(exclude_vm_ctxmenu()));

    connect(qemu_install_dir_combo, SIGNAL(activated(int)),
        this, SLOT(qemu_install_dir_combo_activated(int)));

    connect(this, SIGNAL(monitor_connect(int)),
        terminal_tab, SLOT(terminalTab_connect(int)));
    connect(this, SIGNAL(monitor_abort()),
        terminal_tab, SLOT(terminalTab_abort()));

    connect(tab, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    connect(rec_replay_tab, SIGNAL(startRR(LaunchMode)),
        this, SLOT(play_machine(LaunchMode)));
    connect(this, SIGNAL(recordReplayEnableBtns(bool)),
        rec_replay_tab, SLOT(enableBtns(bool)));
    connect(this, SIGNAL(recordReplayStateVM(bool)),
        rec_replay_tab, SLOT(setState(bool)));
    connect(this, SIGNAL(currentQemuChanged()),
        rec_replay_tab, SLOT(replayCurrentQemuChanged()));
}

void QemuGUI::currentTabChanged(int index)
{
    if (tab->tabText(index) == terminal_tab->objectName())
    {
        terminal_tab->setCommmandLineFocus();
    }
}

QString QemuGUI::delete_exclude_vm(bool delete_vm)
{
    QString del_vm_name = listVM->currentItem()->text();
    GlobalConfig::delete_exclude_vm(del_vm_name, delete_vm);
    // may be return value if all ok, exclude from list
    rec_replay_tab->clearVM();

    delete listVM->currentItem();

    if (listVM->count() != 0)
    {
        listVM->setCurrentRow(0);
    }
    else
    {
        emit recordReplayEnableBtns(false);
        qemu_play->setEnabled(false);
    }
    return del_vm_name;
}

void QemuGUI::delete_vm_ctxmenu()
{
    int answer = QMessageBox::question(this, "Deleting", "Are you sure?",
        QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        QString del_vm_name = delete_exclude_vm(true);
        QMessageBox::information(this, "Success",
            "Virtual machine " + del_vm_name + " was deleted");
    }
}

void QemuGUI::exclude_vm_ctxmenu()
{
    int answer = QMessageBox::question(this, "Excluding", "Are you sure?",
        QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        QString del_vm_name = delete_exclude_vm(false);
        QMessageBox::information(this, "Success",
            "Virtual machine " + del_vm_name + " was excluded");
    }
}

void QemuGUI::play_machine()
{
    if (listVM->currentItem())
    {
        if (vm_state == VMState::None)
        {
            VMConfig *vm = GlobalConfig::get_vm_by_name(listVM->currentItem()->text());
            runOptionPrepare();
            listVM->setDisabled(true);

            launch_qemu = new QemuLauncher(GlobalConfig::get_current_qemu_dir(),
                vm, *runOptions, launchMode, rec_replay_tab->getRecordReplayParams());
            connect(launch_qemu, &QemuLauncher::qemuStarted,
                [=](const QString &cmdLine) { consoleTab->addConsoleText(cmdLine); }
            );
            if (launch_qemu->isQemuExist())
            {
                vmToolBar->setEnabled(false);
                if (debugCheckBox->isChecked())
                {
                    vm_state = VMState::Stopped;
                }
                else
                {
                    vm_state = VMState::Running;
                }
                qemu_play->setEnabled(false);
                qemu_pause->setEnabled(false);
                qemu_stop->setEnabled(false);
                emit recordReplayEnableBtns(false);
                emit recordReplayStateVM(false);

                QThread *thread = new QThread();
                launch_qemu->moveToThread(thread);
                connect(thread, SIGNAL(started()), launch_qemu, SLOT(start_qemu()));
                connect(launch_qemu, SIGNAL(qemu_laucher_finished(int)),
                    this, SLOT(finish_qemu(int)));
                thread->start();

                qmp = new QMPInteraction(nullptr, qmp_port.toInt());
                connect(qmp, SIGNAL(qemuRunningStatus(bool)), this, SLOT(setButtonsState(bool)));

                connect(this, SIGNAL(qmpSendCommand(QMPCommands)),
                    qmp, SLOT(commandQmp(QMPCommands)));

                connect(qmp, SIGNAL(qemuResumed()), this, SLOT(resume_qemu_btn_state()));
                connect(qmp, SIGNAL(qemuStopped()), this, SLOT(stop_qemu_btn_state()));

                connect(launch_qemu, SIGNAL(creatingOverlayFailed()), this, SLOT(overlayFailed()));

                emit monitor_connect(monitor_port.toInt());
            }
            else
            {
                delete launch_qemu;
                QMessageBox::critical(this, "Error", "QEMU for selected platform doesn't exist");
            }
        }
        else if (vm_state == VMState::Stopped)
        {
            emit qmpSendCommand(QMPCommands::Continue);
        }
    }
}

void QemuGUI::play_machine(LaunchMode mode)
{
    launchMode = mode;
    play_machine();
}

void QemuGUI::finish_qemu(int exitCode)
{
    if (exitCode != 0)
        emit monitor_abort();
    listVM->setDisabled(false);
    vmToolBar->setEnabled(true);
    vm_state = VMState::None;
    launchMode = LaunchMode::NORMAL;
    qemu_play->setEnabled(true);
    qemu_stop->setEnabled(false);
    qemu_pause->setEnabled(false);
    delete launch_qemu;
    launch_qemu = NULL;
    delete qmp;
    qmp = NULL;
    emit recordReplayEnableBtns(true);
    emit recordReplayStateVM(true);
}

void QemuGUI::pause_machine()
{
    emit qmpSendCommand(QMPCommands::Stop);
}

void QemuGUI::stop_machine()
{
    emit qmpSendCommand(QMPCommands::Quit);
}

void QemuGUI::create_machine()
{
    if (qemu_install_dir_combo->currentIndex() != qemu_install_dir_combo->count() - 1)
    {
        createVMWindow = new CreateVMForm();
        setWindowGeometry(createVMWindow, this);
        connect(createVMWindow, &CreateVMForm::createVM_new_vm_is_complete,
            [=](VMConfig *vm)
            {
                GlobalConfig::vm_is_created(vm);
                refresh();
            }
        );
    }
    else
    {
        QMessageBox::warning(this, "No selected Qemu",
            "You have to add Qemu or select existing one");
    }
}

void QemuGUI::add_machine()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add exist VM",
        GlobalConfig::get_home_dir(), "*.xml");
    if (filename != "")
    {
        if (GlobalConfig::add_exist_vm(filename))
            refresh();
        else
            QMessageBox::critical(this, "Error", "Virtual machine cannot be add");
    }
}

void QemuGUI::setRunOptions()
{
    runOptionsDlg->show();
    setWindowGeometry(runOptionsDlg, this);
}

void QemuGUI::edit_settings()
{
    VMConfig *vm = GlobalConfig::get_vm_by_name(listVM->currentItem()->text());
    VMSettingsForm *settingsWindow = new VMSettingsForm(vm,
        qemu_install_dir_combo->currentText());
    setWindowGeometry(settingsWindow, this);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsWindow, SIGNAL(settingsDeleteRecords()),
        rec_replay_tab, SLOT(recordDeleteRecords()));
    connect(settingsWindow, SIGNAL(isExecutionListNotEmpty()),
        rec_replay_tab, SLOT(isExecutionsExist()));
    connect(settingsWindow, SIGNAL(updateVMInfo()),
        this, SLOT(listVM_item_selection_changed()));
}

void QemuGUI::listVM_item_selection_changed()
{
    if (listVM->currentItem())
    {
        VMConfig *vm = GlobalConfig::get_vm_by_name(listVM->currentItem()->text());
        if (vm)
        {
            vmInfoTextEdit->setPlainText(vm->get_vm_info());
            vmInfoTextEdit->ensureCursorVisible();
            rec_replay_tab->setVM(vm);
        }
        checkQemuCompatibility();
        propBox->setVisible(true);
        delete_act->setDisabled(false);
        exclude_act->setDisabled(false);
        editVMAct->setVisible(true);
    }
    else
    {
        propBox->setVisible(false);
        delete_act->setDisabled(true);
        exclude_act->setDisabled(true);
        editVMAct->setVisible(false);
    }
}

void QemuGUI::listVM_current_item_changed(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current)
    {
        QFont font = listVM->font();

        if (previous)
        {
            previous->setTextColor(Qt::GlobalColor::black);
            previous->setFont(font);
        }

        font.setBold(true);
        current->setTextColor(Qt::GlobalColor::darkBlue);
        current->setFont(font);
    }
}

void QemuGUI::refresh()
{
    listVM->clear();
    fill_listVM_from_config();
    listVM->setCurrentItem(listVM->item(listVM->count() - 1));
}

// void QemuGUI::set_qemu_install_dir()
// {
//     qemu_install_dir_settings->show();
//     setWindowGeometry(qemu_install_dir_settings, this);
// }


void QemuGUI::qemu_install_dir_combo_activated(int index)
{
    if (index == qemu_install_dir_combo->count() - 1)
    {
        createQemuListEditor();
        qemu_play->setEnabled(false);
        editVMAct->setEnabled(false);
        emit recordReplayEnableBtns(false);
    }
    else
    {
        GlobalConfig::set_current_qemu(qemu_install_dir_combo->itemText(index));
        checkQemuCompatibility();
        emit currentQemuChanged();
    }
}


void QemuGUI::set_terminal_settings()
{
    TerminalSettingsForm *terminal_settings = new TerminalSettingsForm(terminal_tab->getTerminalText());
    setWindowGeometry(terminal_settings, this);
    terminal_settings->setAttribute(Qt::WA_DeleteOnClose);

    connect(terminal_settings, SIGNAL(save_terminal_settings(QTextEdit *)),
        terminal_tab, SLOT(save_terminal_interface_changes(QTextEdit *)));
}

void QemuGUI::launch_settings()
{
    ConnectionSettingsForm *connections_settings = new ConnectionSettingsForm();
    setWindowGeometry(connections_settings, this);
    connections_settings->setAttribute(Qt::WA_DeleteOnClose);

    connect(connections_settings, SIGNAL(done_connection_settings(QString, QString)),
        this, SLOT(set_connection_settings(QString, QString)));
}

void QemuGUI::overlayFailed()
{
    QMessageBox::critical(this, "Error", "Overlay is not created");
    connect(this, SIGNAL(deleteRecord()), rec_replay_tab, SLOT(deleteRecordFolder()));
    emit deleteRecord();
}

void QemuGUI::runOptionPrepare()
{
    logFileName = logfileNameLineEdit->text();
    logOptions.clear();
    foreach(QCheckBox *op, logCheckBox)
    {
        if (op->isChecked())
        {
            logOptions.append(op->objectName());
        }
    }
    runOptions->setLogfileName(logfileNameLineEdit->text());
    runOptions->setLogOptionList(logOptions);
    runOptions->setDebugEnable(debugCheckBox->isChecked());
    runOptions->setQemuRunStopped(qemuStoppedCheckBox->isChecked());
    runOptions->setSnapshotEnable(snapshotCheckBox->isChecked());
    runOptions->setAdditionalCmdLine(cmdLineAdditionalLineEdit->text());
}

void QemuGUI::set_connection_settings(const QString &qmp, const QString &monitor)
{
    monitor_port = monitor;
    qmp_port = qmp;
    runOptions->setMonitorPort(monitor_port);
    runOptions->setQmpPort(qmp_port);
}




