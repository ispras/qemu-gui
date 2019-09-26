#include "CommandLineParameters.h"

LaunchMode CommandLineParameters::getLaunchMode()
{
    return mode;
}

QString CommandLineParameters::getNextOverlayName()
{
    return overlayPath + "/overlay" + QString::number(diskID++) + ".ovl";
}

QString CommandLineParameters::getOverlayPath()
{
    return overlayPath + "/overlay0.ovl";
}

QString CommandLineParameters::getOverlayForImage(const QString &image)
{
    images.append(image);
    overlays.append(getNextOverlayName());
    return overlays.back();
}


