#ifndef RECORDREPLAYPARAMS_H
#define RECORDREPLAYPARAMS_H

#include <QtCore/QtCore>

class RecordReplayParams : public QObject
{
    Q_OBJECT
public:
    RecordReplayParams(QObject *parent = NULL);

    void createXml(const QString &name) const;
    void readXml(const QString &name);

    const QString &getCurrentDir() const { return currentDir; }
    void setCurrentDir(const QString &dir) { currentDir = dir; }
    const QString &getQemuHash() const { return qemuHash; }
    void setQemuHash(const QString &hash) { qemuHash = hash; }
    bool isOverlayEnabled() const { return overlay; }
    void setOverlayEnabled(bool value) { overlay = value; }
    int getIcount() const { return icount; }
    void setIcount(int value) { icount = value; }

private:
    QString currentDir;
    QString qemuHash;
    bool overlay;
    int icount;
};

#endif // RECORDREPLAYPARAMS_H
