#ifndef RECORDREPLAYTAB_H
#define RECORDREPLAYTAB_H

#include <QtWidgets>
#include "VMConfig.h"

class RecordReplayTab : public QWidget
{
    Q_OBJECT

public:
    RecordReplayTab(QWidget *parent = 0);
    ~RecordReplayTab();

    void setVM(VMConfig *vm);
private:
    VMConfig *vm;

private:
    void connect_signals();
    void widget_placement();

    private slots:
    void record_execution();
    void replay_execution();
    void execution_listItemSelectionChanged();
    void rename_ctxmenu();
    void delete_ctxmenu();

private:
    QListWidget *execution_list;
    QPushButton *rec_btn;
    QPushButton *rpl_btn;
    QAction *rename_act;
    QAction *delete_act;

signals:
    void startRR();
};

#endif // RECORDREPLAYTAB_H
