#include "CommandLineParameters.h"

QString CommandLineParameters::getNextID()
{
    return "device-" + QString::number(id++);
}

LaunchMode CommandLineParameters::getLaunchMode()
{
    return mode;
}


