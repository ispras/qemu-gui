#include "qemugui.h"


QemuGUI::QemuGUI(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowTitle(QApplication::translate("QemuGUIClass", "QemuGUI", Q_NULLPTR));

	//main menu
	ui.menuBar->addMenu("What");
	ui.menuBar->addMenu("do");
	ui.menuBar->addMenu("you");
	ui.menuBar->addMenu("want?");

	// tool menu
	ui.mainToolBar->addAction(QIcon("play.gif"), "Play VM", this, SLOT(play_machine()));
	ui.mainToolBar->addAction(QIcon("pause.gif"), "Pause VM", this, SLOT(pause_machine()));
	ui.mainToolBar->addAction(QIcon("stop.gif"), "Stop VM", this, SLOT(stop_machine()));
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction("Create machine", this, SLOT(create_machine()));
	ui.mainToolBar->addAction("Import machine", this, SLOT(import_machine()));

	// tab	
	tab = new QTabWidget(ui.centralWidget);
	tab->setMinimumWidth(400);
	tab_info = new QWidget(ui.centralWidget);
	tab->addTab(tab_info, "Information about VM");

	propBox = new QGroupBox(tab_info);
	edit_btn = new QPushButton("Edit VM", tab_info);
	info_lbl = new QLabel("", propBox);

	propBox->setMinimumWidth(300);
	propBox->setVisible(false);
	edit_btn->setVisible(false);
	
	listVM = new QListWidget();
	listVM->setMaximumWidth(300);
	
	recReplay = new RecordReplay();
	tab->addTab(recReplay, "Record/Replay");


	this->widget_placement();
	this->connect_signals();
}


QemuGUI::~QemuGUI()
{
	delete settingsWindow;
	delete createVMDlg;
	delete recReplay;
}

void QemuGUI::widget_placement()
{
	QVBoxLayout *infoGroupLay = new QVBoxLayout(propBox);
	infoGroupLay->addWidget(info_lbl);
	infoGroupLay->addStretch(100);

	QVBoxLayout *infoLay = new QVBoxLayout(tab_info);
	infoLay->addWidget(propBox);
	infoLay->addWidget(edit_btn);

	QHBoxLayout *one = new QHBoxLayout(ui.centralWidget);

	QVBoxLayout *lay = new QVBoxLayout(ui.centralWidget);
	lay->addWidget(listVM);

	QVBoxLayout *lay2 = new QVBoxLayout(ui.centralWidget);
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
	for (int i = 0; i < listVM->count(); i++)
	{
		qDebug() << listVM->item(i)->text();
		if (listVM->item(i)->isSelected())
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
}

void QemuGUI::pause_machine()
{
}

void QemuGUI::stop_machine()
{
}

void QemuGUI::create_machine()
{
	createVMDlg = new CreateVM();
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
	settingsWindow = new QemuSettings();
}

void QemuGUI::onListVMItemClicked(QListWidgetItem *item)
{
	info_lbl->setText(item->text());
	propBox->setVisible(true);
	edit_btn->setVisible(true);
}

