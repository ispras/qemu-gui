#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include "Device.h"



namespace DeviceFactory
{
    Device *createDevice(QStringRef name);
};

#endif // DEVICEFACTORY_H
