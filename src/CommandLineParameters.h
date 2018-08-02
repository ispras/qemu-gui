#ifndef COMMANDLINEPARAMETERS_H
#define COMMANDLINEPARAMETERS_H

#include <QtWidgets>

class CommandLineParameters : public QObject
{
public:
    CommandLineParameters(QObject *parent = 0) : id(0) {}
    QString getID();

private:
    int id;

};

#endif


