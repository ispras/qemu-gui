#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>
#include "QemuLauncher.h"

class CommandLineParameters
{
public:
    CommandLineParameters(LaunchMode mode) : mode(mode), diskID(0),
        overlayEnabled(false) {}
    CommandLineParameters() : mode(LaunchMode::NORMAL), diskID(0),
        overlayEnabled(false) {}
    
    LaunchMode getLaunchMode();

    /* record|replay */
    const QStringList &getImages() { return images; }
    const QStringList &getOverlays() { return overlays; }
    void setOverlayDir(const QString &path) { overlayPath = path; }
    QString getOverlayForImage(const QString &image);
    void setOverlayEnabled(bool isEnabled) { overlayEnabled = isEnabled; }
    bool isOverlayEnabled() { return overlayEnabled; }

private:
    QString getNextOverlayName();

public:
    QString getOverlayPath();

private:
    LaunchMode mode; /* normal, record, replay */
    QStringList images;
    QStringList overlays;
    int diskID;
    QString overlayPath;
    bool overlayEnabled;

};

#endif


