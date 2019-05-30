#include "PlatformInformation.h"


QStringList PlatformInformation::getPlatformInfoByName(const QString &path,
    const QString &element)
{
    QStringList list;
    QFile file(path + ".xml");
    if (file.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&file);
        xmlReader.readNextStartElement();
        QString platform = path.section('/', -1);
        Q_ASSERT(xmlReader.name() == platform);

        while (xmlReader.readNextStartElement())
        {
            if (xmlReader.name() == element)
            {
                list.append(xmlReader.readElementText());
            }
            else
            {
                xmlReader.readElementText();
            }
        }
    }
    return list;
}

