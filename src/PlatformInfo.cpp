#include "PlatformInfo.h"


PlatformInfo::PlatformInfo(const QString &path)
{
    QFile file(path + ".xml");
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);
        xmlReader.readNextStartElement();
        QString platform = path.section('/', -1);
        Q_ASSERT(xmlReader.name() == platform);

        while (xmlReader.readNextStartElement())
        {
            if (xmlReader.name() == "Machine")
            {
                machines.append(xmlReader.readElementText());
            }
            else if (xmlReader.name() == "Cpu")
            {
                cpus.append(xmlReader.readElementText());
            }
            else if (xmlReader.name() == "Netdev")
            {
                netdev.append(xmlReader.readElementText());
            }
        }
    }
}



