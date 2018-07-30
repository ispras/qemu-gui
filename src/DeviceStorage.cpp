#include "DeviceStorage.h"
#include "DeviceBus.h"

DeviceStorageController::DeviceStorageController(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceIdeController::typeName[] = "DeviceIdeController";

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController("IDE", parent)
{
    initDefault();
}

void DeviceIdeController::initDefault()
{
    new DeviceBusIde(0, this);
    new DeviceBusIde(1, this);
}

DeviceStorage::DeviceStorage(const QString &n, Device *parent)
    : Device(n, parent)
{

}

/******************************************************************************
 * IDE HDD                                                                    *
 ******************************************************************************/

const char DeviceIdeHd::typeName[] = "DeviceIdeHd";
static const char xml_image[] = "Image";

DeviceIdeHd::DeviceIdeHd(const QString &img, Device *parent)
    : DeviceStorage("IDE-HD", parent), image(img)
{
}

QWidget *DeviceIdeHd::getEditorForm()
{
    QWidget *form = new QWidget();
    QGroupBox *ideFormGroup = new QGroupBox(form);
    QLineEdit *imageLine = new QLineEdit(ideFormGroup);
    QPushButton *selectImageBtn = new QPushButton("...", ideFormGroup);

    selectImageBtn->setFixedWidth(30);
    imageLine->setText(image);
    imageLine->setReadOnly(true);

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(imageLine);
    topLay->addWidget(selectImageBtn);

    mainLay->addLayout(topLay);
    mainLay->addStretch(500);

    ideFormGroup->setLayout(mainLay);

    connect(selectImageBtn, &QPushButton::clicked, this, &DeviceIdeHd::editImage);
    connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));

    return ideFormGroup;
}

void DeviceIdeHd::saveParameters(QXmlStreamWriter &xml) const
{
    xml.writeStartElement(xml_image);
    xml.writeCharacters(image);
    xml.writeEndElement();
}

void DeviceIdeHd::readParameters(QXmlStreamReader &xml)
{
    xml.readNextStartElement();
    Q_ASSERT(xml.name() == xml_image);
    image = xml.readElementText();
}

QString DeviceIdeHd::getCommandLineOption()
{
    /* TODO: use -drive option and correct id */
    return "-hda " + image;
}

void DeviceIdeHd::editImage()
{
    QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image", "", "*.qcow *.qcow2 *.raw");
    if (!newImage.isEmpty())
    {
        image = newImage;
        emit newImageSet(image);
    }
}

