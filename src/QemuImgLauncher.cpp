#include "QemuImgLauncher.h"

QemuImgLauncher::QemuImgLauncher(const QString &qemuImg, const QString &imageFormat,
    const QString &imageName, QObject *parent) :
    QObject(parent), imageFormat(imageFormat),
    imageName(imageName)
{
    this->qemuImg = qemuImg
#ifdef Q_OS_WIN
        + "/qemu-img.exe";
#else
        + "/qemu-img";
#endif
}

QemuImgLauncher::QemuImgLauncher(const QString &qemuImg, const QString &imageFormat, 
    const QString &imageName, int imageSize, QObject *parent) :
    QemuImgLauncher(qemuImg, imageFormat, imageName, parent)
{
    this->imageSize = imageSize;
}

QemuImgLauncher::QemuImgLauncher(const QString &qemuImg, const QString &imageFormat,
    const QString &imageName, const QString &path, QObject *parent) :
    QemuImgLauncher(qemuImg, imageFormat, imageName, parent)
{
    overlayName = path;
}

QemuImgLauncher::~QemuImgLauncher()
{

}

void QemuImgLauncher::startQemuImg(const QString & cmd)
{
    QProcess *qemuImgProc = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(qemuImgProc, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(finish_qemu_img(int, QProcess::ExitStatus)));
    qemuImgProc->start(cmd);
    qemuImgProc->waitForFinished(-1);
}

void QemuImgLauncher::startQemuImgCreateOverlay()
{
    QString cmd = "\"" + qemuImg + "\"" + " create -f qcow2 -b " +
        imageName + " " + overlayName;
    qDebug() << "create overlay: " << cmd << "----";
    startQemuImg(cmd);
}

void QemuImgLauncher::finish_qemu_img(int exitCode, QProcess::ExitStatus ExitStatus)
{
    qDebug() << "Process is finished" << exitCode << ExitStatus;
    emit qemu_img_finished(exitCode);
}

void QemuImgLauncher::startQemuImgCreateDisk()
{
    QString cmd = "\"" + qemuImg + "\"" + " create -f " + imageFormat + " " +
        imageName + " " + QString().number(imageSize) + "M";
    startQemuImg(cmd);
}


