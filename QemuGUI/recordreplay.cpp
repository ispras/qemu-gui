#include "recordreplay.h"

RecordReplay::RecordReplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	selected_item = NULL;

	journal_list = new QListWidget();
	rec_btn = new QPushButton("Start record");
	rpl_btn = new QPushButton("Start replay");

	rename_act = new QAction("Rename");
	delete_act = new QAction("Delete");
	rename_act->setDisabled(true);
	delete_act->setDisabled(true);

	journal_list->setEditTriggers(QAbstractItemView::AnyKeyPressed | QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);

	journal_list->addItem("First");
	journal_list->addItem("Second");
	journal_list->addItem("Third");


	journal_list->addAction(rename_act);
	journal_list->addAction(delete_act);
	journal_list->setContextMenuPolicy(Qt::ActionsContextMenu);
	

	this->widget_placement();
	this->connect_signals();
}

RecordReplay::~RecordReplay()
{

}

void RecordReplay::connect_signals()
{
	connect(rec_btn, SIGNAL(clicked()), this, SLOT(record_journal()));
	connect(rpl_btn, SIGNAL(clicked()), this, SLOT(replay_journal()));

	connect(journal_list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onJournalListtemClicked(QListWidgetItem *)));

	connect(rename_act, SIGNAL(triggered()), this, SLOT(rename_ctxmenu()));
	connect(delete_act, SIGNAL(triggered()), this, SLOT(delete_ctxmenu()));
}

void RecordReplay::widget_placement()
{
	QVBoxLayout *main_lay = new QVBoxLayout(this);

	QHBoxLayout *lay_btn = new QHBoxLayout(this);
	lay_btn->addWidget(rec_btn);
	lay_btn->addWidget(rpl_btn);

	main_lay->addWidget(new QLabel("List of existing journals"));
	main_lay->addWidget(journal_list);
	main_lay->addLayout(lay_btn);
}

void RecordReplay::record_journal()
{
	QMessageBox::about(this, "", "record");
}

void RecordReplay::replay_journal()
{
	QMessageBox::about(this, "", "replay");
}

void RecordReplay::onJournalListtemClicked(QListWidgetItem *item)
{
	rename_act->setDisabled(false);
	delete_act->setDisabled(false);
	selected_item = item;
}

void RecordReplay::rename_ctxmenu()
{
	if (selected_item)
	{
		//ui.selected_item->setText("ooops");
		selected_item->setFlags(selected_item->flags() | Qt::ItemIsEditable);
	}
}

void RecordReplay::delete_ctxmenu()
{
	if (selected_item)
	{
		delete selected_item;
		journal_list->clearSelection();
		selected_item = NULL;
		rename_act->setDisabled(true);
		delete_act->setDisabled(true);
	}
}
