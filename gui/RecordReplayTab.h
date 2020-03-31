#ifndef RECORDREPLAYTAB_H
#define RECORDREPLAYTAB_H

#include <QtWidgets>
#include "VMConfig.h"
#include "QemuLauncher.h"
#include "RecordReplayParams.h"

class RecordReplayTab : public QWidget
{
    Q_OBJECT

public:
    RecordReplayTab(QWidget *parent = 0);
    ~RecordReplayTab();

    void setVM(VMConfig *virtualMachine);
    void clearVM();

    const RecordReplayParams &getRecordReplayParams() const { return rrParams; }

private:
    VMConfig *vm;
    QWidget *pWidget;

private:
    void connect_signals();
    void executionListConnectSignals();
    void widget_placement();
    void createXml();
    void setCurrentDir(const QString &name);
    void createDialog(const QString &caption);
    bool checkPeriodSet();
    bool checkReplayForm();
    QHBoxLayout *periodLayout(int width);
    QHBoxLayout *overlayLayout();
    QStringList getSnapshotInfo();

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
    QComboBox *snapshotCombo;
    QString commonDirRR;
    QString oldRRName;
    QString nameReplay;
    QString icountValue;
    QStringList snapshotTips;
    bool isNotRunning;
    RecordReplayParams rrParams;

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
