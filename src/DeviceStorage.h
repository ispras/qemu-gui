#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include "Device.h"

/*class DeviceStorage : public Device
{
public:
    DeviceStorage(const QString &n, Device *parent);
};*/

class DeviceStorageController : public Device
{
public:
    DeviceStorageController() {}
    DeviceStorageController(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceIdeController : public DeviceStorageController
{
public:
    static const char typeName[];

    DeviceIdeController() { initDefault(); }
    DeviceIdeController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }

private:
    void initDefault();
};

class DeviceStorage : public Device
{
public:
    DeviceStorage() {}
    DeviceStorage(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceIdeHd : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeHd() {}
    DeviceIdeHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    void setNewHDD(const QString &imageName) { image = imageName; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption();

private:
    QString image;

};

class DeviceIdeHdForm : public QWidget
{
Q_OBJECT

public:
    DeviceIdeHdForm(DeviceIdeHd *dev, const QString &image) 
        : device(dev), imageName(image) {}

    QWidget *getForm()
    {
        QWidget *form = new QWidget();
        QGroupBox *ideFormGroup = new QGroupBox(form);
        QLineEdit *imageLine = new QLineEdit(ideFormGroup);
        QPushButton *selectImageBtn = new QPushButton("...", ideFormGroup);

        selectImageBtn->setFixedWidth(30);
        imageLine->setText(imageName);
        imageLine->setReadOnly(true);

        QVBoxLayout *mainLay = new QVBoxLayout();
        QHBoxLayout *topLay = new QHBoxLayout();
        topLay->addWidget(imageLine);
        topLay->addWidget(selectImageBtn);

        mainLay->addLayout(topLay);
        mainLay->addStretch(500);

        ideFormGroup->setLayout(mainLay);
        connect(selectImageBtn, &QPushButton::clicked, this, &DeviceIdeHdForm::editImage);
        connect(this, SIGNAL(newImageSet(QString)), imageLine, SLOT(setText(QString)));

        return ideFormGroup;
    }
    

private:
    QString imageName;
    DeviceIdeHd *device;

    private slots:
    void editImage()
    {
        QString newImage = QFileDialog::getOpenFileName(nullptr, "Selecting image", "", "*.qcow *.qcow2 *.raw");
        if (!newImage.isEmpty())
        {
            imageName = newImage;
            emit newImageSet(imageName);
            device->setNewHDD(imageName);
        }
    }

signals:
    void newImageSet(QString);

};

#endif // DEVICESTORAGE_H
