#include "PlatformInformationReader.h"

PlatformInformationReader::PlatformInformationReader(const QString &qemuPath,
    const QString &homeDir, QemuRunOptions *runOptions)
    : qemuDirPath(qemuPath), platformDirPath(homeDir + "/platforms"),
    runOptions(runOptions)
{
    platforms = { QStringList({ "i386", "pc" }),
        QStringList({ "x86_64", "pc" }),
        QStringList({ "arm", "virt" }),
        /*QStringList({ "mips", "none" }),*/
        QStringList({ "ppc", "none" })
    };

    qmp = NULL;
    qemu = NULL;
    timer = NULL;
    qmpPort = runOptions->getQmpPort();
    allInfoReady = false;

    QDir platformDir(platformDirPath);
    if (!platformDir.exists())
    {
        platformDir.mkdir(platformDirPath);
    }
    currentPlatformDirPath = homeDir + getQemuProfilePath(qemuDirPath);
    QDir currentQemuPlatformDir(currentPlatformDirPath);
    if (!currentQemuPlatformDir.exists())
    {
        currentQemuPlatformDir.mkdir(currentPlatformDirPath);
        createProgressDialog();
        launchQemu();
    }
}

PlatformInformationReader::~PlatformInformationReader()
{
}

QString PlatformInformationReader::getQemuProfilePath(const QString &name)
{
    return "/platforms/qemu_" + getQemuHash(name);
}

QString PlatformInformationReader::getQemuHash(const QString &name)
{
    uint hash = qHash(name);
    return QString::number(hash).setNum(hash, 16);
}


void PlatformInformationReader::launchQemu()
{
    allInfoReady = false;

    qemu = new QemuLauncher(qemuDirPath, runOptions, platforms.first().at(0),
        platforms.first().at(1));
    if (qemu->isQemuExist())
    {
        result.clear();
        result.append(platforms.first());
        qmp = new QMPInteractionSettings(nullptr, qmpPort.toInt());
        connect(qmp, SIGNAL(qmpConnected()), this, SLOT(qmpConnectOk()));
        connect(qmp, SIGNAL(readyInfo(const QStringList&, bool)),
            this, SLOT(nextRequest(const QStringList&, bool)));
        connect(this, SIGNAL(qmpSendCommand()), qmp, SLOT(commandQmp()));
        connect(this, SIGNAL(qmpShutdownQemu()), qmp, SLOT(commandShutdownQemu()));
        connect(this, SIGNAL(qemuMustDie()), qemu, SLOT(terminateQemu()));

        thread = new QThread();
        qemu->moveToThread(thread);
        connect(thread, SIGNAL(started()), qemu, SLOT(start_qemu()));
        connect(qemu, SIGNAL(qemu_laucher_finished(int)),
            this, SLOT(finishQemu(int)));

        timer = new QTimer();
        timer->start(10000);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeIsOut()));

        platforms.removeFirst();
        thread->start();
    }
    else
    {
        qDebug() << "QEMU for platform " << platforms.first().first() << " doesn't exist";
        platforms.removeFirst();
        progress->setValue(progress->maximum() - platforms.count());
        finishQemu(QProcess::CrashExit);
    }
}

void PlatformInformationReader::createProgressDialog()
{
    progress = new QProgressDialog("Please wait...", "", 0, platforms.count());
    progress->setWindowTitle("Reading information");
    progress->setWindowModality(Qt::ApplicationModal);
    progress->setWindowFlags(Qt::WindowCloseButtonHint);
    progress->setCancelButton(NULL);
    progress->setRange(0, platforms.count());
    progress->show();
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
        progress->setValue(progress->maximum() - platforms.count());
        createXml();
        emit qmpShutdownQemu();
    }
}

void PlatformInformationReader::finishQemu(int exitCode)
{
    delete timer;
    timer = NULL;
    delete qemu;
    qemu = NULL;
    delete qmp;
    qmp = NULL;
    if (!platforms.isEmpty())
    {
        launchQemu();
    }
    else
    {
        delete progress;
        emit workFinish();
    }
}

void PlatformInformationReader::createXml()
{
    QFile file(currentPlatformDirPath + "/" + result.first().first() + ".xml");
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

