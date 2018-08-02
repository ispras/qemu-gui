#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>

class CommandLineParameters
{
public:
    CommandLineParameters() : id(0) {}
    QString getNextID();

private:
    int id;

};

#endif


