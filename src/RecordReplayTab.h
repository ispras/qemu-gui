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

    void setRecordReplayList(VMConfig *virtualMachine);
    QString getCurrentDirRR();
    QString getICountValue();
    QString getSnapshotPeriod();
    bool isOverlayEnabled();
    void setSnapshotPeriod(QString val);
    void clearExecutionList();

private:
    VMConfig *vm;
    GlobalConfig *globalConfig;
    QWidget *pWidget;

private:
    void connect_signals();
    void executionListConnectSignals();
    void widget_placement();
    void createXml(const QString &path, const QString &name);
    void readXml(const QString &name);
    void setCurrentDir(const QString &name);
    void createDialog(const QString &caption);
    bool checkPeriodSet();
    bool checkReplayForm();
    QHBoxLayout *periodLayout(int width);
    QHBoxLayout *overlayLayout();

private:
    QListWidget *executionList;
    QPushButton *rec_btn;
    QPushButton *rpl_btn;
    QAction *renameAct;
    QAction *deleteAct;
    QAction *deleteAllAct;

    QDialog *replayDialog;
    QLineEdit *nameEdit;
    QSpinBox *icountSpin;
    QCheckBox *periodCheckBox;
    QLineEdit *periodLineEdit;
    QCheckBox *overlayCheck;
    QString commonDirRR;
    QString currentDirRR;
    QString oldRRName;
    QString nameReplay;
    QString icountValue;
    QString qemuHash;
    QString periodAutoSnap;
    bool isNotRunning;
    bool isOverlayChecked;
    int currentIcount;

private slots:
    void record_execution();
    void replay_execution();
    void rename_ctxmenu();
    void delete_ctxmenu();
    void deleteAllCtxmenu();
    void setRRNameDir();
    void setPeriodSnapReplay();
    void renameRRRecord();
    void executionListItemClicked(QListWidgetItem *item);
    void autoSnapshotEnabled(int state);
    void setCurrentIcount(int value);
    void setAutoSnapshotEnabled(int value);

public slots:
    void enableBtns(bool state);
    void setState(bool state);
    void recordDeleteRecords();
    void deleteRecordFolder();
    void executionListItemSelectionChanged();
    void executionListItemRowChanged(int currentRow);
    void replayCurrentQemuChanged();
    bool isExecutionsExist();

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
