#ifndef RECORDREPLAY_H
#define RECORDREPLAY_H

#include <QtWidgets>
#include "ui_recordreplay.h"

class RecordReplay : public QWidget
{
	Q_OBJECT

public:
	RecordReplay(QWidget *parent = 0);
	~RecordReplay();

private:
	void connect_signals();
	void widget_placement();

private slots:
	void record_journal();
	void replay_journal();
	void onJournalListtemClicked(QListWidgetItem *item);
	void rename_ctxmenu();
	void delete_ctxmenu();

private:
	Ui::RecordReplay ui;

	QListWidget *journal_list;
	QPushButton *rec_btn;
	QPushButton *rpl_btn;
	QAction *rename_act;
	QAction *delete_act;

	QListWidgetItem *selected_item;
};

#endif // RECORDREPLAY_H
