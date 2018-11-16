#ifndef DEVICEFACTORY_H
#define DEVICEFACTORY_H

#include <QMap>
#include "Device.h"

namespace DeviceFactory
{
    Device *createDevice(const QString &name);
    Devices getDevicesForBus(BusType t);

    namespace Internal
    {
        typedef Device * (*CreateDeviceFunc)();
        typedef QMap<QString, CreateDeviceFunc> DeviceRegistry;

        DeviceRegistry &getDeviceRegistry();

        template<class T>
        Device *createDevice()
        {
            return new T;
        }

        template <class T>
        class RegistryEntry
        {
        public:
            RegistryEntry(const QString &name)
            {
                getDeviceRegistry().insert(name, createDevice<T>);
            }
        };
    }
}

#define REGISTER_DEVICE(TYPE)                                  \
namespace DeviceFactory                                        \
{                                                              \
    namespace Internal                                         \
    {                                                          \
        static RegistryEntry<TYPE> reg_##TYPE(TYPE::typeName); \
    }                                                          \
}

#endif // DEVICEFACTORY_H
