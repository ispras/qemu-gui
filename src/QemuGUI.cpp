#include "QemuGUI.h"


QemuGUI::QemuGUI(QWidget *parent)
    : QMainWindow(parent)
{
    if (QemuGUI::objectName().isEmpty())
        QemuGUI::setObjectName(QStringLiteral("QemuGUI"));
    setWindowTitle(QApplication::translate("QemuGUIClass", "QemuGUI", Q_NULLPTR));
    resize(600, 400);
    menuBar = new QMenuBar(this);
    menuBar->setObjectName(QStringLiteral("menuBar"));
    setMenuBar(menuBar);
    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
    addToolBar(mainToolBar);
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    setCentralWidget(centralWidget);
    statusBar = new QStatusBar(this);
    statusBar->setObjectName(QStringLiteral("statusBar"));
    setStatusBar(statusBar);

    global_config = new GlobalConfig();

    //main menu
    menuBar->addMenu("What");
    menuBar->addMenu("do");
    menuBar->addMenu("you");
    menuBar->addMenu("want?");

    // tool menu
    mainToolBar->addAction(QIcon(":Resources/play.png"), "Play VM", this, SLOT(play_machine()));
    mainToolBar->addAction(QIcon(":Resources/pause.png"), "Pause VM", this, SLOT(pause_machine()));
    mainToolBar->addAction(QIcon(":Resources/stop.png"), "Stop VM", this, SLOT(stop_machine()));
    mainToolBar->addSeparator();
    mainToolBar->addAction("Create machine", this, SLOT(create_machine()));
    mainToolBar->addAction("Add exist machine", this, SLOT(add_machine()));

    // tab	
    tab = new QTabWidget(centralWidget);
    tab->setMinimumWidth(400);
    tab_info = new QWidget(centralWidget);
    tab->addTab(tab_info, "Information about VM");

    propBox = new QGroupBox(tab_info);
    edit_btn = new QPushButton("Edit VM", tab_info);
    info_lbl = new QLabel("", propBox);

    propBox->setMinimumWidth(300);
    propBox->setVisible(false);
    edit_btn->setVisible(false);

    listVM = new QListWidget();
    QFont listVMfont;
    listVMfont.setPointSize(10);
    listVM->setFont(listVMfont);

    delete_act = new QAction("Delete VM", listVM);
    exclude_act = new QAction("Exclude VM", listVM);
    listVM->addAction(exclude_act);
    listVM->addAction(delete_act);
    listVM->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    connect_signals();
    fill_listVM_from_config();

    recReplayTab = new RecordReplayTab();
    tab->addTab(recReplayTab, "Record/Replay");

    widget_placement();
}

QemuGUI::~QemuGUI()
{

}


void QemuGUI::fill_listVM_from_config()
{
    listVM->setMaximumWidth(300);
    listVM->setUniformItemSizes(true);

    QStringList exist_vm = global_config->get_exist_vm();
    if (!exist_vm.isEmpty())
    {
        listVM->addItems(exist_vm);
        listVM->setCurrentRow(0);
    }

    for (int i = 0; i < listVM->count(); i++)
    {
        listVM->item(i)->setSizeHint(QSize(0, 20));
    }
    listVM->setFocus();
}

void QemuGUI::widget_placement()
{
    QVBoxLayout *infoGroupLay = new QVBoxLayout(propBox);
    infoGroupLay->addWidget(info_lbl);
    infoGroupLay->addStretch(100);

    QVBoxLayout *infoLay = new QVBoxLayout(tab_info);
    infoLay->addWidget(propBox);
    infoLay->addWidget(edit_btn);

    QHBoxLayout *one = new QHBoxLayout(centralWidget);

    QVBoxLayout *lay = new QVBoxLayout(centralWidget);
    lay->addWidget(listVM);

    QVBoxLayout *lay2 = new QVBoxLayout(centralWidget);
    lay2->addWidget(tab);

    one->addLayout(lay);
    one->addLayout(lay2);
}

void QemuGUI::connect_signals()
{
    /* edit machine */
    connect(edit_btn, SIGNAL(clicked()), this, SLOT(edit_settings()));
    /* list of machines */
    connect(listVM, SIGNAL(itemSelectionChanged()), this, SLOT(listVMItemSelectionChanged()));
    connect(listVM, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
        this, SLOT(listVMcurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    /* delete VM */
    connect(delete_act, SIGNAL(triggered()), this, SLOT(delete_vm_ctxmenu()));
    /* exclude VM */
    connect(exclude_act, SIGNAL(triggered()), this, SLOT(exclude_vm_ctxmenu()));
    /* create VM */
    connect(global_config, SIGNAL(globalConfig_new_vm_is_complete()), this, SLOT(refresh()));
}

void QemuGUI::delete_vm_ctxmenu()
{
    int answer = QMessageBox::question(this, "Deleting", "Are you sure?", QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        QString del_vm_name = listVM->currentItem()->text();
        global_config->delete_vm(del_vm_name);
        // may be return value if all ok, delete from list
        delete listVM->currentItem();

        if (listVM->count() != 0)
        {
            listVM->setCurrentRow(0);
        }
        QMessageBox::information(this, "Success", "Virtual machine " + del_vm_name.toUpper() + " was deleted");
    }
}

void QemuGUI::exclude_vm_ctxmenu()
{
    int answer = QMessageBox::question(this, "Excluding", "Are you sure?", QMessageBox::Yes, QMessageBox::No);
    if (answer == QMessageBox::Yes)
    {
        QString del_vm_name = listVM->currentItem()->text();
        global_config->exclude_vm(del_vm_name);
        // may be return value if all ok, exclude from list

        delete listVM->currentItem();

        if (listVM->count() != 0)
        {
            listVM->setCurrentRow(0);
        }
        QMessageBox::information(this, "Success", "Virtual machine " + del_vm_name.toUpper() + " was excluded");
    }
}

void QemuGUI::play_machine()
{
    if (listVM->currentItem())
    {
        QString qemu_exe = QFileDialog::getOpenFileName(this, "Select Qemu executable", "", "*.exe");
        if (qemu_exe != "" && qemu_exe.contains("qemu", Qt::CaseSensitive))
        {
            QProcess qemu;
            //qemu.start("c:/qemu_new/qemu-system-i386.exe");
            qemu.start(qemu_exe);
            qemu.waitForFinished();
        }
        else
        {
            if (qemu_exe != "")
                QMessageBox::critical(this, "Error", "Wrong exe file", QMessageBox::StandardButton::Ok);
        }
    }
}

void QemuGUI::pause_machine()
{
}

void QemuGUI::stop_machine()
{
}

void QemuGUI::create_machine()
{
    createVMWindow = new CreateVMForm(global_config->get_home_dir());
    connect(createVMWindow, SIGNAL(createVM_new_vm_is_complete(VMConfig *)), global_config, SLOT(vm_is_created(VMConfig *)));
}

void QemuGUI::add_machine()
{
    QString filename = QFileDialog::getOpenFileName(this, "Add exist VM", global_config->get_home_dir(), "*.xml");
    if (filename != "")
    {
        if (global_config->add_exist_vm(filename))
            refresh();
        else
            QMessageBox::critical(this, "Error", "Virtual machine cannot be add");
    }
}

void QemuGUI::edit_settings()
{
    settingsWindow = new VMSettingsForm();
}

void QemuGUI::listVMItemSelectionChanged()
{
    if (listVM->currentItem())
    {
        info_lbl->setText(listVM->currentItem()->text());
        propBox->setVisible(true);
        edit_btn->setVisible(true);
        delete_act->setDisabled(false);
        exclude_act->setDisabled(false);
    }
    else
    {
        propBox->setVisible(false);
        edit_btn->setVisible(false);
        delete_act->setDisabled(true);
        exclude_act->setDisabled(true);
    }
}

void QemuGUI::listVMcurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    QFont font;
    QFont font_old = listVM->font();
    int x = font_old.pointSize();
    
    if (previous)
    {
        qDebug() << "current" << current->text() << "previous" << previous->text();
        previous->setTextColor(Qt::GlobalColor::black);
        previous->setFont(font_old);
    }

    font.setBold(true);
    current->setTextColor(Qt::GlobalColor::darkRed);
    current->setFont(font);
}

void QemuGUI::refresh()
{
    QString new_vm_name = global_config->get_created_vm_name();
    listVM->addItem(new_vm_name);
    listVM->setCurrentItem(listVM->item(listVM->count() - 1));
    listVM->currentItem()->setSizeHint(QSize(0, 20));
}



