#ifndef RECORDREPLAYTAB_H
#define RECORDREPLAYTAB_H

#include <QtWidgets>
#include "VMConfig.h"
#include "QemuLauncher.h"

class RecordReplayTab : public QWidget
{
    Q_OBJECT

public:
    RecordReplayTab(QWidget *parent = 0);
    ~RecordReplayTab();

    void setRecordReplayList(VMConfig *vm);
    QString getCurrentDirRR();

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

    QDialog *nameDirDialog;
    QLineEdit *nameEdit;
    QString commonDirRR;
    QString currentDirRR;
    QString oldRRName = "";

private slots:
    void setRRNameDir();
    void renameRRRecord();

public slots:
    void enableBtns(bool state);
    void recordDeleteRecords();

signals:
    void startRR(LaunchMode mode);
};


/******************************************************************************
* EXECUTION_LIST DELEGATE                                                     *
******************************************************************************/

class RecordRRDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    RecordRRDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const;

private slots:
    void editingFinished();

signals:
    void renamingEnded();
};

#endif // RECORDREPLAYTAB_H
