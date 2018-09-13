#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>
#include "QemuLauncher.h"

class CommandLineParameters
{
public:
    CommandLineParameters(LaunchMode mode) : id(0), mode(mode), diskID(0) {}
    CommandLineParameters() : id(0), mode(LaunchMode::NORMAL), diskID(0) {}
    
    QString getNextID();
    LaunchMode getLaunchMode();

    /* record|replay */
    void setImage(const QString & imageName) { image = imageName; }
    QString getImage() { return image; }
    void setOverlayDir(const QString & path) { overlayPath = path; }
    QString getOverlayName();
    QString getOverlayNameFromDevice() { return overlayDevice; }
    void setOverlayNameFromDeviceNull() { overlayDevice = ""; }

private:
    int id;
    LaunchMode mode; /* normal, record, replay */
    QString image;
    int diskID;
    QString overlayPath;
    QString overlayDevice;

};

#endif


