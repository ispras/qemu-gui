#ifndef CONSOLETAB_H
#define CONSOLETAB_H

#include <QtWidgets>

class ConsoleTab : public QWidget
{
    Q_OBJECT

public:
    ConsoleTab();
    ~ConsoleTab();

    
private:
    QTextEdit *consoleText;

public slots:
    void addConsoleText(const QString &text);
};


#endif // CONSOLETAB_H
