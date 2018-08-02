#include "CommandLineParameters.h"

QString CommandLineParameters::getID()
{
    return "device-" + QString::number(id++);
}

