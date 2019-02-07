#include "PlatformInformation.h"


PlatformInformation::PlatformInformation(const QString &qemuPath,
    const QString &homeDir)
    : qemuDirPath(qemuPath), platformDirPath(homeDir + "/platforms")
{
    platforms = { QStringList({ "i386", "pc" }),
        QStringList({ "arm", "virt" }),
        /*QStringList({ "mips", "none" }),*/
        QStringList({ "ppc", "none" })
    };

    qmp = NULL;
    qemu = NULL;
    qmpPort = "2323";
    allInfoReady = false;

    QDir platformDir(platformDirPath);
    if (!platformDir.exists())
    {
        platformDir.mkdir(platformDirPath);
        launchQemu();
        // TODO: what about different qemu
    }    
}

PlatformInformation::~PlatformInformation()
{
}


void PlatformInformation::launchQemu()
{
    allInfoReady = false;

    result.clear();
    result.append(platforms.first());
    qmp = new QMPInteractionSettings(nullptr, qmpPort.toInt());
    connect(this, SIGNAL(qmpRequestMachineInfo()), qmp, SLOT(commandMachineInfo()));
    connect(this, SIGNAL(qmpRequestCpuInfo()), qmp, SLOT(commandCpuInfo()));
    connect(qmp, SIGNAL(qmpConnected()), this, SLOT(qmpConnectOk()));
    connect(qmp, SIGNAL(readyInfo(const QStringList&)), 
        this, SLOT(nextRequest(const QStringList&)));
    connect(this, SIGNAL(qmpShutdownQemu()), qmp, SLOT(commandShutdownQemu()));

    QThread *thread = new QThread();
    qemu = new QemuLauncher(qemuDirPath, platforms.first().at(0), 
        platforms.first().at(1), qmpPort);
    qemu->moveToThread(thread);
    connect(thread, SIGNAL(started()), qemu, SLOT(start_qemu()));
    connect(qemu, SIGNAL(qemu_laucher_finished(int)),
        this, SLOT(finishQemu(int)));
    thread->start();

    platforms.removeFirst();
}

void PlatformInformation::qmpConnectOk()
{
    emit qmpRequestMachineInfo();
}

void PlatformInformation::nextRequest(const QStringList &list)
{
    result.append(list);
    if (!allInfoReady)
    {
        emit qmpRequestCpuInfo();
        allInfoReady = true;
    }
    else
    {
        createXml();
        emit qmpShutdownQemu();
    }
}

void PlatformInformation::finishQemu(int exitCode)
{
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
        emit workFinish();
    }
}

void PlatformInformation::createXml()
{
    QFile file(platformDirPath + "/" + result.first().first() + ".xml");
    if (!file.exists())
    {
        if (file.open(QIODevice::WriteOnly))
        {
            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.setAutoFormatting(true);
            xmlWriter.writeStartDocument();
            xmlWriter.writeStartElement(result.first().first());

            result.removeFirst();
            foreach(QString machine, result.first())
            {
                xmlWriter.writeStartElement("Machine");
                xmlWriter.writeCharacters(machine);
                xmlWriter.writeEndElement();
            }
            foreach(QString cpu, result.back())
            {
                xmlWriter.writeStartElement("Cpu");
                xmlWriter.writeCharacters(cpu);
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            file.close();
        }
    }
}

