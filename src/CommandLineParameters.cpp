#include "CommandLineParameters.h"

QString CommandLineParameters::getNextID()
{
    return "device-" + QString::number(id++);
}

LaunchMode CommandLineParameters::getLaunchMode()
{
    return mode;
}

QString CommandLineParameters::getNextOverlayName()
{
    return overlayPath + "/overlay" + QString::number(diskID++) + ".ovl";
}

QString CommandLineParameters::getOverlayForImage(const QString & image)
{
    images.append(image);
    overlays.append(getNextOverlayName());
    return overlays.back();
}


