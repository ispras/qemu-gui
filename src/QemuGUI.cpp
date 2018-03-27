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
    mainToolBar->addAction("Import machine", this, SLOT(import_machine()));

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
    listVM->setMaximumWidth(300);
    selected_item = NULL;

    recReplayTab = new RecordReplayTab();
    tab->addTab(recReplayTab, "Record/Replay");

    widget_placement();
    connect_signals();
}

QemuGUI::~QemuGUI()
{
    delete settingsWindow;
    delete createVMWindow;
    delete recReplayTab;
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
    connect(listVM, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onListVMItemClicked(QListWidgetItem *)));
}

void QemuGUI::play_machine()
{
    if (selected_item)
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
    createVMWindow = new CreateVMForm();
}

void QemuGUI::import_machine()
{
    QString filename = QFileDialog::getOpenFileName(this, "Import VM", "", "");
    if (filename != "")
    {
        listVM->addItem(filename);
    }
}

void QemuGUI::edit_settings()
{
    settingsWindow = new VMSettingsForm();
}

void QemuGUI::onListVMItemClicked(QListWidgetItem *item)
{
    selected_item = item;
    info_lbl->setText(item->text());
    propBox->setVisible(true);
    edit_btn->setVisible(true);
}

