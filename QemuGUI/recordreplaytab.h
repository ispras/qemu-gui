#ifndef RECORDREPLAYTAB_H
#define RECORDREPLAYTAB_H

#include <QtWidgets>

class RecordReplayTab : public QWidget
{
    Q_OBJECT

public:
    RecordReplayTab(QWidget *parent = 0);
    ~RecordReplayTab();

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
    QListWidget *journal_list;
    QListWidgetItem *selected_item;
    QPushButton *rec_btn;
    QPushButton *rpl_btn;
    QAction *rename_act;
    QAction *delete_act;
};

#endif // RECORDREPLAYTAB_H
