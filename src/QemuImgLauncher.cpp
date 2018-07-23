#include "QemuImgLauncher.h"

QemuImgLauncher::QemuImgLauncher(const QString &qemuImg, const QString &imageFormat, const QString &imageName, int imageSize, QObject *parent) :
    QObject(parent)
{
    this->qemuImg = qemuImg + "/qemu-img.exe";
    this->imageFormat = imageFormat;
    this->imageName = imageName;
    this->imageSize = imageSize;
}

QemuImgLauncher::~QemuImgLauncher()
{

}

void QemuImgLauncher::finish_qemu_img(int exitCode, QProcess::ExitStatus ExitStatus)
{
    qDebug() << "Process is finished" << exitCode << ExitStatus;
    emit qemu_img_finished(exitCode);
}


void QemuImgLauncher::start_qemu_img()
{
    QProcess *qemuImgProc = new QProcess();
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
    connect(qemuImgProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finish_qemu_img(int, QProcess::ExitStatus)));
    QString cmd = qemuImg + " create -f " + imageFormat + " " + imageName + " " + QString().number(imageSize) + "M";
    qemuImgProc->start(cmd);
    qemuImgProc->waitForFinished(-1);
}

