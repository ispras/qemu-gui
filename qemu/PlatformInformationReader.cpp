#include "PlatformInformationReader.h"
#include "common/FileHelpers.h"
#include "config/GlobalConfig.h"
#include "config/QemuList.h"

PlatformInformationReader::PlatformInformationReader(const QString &qemu, bool del)
    : qemuName(qemu), profilePath(QemuList::getQemuProfilePath(qemu)), deleteSelf(del),
      thread(NULL), platformInfo(nullptr)
#ifdef GUI
    , timer(NULL)
#endif
{
    platforms = { QStringList({ "i386", "pc" }),
        QStringList({ "x86_64", "pc" }),
        QStringList({ "arm", "virt" }),
        QStringList({ "mips", "none" }),
        QStringList({ "mips64", "none" }),
        QStringList({ "ppc", "none" })
    };

    qmp = NULL;
    launcher = NULL;
    allInfoReady = false;

    FileHelpers::deleteDirectory(profilePath);
    FileHelpers::createDirectory(profilePath);
    createProgressDialog();
    launchQemu();
}

void PlatformInformationReader::launchQemu()
{
    allInfoReady = false;

    QemuRunOptions opt = QemuRunOptions::getGlobal();
    opt.setQemuRunStopped(true);
    launcher = new QemuLauncher(qemuName, opt,
        platforms.first().at(0), platforms.first().at(1));
    if (launcher->isQemuExist())
    {
        platformInfo = new PlatformInfo(profilePath + "/" + platforms.first().first());
#ifdef GUI
        qmp = new QMPInteractionSettings(nullptr,
            GlobalConfig::get_port_qmp().toInt(), platformInfo);
        connect(qmp, SIGNAL(ready()),
            this, SLOT(nextRequest()));
        connect(this, SIGNAL(qmpShutdownQemu()), qmp, SLOT(commandShutdownQemu()));
        connect(this, SIGNAL(qemuMustDie()), launcher, SLOT(terminateQemu()));

        thread = new QThread();
        launcher->moveToThread(thread);
        connect(thread, SIGNAL(started()), launcher, SLOT(start_qemu()));
        connect(launcher, SIGNAL(qemu_laucher_finished(int)),
            this, SLOT(finishQemu(int)));
        timer = new QTimer();
        timer->start(10000);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeIsOut()));
        platforms.removeFirst();
        thread->start();
#else
        qDebug() << "Scanning " << platforms.first().first();
        launcher->start_qemu();
        qmp = new QMPInteractionSettings(nullptr,
            GlobalConfig::get_port_qmp().toInt(), platformInfo);
        qmp->connectedSocket();
        qmp->commandShutdownQemu();
        platformInfo->saveXml();
        platforms.removeFirst();
        /* Recursive */
        finishQemu(0);
#endif
    }
    else
    {
        qDebug() << "QEMU for platform " << platforms.first().first() << " doesn't exist";
        platforms.removeFirst();
#ifdef GUI
        progress->setValue(progress->maximum() - platforms.count());
#endif
        finishQemu(QProcess::CrashExit);
    }
}

void PlatformInformationReader::createProgressDialog()
{
#ifdef GUI
    progress = new QProgressDialog("Please wait...", "", 0, platforms.count());
    progress->setWindowTitle("Reading information");
    progress->setWindowModality(Qt::ApplicationModal);
    progress->setWindowFlags(Qt::WindowCloseButtonHint);
    progress->setCancelButton(NULL);
    progress->setRange(0, platforms.count());
    progress->show();
#endif
}

void PlatformInformationReader::timeIsOut()
{
    emit qemuMustDie();
}

void PlatformInformationReader::nextRequest()
{
#ifdef GUI
    progress->setValue(progress->maximum() - platforms.count());
#endif
    platformInfo->saveXml();
    /* To avoid nextRequest invocation on quit */
    disconnect(this, SLOT(nextRequest()));
    emit qmpShutdownQemu();
}

void PlatformInformationReader::finishQemu(int exitCode)
{
#ifdef GUI
    delete timer;
    timer = NULL;
#endif
    delete launcher;
    launcher = NULL;
    delete qmp;
    qmp = NULL;
    delete platformInfo;
    platformInfo = nullptr;
    if (!platforms.isEmpty())
    {
        launchQemu();
    }
    else
    {
#ifdef GUI
        delete progress;
#endif
        emit workFinish();
        if (deleteSelf)
            delete this;
    }
}
