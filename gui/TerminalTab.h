#ifndef TERMINALTAB_H
#define TERMINALTAB_H

#include <QtWidgets>
#include <QTcpSocket>
#include "QemuSocket.h"


class TerminalTab : public QWidget
{
    Q_OBJECT

public:
    TerminalTab(QWidget *parent = 0);
    ~TerminalTab();

protected:
    bool eventFilter(QObject *target, QEvent *event);

private:
    QTextEdit *terminalText;
    QLabel *welcomeLbl;
    QLineEdit *terminalCmd;

    QStringList savedTerminalCmds;

    QemuSocket monitorSocket;

    int currentCommandIndex;

private:
    void setTerminalInterface(QColor bckgrnd_color, QColor text_color,
        const QString &font_family, int font_size);

public:
    QTextEdit *getTerminalText();
    void setCommmandLineFocus();

private slots:
    void sendMonitorCommand();
    void readTerminal();

public slots:
    void terminalTab_connect(int port);
    void terminalTab_abort();
    void save_terminal_interface_changes(QTextEdit *test_text);

};




#endif //TERMINALTAB_H
