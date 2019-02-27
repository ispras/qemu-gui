#ifndef RECORDREPLAYTAB_H
#define RECORDREPLAYTAB_H

#include <QtWidgets>
#include "VMConfig.h"
#include "QemuLauncher.h"
#include "GlobalConfig.h"

class RecordReplayTab : public QWidget
{
    Q_OBJECT

public:
    RecordReplayTab(GlobalConfig *globalConfig, QWidget *parent = 0);
    ~RecordReplayTab();

    void setRecordReplayList(VMConfig *vm);
    QString getCurrentDirRR();
    QString getICountValue();

private:
    VMConfig *vm;
    GlobalConfig *globalConfig;

private:
    void connect_signals();
    void widget_placement();
    void createXml(const QString &path, const QString &name);
    void readXml(const QString &name);
    void setCurrentDir(const QString &name);

private:
    QListWidget *execution_list;
    QPushButton *rec_btn;
    QPushButton *rpl_btn;
    QAction *rename_act;
    QAction *delete_act;

    QDialog *nameDirDialog;
    QLineEdit *nameEdit;
    QSpinBox *icountSpin;
    QString commonDirRR;
    QString currentDirRR;
    QString oldRRName;
    QString nameReplay;
    QString icountValue;
    QString qemuHash;
    bool isNotRunning;

private slots:
    void record_execution();
    void replay_execution();
    void rename_ctxmenu();
    void delete_ctxmenu();
    void setRRNameDir();
    void renameRRRecord();
    void executionListItemActivated(QListWidgetItem *item);

public slots:
    void enableBtns(bool state);
    void recordDeleteRecords();
    void deleteRecordFolder();
    void executionListItemSelectionChanged();

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
