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
    QStringList getImages() const { return images; }
    QStringList getOverlays() const { return overlays; }
    void setOverlayDir(const QString & path) { overlayPath = path; }
    QString getOverlayForImage(const QString & image);

private:
    QString getOverlayName();

private:
    int id;
    LaunchMode mode; /* normal, record, replay */
    QStringList images;
    QStringList overlays;
    int diskID;
    QString overlayPath;

};

#endif


