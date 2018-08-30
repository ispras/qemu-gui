#ifndef ADDDEVICEFORM_H
#define ADDDEVICEFORM_H

#include <QtWidgets>

class AddDeviceForm : public QWidget
{
    Q_OBJECT

public:
    AddDeviceForm(const QStringList &listDevice);
    ~AddDeviceForm();

public slots:
    void addDevice();

private:
    QListWidget *deviceList;
    

private slots:
    void addNewDevice();

signals:
    void deviceWantsToAdd(const QString &);


};

#endif // ADDDEVICEFORM_H
