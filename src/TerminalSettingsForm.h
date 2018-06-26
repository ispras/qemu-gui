#ifndef TERMINALSETTINSFORM_H
#define TERMINALSETTINSFORM_H

#include <QWidget>
#include <QtWidgets>



class TerminalSettingsForm : public QWidget
{
    Q_OBJECT

public:
    TerminalSettingsForm(QWidget *parent, QTextEdit *terminal);
    ~TerminalSettingsForm();

private:
    QTextEdit *test_text;

private slots :
    void save_terminal_interface_changes();
    void closeEvent(QCloseEvent * event);
    void set_background_color();
    void set_text_color();
    void set_text_size(int size);
    void set_test_font(const QFont &font);

public slots:

signals:
    void save_terminal_settings(QTextEdit *);
    void terminal_settings_form_close();
};


#endif //TERMINALSETTINSFORM_H