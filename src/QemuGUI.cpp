#include "QemuGUI.h"
#include "PlatformInformationReader.h"
#include "PlatformInfo.h"


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

    global_config = new GlobalConfig(this);
    vm_state = VMState::None;

    qmp_port = global_config->get_port_qmp();
    monitor_port = global_config->get_port_monitor();
    runOptions->setQmpPort(qmp_port);
    runOptions->setMonitorPort(monitor_port);
    

    //main menu
    QMenu *settings_menu = new QMenu("Settings", this);
    menuBar->addMenu(settings_menu);
    QMenu *service_menu = new QMenu("Service", this);
    menuBar->addMenu(service_menu);
    
    settings_menu->addAction("Set QEMU", this, SLOT(set_qemu_install_dir()));
    settings_menu->addAction("Connection settings", this, SLOT(launch_settings()));

    service_menu->addAction("Terminal Settings", this, SLOT(set_terminal_settings()));

    // tool menu
    qemu_install_dir_combo = new QComboBox(this);
    qemu_install_dir_combo->setMinimumWidth(150);

    qemu_play = new QAction(set_button_icon_for_state(":Resources/play.png",
        ":Resources/play_disable.png"), "Run VM", this);
    mainToolBar->addAction(qemu_play);
    connect(qemu_play, SIGNAL(triggered()), this, SLOT(play_machine()));
    qemu_play->setEnabled(false);

    qemu_pause = new QAction(set_button_icon_for_state(":Resources/pause.png",
        ":Resources/pause_disable.png"), "Pause VM", this);
    mainToolBar->addAction(qemu_pause);
    connect(qemu_pause, SIGNAL(triggered()), this, SLOT(pause_machine()));
    qemu_pause->setEnabled(false);

    qemu_stop = new QAction(set_button_icon_for_state(":Resources/stop.png",
        ":Resources/stop_disable.png"), "Stop VM", this);
    mainToolBar->addAction(qemu_stop);
    connect(qemu_stop, SIGNAL(triggered()), this, SLOT(stop_machine()));
    qemu_stop->setEnabled(false);

    QAction *runOptionsAct = new QAction(set_button_icon_for_state(":Resources/run_options.png",
        ":Resources/create_disable.png"), "Run options");
    mainToolBar->addAction(runOptionsAct);
    connect(runOptionsAct, SIGNAL(triggered()), this, SLOT(setRunOptions()));

    mainToolBar->addWidget(qemu_install_dir_combo);
    //mainToolBar->addSeparator();
    editVMAct = new QAction(set_button_icon_for_state(":Resources/settings.png",
        ":Resources/settings_disable.png"), "Edit virtial machine");
    vmToolBar->addAction(editVMAct);
    connect(editVMAct, SIGNAL(triggered()), this, SLOT(edit_settings()));
    vmToolBar->addSeparator();
    QAction *createAct = new QAction(set_button_icon_for_state(":Resources/create.png",
        ":Resources/create_disable.png"), "Create machine");
    vmToolBar->addAction(createAct);
    connect(createAct, SIGNAL(triggered()), this, SLOT(create_machine()));
    QAction *addAct = new QAction(set_button_icon_for_state(":Resources/import.png", 
        ":Resources/import_disable.png"), "Add existing machine");
    vmToolBar->addAction(addAct);
    connect(addAct, SIGNAL(triggered()), this, SLOT(add_machine()));
    
    // tab widget
    tab = new QTabWidget(centralWidget);
    tab->setMinimumWidth(400);
    tab_info = new QWidget(centralWidget);
    tab->addTab(tab_info, "Information about VM");
    rec_replay_tab = new RecordReplayTab(global_config, this);
    tab->addTab(rec_replay_tab, "Record/Replay");
    terminal_tab = new TerminalTab(global_config, this);
    tab->addTab(terminal_tab, "Terminal");
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

    create_qemu_install_dir_dialog();
    createRunOptionsDialog();
    connect_signals();
    fill_listVM_from_config();
    fill_qemu_install_dir_from_config();
    checkQemuCompatibility();

    widget_placement();
}

QemuGUI::~QemuGUI()
{
    global_config->set_current_qemu_dir(qemu_install_dir_combo->currentText());
    delete qemu_install_dir_settings;
    delete runOptionsDlg;
}


bool QemuGUI::eventFilter(QObject *obj, QEvent *event)
{
    return QMainWindow::eventFilter(obj, event);
}


void QemuGUI::fill_listVM_from_config()
{
    QList<VMConfig *> exist_vm = global_config->get_exist_vm();

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
    qemu_install_dir_list->clear();
    qemu_install_dir_combo->addItem("Add qemu...");
    QStringList qemu_list = global_config->get_qemu_installation_dirs();
    qemu_install_dir_list->addItems(qemu_list);
    for (int i = 0; i < qemu_list.count(); i++)
    {
        qemu_install_dir_combo->insertItem(qemu_install_dir_combo->count() - 1, qemu_list.at(i));
    }
    if (global_config->get_current_qemu_dir() != "")
        qemu_install_dir_combo->setCurrentText(global_config->get_current_qemu_dir());
}

void QemuGUI::checkQemuCompatibility()
{
    if (listVM->currentItem())
    {
        VMConfig *vm = global_config->get_vm_by_name(listVM->currentItem()->text());
        QString path = global_config->get_home_dir() + PlatformInformationReader::getQemuProfilePath(
            qemu_install_dir_combo->currentText()) + "/" + vm->getPlatform();
        PlatformInfo platformInfo(path);
        QStringList machines = platformInfo.getMachines();
        bool qemuIsCompatible = machines.contains(vm->getMachine());
        if (vm_state == VMState::None)
        {
            qemu_play->setEnabled(qemuIsCompatible);
            emit recordReplayEnableBtns(qemuIsCompatible);
        }
    }
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

void QemuGUI::create_qemu_install_dir_dialog()
{
    qemu_install_dir_settings = new QDialog(this);
    qemu_install_dir_settings->setWindowTitle("Qemu installation folders");
    qemu_install_dir_list = new QListWidget();
    QPushButton *add_install_dir_btn = new QPushButton("Add QEMU");
    add_install_dir_btn->setAutoDefault(true);
    QPushButton *del_install_dir_btn = new QPushButton("Delete QEMU");
    del_install_dir_btn->setAutoDefault(true);

    QHBoxLayout *buttons_lay = new QHBoxLayout();
    buttons_lay->addWidget(add_install_dir_btn);
    buttons_lay->addWidget(del_install_dir_btn);

    QVBoxLayout *layout = new QVBoxLayout();
    
    layout->addLayout(buttons_lay);
    layout->addWidget(qemu_install_dir_list);

    qemu_install_dir_settings->setLayout(layout);

    connect(add_install_dir_btn, SIGNAL(clicked()), this, SLOT(add_qemu_install_dir_btn()));
    connect(del_install_dir_btn, SIGNAL(clicked()), this, SLOT(del_qemu_install_dir_btn()));
}

void QemuGUI::createRunOptionsDialog()
{
    QStringList logOptions = { "out_asm", "in_asm", "op", "op_opt", "op_ind",
        "int", "exec", "cpu", "fpu", "mmu", "pcall", "cpu_reset", "unimp",
        "guest_error", "page", "nochain" };

    runOptionsDlg = new QDialog(this);
    runOptionsDlg->setWindowTitle("Run options");
    runOptionsDlg->setWindowIcon(QIcon(":Resources/run_options_mini.png"));
    runOptionsDlg->setModal(true);

    debugCheckBox = new QCheckBox("Debug enable");
    snapshotCheckBox = new QCheckBox("Snapshot enable");
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
    connect(okBtn, &QDialogButtonBox::accepted, this, &QemuGUI::runOptionPrepare);

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *chkLay = new QHBoxLayout();
    chkLay->addWidget(debugCheckBox);
    chkLay->addWidget(snapshotCheckBox);
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
    /* create VM */
    connect(global_config, SIGNAL(globalConfig_new_vm_is_complete()), 
        this, SLOT(refresh()));
    
    connect(qemu_install_dir_combo, SIGNAL(activated(int)), 
        this, SLOT(qemu_install_dir_combo_activated(int)));
    connect(qemu_install_dir_combo, SIGNAL(currentIndexChanged(int)), 
        this, SLOT(qemu_install_dir_combo_index_changed(int)));

    connect(this, SIGNAL(monitor_connect(int)), 
        terminal_tab, SLOT(terminalTab_connect(int)));
    connect(this, SIGNAL(monitor_abort()),
        terminal_tab, SLOT(terminalTab_abort()));

    connect(rec_replay_tab, SIGNAL(startRR(LaunchMode)), 
        this, SLOT(play_machine(LaunchMode)));
    connect(this, SIGNAL(recordReplayEnableBtns(bool)), 
        rec_replay_tab, SLOT(enableBtns(bool)));
    connect(this, SIGNAL(recordReplayStateVM(bool)),
        rec_replay_tab, SLOT(setState(bool)));
    connect(this, SIGNAL(currentQemuChanged()), 
        rec_replay_tab, SLOT(replayCurrentQemuChanged()));
}

QString QemuGUI::delete_exclude_vm(bool delete_vm)
{
    QString del_vm_name = listVM->currentItem()->text();
    global_config->delete_exclude_vm(del_vm_name, delete_vm);
    // may be return value if all ok, exclude from list
    rec_replay_tab->clearExecutionList();

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
            VMConfig *vm = global_config->get_vm_by_name(listVM->currentItem()->text());
            
            launch_qemu = new QemuLauncher(qemu_install_dir_combo->currentText(),
                vm, runOptions, launchMode, consoleTab,
                launchMode != LaunchMode::NORMAL ? rec_replay_tab : NULL);
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
                rec_replay_tab->setSnapshotPeriod("");
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

                connect(this, SIGNAL(qmp_resume_qemu()), qmp, SLOT(command_resume_qemu()));
                connect(this, SIGNAL(qmp_stop_qemu()), qmp, SLOT(command_stop_qemu()));
                connect(this, SIGNAL(qmp_shutdown_qemu()), qmp, SLOT(commandShutdownQemu()));
                connect(qmp, SIGNAL(qemu_resumed()), this, SLOT(resume_qemu_btn_state()));
                connect(qmp, SIGNAL(qemu_stopped()), this, SLOT(stop_qemu_btn_state()));

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
            emit qmp_resume_qemu();
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
    emit qmp_stop_qemu();
}

void QemuGUI::stop_machine()
{
    emit qmp_shutdown_qemu();
}

void QemuGUI::create_machine()
{
    if (qemu_install_dir_combo->count() > 1 &&
        qemu_install_dir_combo->currentIndex() != qemu_install_dir_combo->count() - 1)
    {
        createVMWindow = new CreateVMForm(global_config->get_home_dir(),
            qemu_install_dir_combo->currentText());
    }
    else
    {
        createVMWindow = new CreateVMForm(global_config->get_home_dir());
    }
    connect(createVMWindow, SIGNAL(createVM_new_vm_is_complete(VMConfig *)),
        global_config, SLOT(vm_is_created(VMConfig *)));
}

void QemuGUI::add_machine()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add exist VM",
        global_config->get_home_dir(), "*.xml");
    if (filename != "")
    {
        if (global_config->add_exist_vm(filename))
            refresh();
        else
            QMessageBox::critical(this, "Error", "Virtual machine cannot be add");
    }
}

void QemuGUI::setRunOptions()
{
    runOptionsDlg->show();
}

void QemuGUI::edit_settings()
{
    VMConfig *vm = global_config->get_vm_by_name(listVM->currentItem()->text());
    VMSettingsForm *settingsWindow = new VMSettingsForm(vm);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsWindow, SIGNAL(settingsDeleteRecords()),
        rec_replay_tab, SLOT(recordDeleteRecords()));
    connect(settingsWindow, SIGNAL(updateVMInfo()),
        this, SLOT(listVM_item_selection_changed()));
}

void QemuGUI::listVM_item_selection_changed()
{
    if (listVM->currentItem())
    {
        VMConfig *vm = global_config->get_vm_by_name(listVM->currentItem()->text());
        if (vm)
        {
            vmInfoTextEdit->setPlainText(vm->get_vm_info());
            vmInfoTextEdit->ensureCursorVisible();
            vm->fillReplayList();
            rec_replay_tab->setRecordReplayList(vm);
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

void QemuGUI::add_qemu_install_dir_btn()
{
    QString qemu_install_dir = QFileDialog::getExistingDirectory(qemu_install_dir_settings,
        "Select Qemu directory", "");
    if (qemu_install_dir != "")
    {
        if (qemu_install_dir_list->findItems(qemu_install_dir, 
            Qt::MatchFlag::MatchFixedString).size() != 0)
        {
            QMessageBox::critical(this, "Error", qemu_install_dir + " is already added");
            return;
        }

        global_config->add_qemu_installation_dir(qemu_install_dir);
        global_config->set_current_qemu_dir(qemu_install_dir);
        fill_qemu_install_dir_from_config();
        
        platformInfo = new PlatformInformationReader(qemu_install_dir,
            global_config->get_home_dir(), runOptions);
        connect(platformInfo, SIGNAL(workFinish()), this, SLOT(platformInfoReady()));
    }
}

void QemuGUI::platformInfoReady()
{
    delete platformInfo;
    qemu_install_dir_settings->close();
}

void QemuGUI::del_qemu_install_dir_btn()
{
    if (qemu_install_dir_list->currentItem())
    {
        int answer = QMessageBox::question(this, "Deleting", "Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            VMConfig::remove_directory_vm(global_config->get_home_dir()
                + PlatformInformationReader::getQemuProfilePath(qemu_install_dir_list->currentItem()->text()));
            global_config->del_qemu_installation_dir(qemu_install_dir_list->currentItem()->text());
            delete qemu_install_dir_list->currentItem();
            fill_qemu_install_dir_from_config();
        }
    }
}

void QemuGUI::refresh()
{
    listVM->clear();
    fill_listVM_from_config();
    listVM->setCurrentItem(listVM->item(listVM->count() - 1));
}

void QemuGUI::set_qemu_install_dir()
{
    qemu_install_dir_settings->show();
}


void QemuGUI::qemu_install_dir_combo_activated(int index)
{
    if (index == qemu_install_dir_combo->count() - 1)
    {
        qemu_install_dir_settings->show();
        qemu_play->setEnabled(false);
        emit recordReplayEnableBtns(false);
    }
    else
    {
        global_config->set_current_qemu_dir(qemu_install_dir_combo->itemText(index));
        checkQemuCompatibility();
        emit currentQemuChanged();
    }
}


void QemuGUI::qemu_install_dir_combo_index_changed(int index)
{

}

void QemuGUI::set_terminal_settings()
{
    TerminalSettingsForm *terminal_settings = new TerminalSettingsForm(terminal_tab->get_terminal_text());
    terminal_settings->setAttribute(Qt::WA_DeleteOnClose);

    connect(terminal_settings, SIGNAL(save_terminal_settings(QTextEdit *)), 
        terminal_tab, SLOT(save_terminal_interface_changes(QTextEdit *)));
}

void QemuGUI::launch_settings()
{
    ConnectionSettingsForm *connections_settings = new ConnectionSettingsForm(global_config);
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
    runOptions->setSnapshotEnable(snapshotCheckBox->isChecked());

    runOptions->setAdditionalCmdLine(cmdLineAdditionalLineEdit->text());

    runOptionsDlg->close();
}

void QemuGUI::set_connection_settings(const QString &qmp, const QString &monitor)
{
    monitor_port = monitor;
    qmp_port = qmp;
    runOptions->setMonitorPort(monitor_port);
    runOptions->setQmpPort(qmp_port);
}




