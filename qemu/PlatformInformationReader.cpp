#include "PlatformInformationReader.h"
#include "common/FileHelpers.h"
#include "config/GlobalConfig.h"
#include "config/QemuList.h"

PlatformInformationReader::PlatformInformationReader(const QString &qemu, bool del)
    : qemuName(qemu), profilePath(QemuList::getQemuProfilePath(qemu)), deleteSelf(del),
      thread(NULL)
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

    launcher = new QemuLauncher(qemuName, QemuRunOptions::getGlobal(),
        platforms.first().at(0), platforms.first().at(1));
    if (launcher->isQemuExist())
    {
#ifdef GUI
        result.clear();
        result.append(platforms.first());
        qmp = new QMPInteractionSettings(nullptr, GlobalConfig::get_port_qmp().toInt());
        connect(qmp, SIGNAL(qmpConnected()), this, SLOT(qmpConnectOk()));
        connect(qmp, SIGNAL(readyInfo(const QStringList&, bool)),
            this, SLOT(nextRequest(const QStringList&, bool)));
        connect(this, SIGNAL(qmpSendCommand()), qmp, SLOT(commandQmp()));
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
        result.clear();
        qDebug() << "Scanning " << platforms.first().first();
        result.append(platforms.first());
        launcher->start_qemu();
        qmp = new QMPInteractionSettings(nullptr, GlobalConfig::get_port_qmp().toInt());
        qmp->connectedSocket();
        // TODO: omit order dependency
        qmp->commandQmp();
        result.append(qmp->getInfoList());
        qmp->commandQmp();
        result.append(qmp->getInfoList());
        qmp->commandQmp();
        result.append(qmp->getNetdevList());
        qmp->commandShutdownQemu();
        createXml();
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

void PlatformInformationReader::qmpConnectOk()
{
    emit qmpSendCommand();
}

void PlatformInformationReader::nextRequest(const QStringList &list, bool isReady)
{
    result.append(list);
    if (isReady)
    {
        emit qmpSendCommand();
    }
    else
    {
#ifdef GUI
        progress->setValue(progress->maximum() - platforms.count());
#endif
        createXml();
        emit qmpShutdownQemu();
    }
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

void PlatformInformationReader::createXml()
{
    QFile file(profilePath + "/" + result.first().first() + ".xml");
    if (!file.exists())
    {
        if (file.open(QIODevice::WriteOnly))
        {
            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement(result.first().first());
            result.removeFirst();

            QStringList xmlNames = { "Machine", "Cpu", "Netdev" };
            result[xmlNames.indexOf("Cpu")].push_front("default");
            for (int i = 0; i < xmlNames.count(); i++)
            {
                foreach(QString name, result[i])
                {
                    xmlWriter.writeStartElement(xmlNames[i]);
                    xmlWriter.writeCharacters(name);
                    xmlWriter.writeEndElement();
                }
            }

            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            file.close();
        }
    }
}

