#include "DeviceForm.h"

/***************************************************************************
* DeviceIdeHdForm                                                          *
***************************************************************************/

DeviceIdeHdForm::DeviceIdeHdForm(DeviceIdeHd *dev) : device(dev)
{
    QGroupBox *ideFormGroup = this;
    QLineEdit *imageLine = new QLineEdit(ideFormGroup);
    QPushButton *selectImageBtn = new QPushButton("...", ideFormGroup);

    selectImageBtn->setFixedWidth(30);
    imageLine->setText(device->getImage());
    imageLine->setReadOnly(true);
    if (device->getImage().isEmpty())
    {
        imageLine->setStyleSheet("background: #EE756F");
    }

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(imageLine);
    topLay->addWidget(selectImageBtn);

    mainLay->addLayout(topLay);
    mainLay->addStretch(500);

    ideFormGroup->setLayout(mainLay);
    connect(selectImageBtn, &QPushButton::clicked, this, &DeviceIdeHdForm::editImage);
    connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));
    connect(this, SIGNAL(newDiskCompleted(QString)), imageLine, SLOT(setStyleSheet(QString)));
}

void DeviceIdeHdForm::editImage()
{
    QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image",
        "", "*.qcow *.qcow2 *.raw");
    if (!newImage.isEmpty())
    {
        emit newImageSet(newImage);
        emit newDiskCompleted("");
        device->setImage(newImage);
    }
}


///***************************************************************************
//* DeviceIdeCdromForm                                                       *
//***************************************************************************/

DeviceIdeCdromForm::DeviceIdeCdromForm(DeviceIdeCdrom *dev) : device(dev)
{
    QGroupBox *ideFormGroup = this;
    QLineEdit *imageLine = new QLineEdit(ideFormGroup);
    QPushButton *selectImageBtn = new QPushButton("...", ideFormGroup);

    selectImageBtn->setFixedWidth(30);
    imageLine->setText(device->getImage());
    imageLine->setReadOnly(true);
    if (device->getImage().isEmpty())
    {
        imageLine->setStyleSheet("background: #EE756F");
    }

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(imageLine);
    topLay->addWidget(selectImageBtn);

    mainLay->addLayout(topLay);
    mainLay->addStretch(500);

    ideFormGroup->setLayout(mainLay);
    connect(selectImageBtn, &QPushButton::clicked, this, &DeviceIdeCdromForm::editImage);
    connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));
    connect(this, SIGNAL(newDiskCompleted(QString)), imageLine, SLOT(setStyleSheet(QString)));
}

void DeviceIdeCdromForm::editImage()
{
    QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image",
        "", "*.qcow *.qcow2 *.raw");
    if (!newImage.isEmpty())
    {
        emit newImageSet(newImage);
        emit newDiskCompleted("");
        device->setImage(newImage);
    }
}

///***************************************************************************
//* DeviceIdeCdromForm                                                       *
//***************************************************************************/

DeviceScsiHdForm::DeviceScsiHdForm(DeviceScsiHd *dev) : device(dev)
{
    QGroupBox *ideFormGroup = this;
    QLineEdit *imageLine = new QLineEdit(ideFormGroup);
    QPushButton *selectImageBtn = new QPushButton("...", ideFormGroup);

    selectImageBtn->setFixedWidth(30);
    imageLine->setText(device->getImage());
    imageLine->setReadOnly(true);
    if (device->getImage().isEmpty())
    {
        imageLine->setStyleSheet("background: #EE756F");
    }

    QVBoxLayout *mainLay = new QVBoxLayout();
    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(imageLine);
    topLay->addWidget(selectImageBtn);

    mainLay->addLayout(topLay);
    mainLay->addStretch(500);

    ideFormGroup->setLayout(mainLay);
    connect(selectImageBtn, &QPushButton::clicked, this, &DeviceScsiHdForm::editImage);
    connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));
    connect(this, SIGNAL(newDiskCompleted(QString)), imageLine, SLOT(setStyleSheet(QString)));
}

void DeviceScsiHdForm::editImage()
{
    QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image",
        "", "*.qcow *.qcow2 *.raw");
    if (!newImage.isEmpty())
    {
        emit newImageSet(newImage);
        emit newDiskCompleted("");
        device->setImage(newImage);
    }
}