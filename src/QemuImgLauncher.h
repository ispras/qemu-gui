#ifndef QEMUIMGLAUNCHER_H
#define QEMUIMGLAUNCHER_H

#include <QWidget>
#include <QtWidgets>

class QemuImgLauncher : public QObject
{
    Q_OBJECT

public:
    QemuImgLauncher(const QString &qemuImg, const QString &imageFormat, 
        const QString &imageName, int imageSize, QObject *parent = 0);
    ~QemuImgLauncher();

private:
    QString qemuImg;
    QString imageFormat;
    QString imageName;
    int imageSize; /* in megabytes */

    QProgressDialog *imgDlg;

public slots:
    void start_qemu_img();
    void finish_qemu_img(int exitCode, QProcess::ExitStatus ExitStatus);

signals:
    void qemu_img_finished(int);

};
#endif // QEMUIMGLAUNCHER_H

