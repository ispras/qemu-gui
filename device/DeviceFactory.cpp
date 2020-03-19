#include "DeviceFactory.h"
#include "DeviceStorage.h"
#include "DeviceSystem.h"
#include "DeviceUsb.h"

namespace DeviceFactory
{

Device *createDevice(const QString &name)
{
    auto create = Internal::getDeviceRegistry().value(name);
    if (create)
        return create();

    return new Device();
}


Devices getDevicesForBus(BusType t)
{
    Devices res;
    Internal::DeviceRegistry &reg = Internal::getDeviceRegistry();
    foreach(auto create, reg)
    {
        Device *d = create();
        if (d->needsBus() == t)
        {
            res.append(d);
        }
        else
        {
            delete d;
        }
    }
    return res;
}


QWidget *getDeviceEditorForm()
{
    return NULL;
}


namespace Internal
{

DeviceRegistry &getDeviceRegistry()
{
    static DeviceRegistry reg;
    return reg;
}

}

}
