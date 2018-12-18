#include "DeviceForm.h"
#include "CommandLineParameters.h"


DeviceForm::DeviceForm(Device *dev) : device(dev)
{
    QGroupBox *form = this;
    mainLay = new QVBoxLayout(this);
    mainLay->setAlignment(Qt::AlignmentFlag::AlignTop);
    if (device->isRemovable())
    {
        cmdWidget = new DeviceCommandLineForm(dev);
        mainLay->addWidget(cmdWidget);
    }
    form->setLayout(mainLay);
}

void DeviceForm::devFormAddWidget(QWidget *widget)
{
    if (device->isRemovable())
    {
        mainLay->takeAt(mainLay->count() - 1);
        mainLay->addWidget(widget);
        mainLay->addWidget(cmdWidget);
    }
    else
    {
        mainLay->addWidget(widget);
    }
}

void DeviceForm::devFormAddLayout(QLayout *layout)
{
    if (device->isRemovable())
    {
        mainLay->takeAt(mainLay->count() - 1);
        mainLay->addLayout(layout);
        mainLay->addWidget(cmdWidget);
    }
    else
    {
        mainLay->addLayout(layout);
    }
}


/******************************************************************************
* Additional command line options Form                                        *
******************************************************************************/

DeviceCommandLineForm::DeviceCommandLineForm(Device *dev)
    : device(dev)
{
    QGroupBox *cmdGroup = this;
    QPushButton *cmdBtn = new QPushButton("Command line options");
    optionalLbl = new QLabel("Additional options:");
    optionalLine = new QLineEdit();
    cmdLine = new QTextEdit();

    cmdLine->setVisible(false);
    optionalLbl->setVisible(false);
    optionalLine->setVisible(false);

    cmdBtn->setStyleSheet("background-color: white;");

    CommandLineParameters cmd;
    cmdLine->setPlainText("Command line: \n" + device->getCommandLine(cmd).trimmed());
    cmdLine->setReadOnly(true);
    cmdLine->setStyleSheet("background-color: #F0F0F0;");

    optionalLine->setText(dev->getAddtionalCommandLineOption());
    optionalLine->setToolTip("Use next format: \"param1=value1,param2=value2,..\"");

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addWidget(cmdBtn);
    mainLay->addWidget(optionalLbl);
    mainLay->addWidget(optionalLine);
    mainLay->addWidget(cmdLine);
    mainLay->setAlignment(Qt::AlignmentFlag::AlignTop);

    cmdGroup->setLayout(mainLay);

    connect(cmdBtn, &QPushButton::clicked,
        this, &DeviceCommandLineForm::showCmdLine);

    connect(optionalLine, &QLineEdit::editingFinished,
        this, &DeviceCommandLineForm::saveUserOptions);
}

void DeviceCommandLineForm::showCmdLine()
{
    bool isVisible = !cmdLine->isVisible();
    cmdLine->setVisible(isVisible);
    optionalLbl->setVisible(isVisible);
    optionalLine->setVisible(isVisible);
}

void DeviceCommandLineForm::saveUserOptions()
{
    device->setAdditionalCommandLineOption(optionalLine->text().trimmed());
}


/******************************************************************************
* Storage Device Form                                                         *
******************************************************************************/

DeviceStorageForm::DeviceStorageForm(DeviceStorage *dev)
    : DeviceForm(dev), device(dev)
{
    QLineEdit *imageLine = new QLineEdit();
    QPushButton *selectImageBtn = new QPushButton("...");

    selectImageBtn->setFixedWidth(30);
    imageLine->setText(device->getImage());
    imageLine->setReadOnly(true);
    if (device->getImage().isEmpty())
    {
        imageLine->setStyleSheet("background: #EE756F");
    }

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(imageLine);
    topLay->addWidget(selectImageBtn);
    devFormAddLayout(topLay);

    connect(selectImageBtn, &QPushButton::clicked, this, &DeviceStorageForm::editImage);
    connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));
    connect(this, SIGNAL(newDiskCompleted(QString)), imageLine, SLOT(setStyleSheet(QString)));
}

void DeviceStorageForm::editImage()
{
    QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image",
        "", "*.qcow *.qcow2 *.img *.raw");
    if (!newImage.isEmpty())
    {
        emit newImageSet(newImage);
        emit newDiskCompleted("");
        device->setImage(newImage);
    }
}


/******************************************************************************
* SCSI Controller Form                                                        *
******************************************************************************/

DeviceScsiControllerForm::DeviceScsiControllerForm(DeviceScsiController *dev)
    : DeviceForm(dev), device(dev)
{
    QComboBox *controllersCombo = new QComboBox();

    controllersCombo->addItems(device->getControllers());
    controllersCombo->setCurrentText(device->getCurrentController());

    devFormAddWidget(controllersCombo);

    connect(controllersCombo, SIGNAL(currentIndexChanged(const QString &)),
        this, SLOT(setController(const QString &)));
}

void DeviceScsiControllerForm::setController(const QString &name)
{
    device->setController(name);
}


/******************************************************************************
* Device Memory                                                               *
******************************************************************************/

DeviceMemoryForm::DeviceMemoryForm(DeviceMemory *dev)
    : DeviceForm(dev), device(dev)
{
    QLabel *memLbl = new QLabel("Size (MB)");
    QSlider *sizeSlider = new QSlider(Qt::Horizontal);
    QSpinBox *sizeSpin = new QSpinBox();

    sizeSpin->setMaximum(MIN_RAM_VALUE);
    sizeSpin->setMaximum(MAX_RAM_VALUE);
    sizeSpin->setValue(dev->getSize());

    sizeSlider->setMinimum(MIN_RAM_VALUE);
    sizeSlider->setMaximum(MAX_RAM_VALUE);
    sizeSlider->setValue(dev->getSize());

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(memLbl);
    topLay->addWidget(sizeSlider);
    topLay->addWidget(sizeSpin);

    devFormAddLayout(topLay);

    connect(sizeSpin, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));

    connect(sizeSlider, &QSlider::valueChanged, sizeSpin, &QSpinBox::setValue);
    connect(sizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
        sizeSlider, &QSlider::setValue);
}

void DeviceMemoryForm::sizeChanged(int val)
{
    device->setSize(QString::number(val));
}

