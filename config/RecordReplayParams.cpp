#include "RecordReplayParams.h"
#include "GlobalConfig.h"
#include "PlatformInformationReader.h"

static const QString constXmlName = "replay.xml";
static const QString xml_hash = "QemuHash";
static const QString xml_icount = "icount";
static const QString xml_overlay = "IsOverlay";
static const QString xml_start = "replay_";

RecordReplayParams::RecordReplayParams()
    : icount(5), snapshotPeriod(0), initialSnapshot("init")
{
}

void RecordReplayParams::createXml(const QString &name) const
{
    QFile file(currentDir + "/" + constXmlName);
    if (file.open(QIODevice::WriteOnly))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(xml_start + name);

        xmlWriter.writeStartElement(xml_hash);
        xmlWriter.writeCharacters(qemuHash);
        xmlWriter.writeEndElement();
             
        xmlWriter.writeStartElement(xml_icount);
        xmlWriter.writeCharacters(QString::number(icount));
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement(xml_overlay);
        xmlWriter.writeCharacters(overlay ? "true" : "");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }
}

void RecordReplayParams::readXml(const QString &name)
{
    QFile file(currentDir + "/" + constXmlName);
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);
        xmlReader.readNextStartElement();
        Q_ASSERT(xmlReader.name() == xml_start + name);

        while (xmlReader.readNextStartElement())
        {
            if (xmlReader.name() == xml_hash)
            {
                qemuHash = xmlReader.readElementText();
            }
            else if (xmlReader.name() == xml_icount)
            {
                icount = xmlReader.readElementText().toInt();
            }
            else if (xmlReader.name() == xml_overlay)
            {
                overlay = !xmlReader.readElementText().isEmpty();
            }
        }
    }
}

QString RecordReplayParams::getCommandLine(LaunchMode mode) const
{
    QString initSnapshotCmd = overlay ? ",rrsnapshot=" + initialSnapshot : "";
    QString rr = mode == LaunchMode::RECORD ? "record" : "replay";
    QString res = "-icount shift=" + QString::number(icount)
         + ",rr=" + rr + ",rrfile=" +
        "\"" + currentDir + "/replay.bin\"" + initSnapshotCmd;
    if (snapshotPeriod)
    {
        res += ",rrperiod=" + QString::number(snapshotPeriod);
    }
    return res;
}
