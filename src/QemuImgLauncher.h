#ifndef QEMUIMGLAUNCHER_H
#define QEMUIMGLAUNCHER_H

#include <QWidget>
#include <QtWidgets>

class QemuImgLauncher : public QObject
{
    Q_OBJECT

public:
    QemuImgLauncher(const QString &qemuImg, const QString &imageFormat,
        const QString &imageName, QObject *parent = 0);
    QemuImgLauncher(const QString &qemuImg, const QString &imageFormat, 
        const QString &imageName, int imageSize, QObject *parent = 0);
    QemuImgLauncher(const QString &qemuImg, const QString &imageFormat,
        const QString &imageName, const QString &path, QObject *parent = 0);
    ~QemuImgLauncher();

    QStringList getSnapshotInformation();
    
private:
    QString qemuImg;
    QString imageFormat;
    QString imageName;
    int imageSize; /* in megabytes */
    QString overlayName;

private:
    void startQemuImg(QProcess &qemuImgProc, const QString &cmd);

public slots:
    void startQemuImgCreateDisk();
    void startQemuImgCreateOverlay();
    void finish_qemu_img(int exitCode, QProcess::ExitStatus ExitStatus);

signals:
    void qemu_img_finished(int);

};
#endif // QEMUIMGLAUNCHER_H

