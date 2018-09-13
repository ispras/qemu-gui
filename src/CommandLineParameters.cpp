#include "CommandLineParameters.h"

QString CommandLineParameters::getNextID()
{
    return "device-" + QString::number(id++);
}

LaunchMode CommandLineParameters::getLaunchMode()
{
    return mode;
}

QString CommandLineParameters::getOverlayName()
{
    overlayDevice = overlayPath + "/overlay" + QString::number(diskID++) + ".ovl";
    return overlayDevice;
}


