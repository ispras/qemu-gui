#ifndef QMPINTERACTION_H
#define QMPINTERACTION_H

#include <QWidget>
#include <QtWidgets>


class QMPInteraction : public QObject
{
    Q_OBJECT

public:
    QMPInteraction(QObject *parent = 0);
    ~QMPInteraction();

public:
    QByteArray init();
    QByteArray cmd_stop();
    QByteArray cmd_continue();

    int what_said_qmp(QByteArray message);

private:

};


#endif // QMPINTERACTION_H

