#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>
#include "QemuGUICommon.h"

class CommandLineParameters
{
public:
    CommandLineParameters(LaunchMode mode) : id(0), mode(mode) {}
    CommandLineParameters() : id(0) {}
    
    QString getNextID();
    LaunchMode getLaunchMode();

private:
    int id;
    LaunchMode mode; /* normal, record, replay */
    QString dirRR;

};

#endif


