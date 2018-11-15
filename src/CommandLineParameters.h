#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>
#include "QemuLauncher.h"

class CommandLineParameters
{
public:
    CommandLineParameters(LaunchMode mode) : mode(mode), diskID(0) {}
    CommandLineParameters() : mode(LaunchMode::NORMAL), diskID(0) {}
    
    LaunchMode getLaunchMode();

    /* record|replay */
    const QStringList & getImages() { return images; }
    const QStringList & getOverlays() { return overlays; }
    void setOverlayDir(const QString & path) { overlayPath = path; }
    QString getOverlayForImage(const QString & image);

private:
    QString getNextOverlayName();

private:
    LaunchMode mode; /* normal, record, replay */
    QStringList images;
    QStringList overlays;
    int diskID;
    QString overlayPath;

};

#endif


