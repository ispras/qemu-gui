#ifndef TERMINALTAB_H
#define TERMINALTAB_H

#include <QtWidgets>
#include <QTcpSocket>
#include "GlobalConfig.h"


class TerminalTab : public QWidget
{
    Q_OBJECT

public:
    TerminalTab(GlobalConfig *global_config, QWidget *parent = 0);
    ~TerminalTab();

protected:
    bool eventFilter(QObject *target, QEvent *event);

private:
    GlobalConfig *global_config;

    QTextEdit *terminal_text;
    QLabel *welcome_lbl;
    QLineEdit *terminal_cmd;

    QStringList saved_terminal_cmds;

    QTcpSocket monitor_socket;

private:
    void set_terminal_interface(QColor bckgrnd_color = Qt::black, QColor text_color = Qt::green,
        const QString &font_family = "Courier New", int font_size = 12);

public:
    QTextEdit *get_terminal_text();

private slots:
    void send_monitor_command();
    void read_terminal();

public slots:
    void terminalTab_connect(int port);
    void save_terminal_interface_changes(QTextEdit *test_text);

};




#endif //TERMINALTAB_H
