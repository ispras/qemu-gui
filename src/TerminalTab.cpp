#include "TerminalTab.h"

TerminalTab::TerminalTab(QWidget *parent, GlobalConfig *global_config)
    : QWidget(parent)
{
    this->global_config = global_config;

    terminal_text = new QTextEdit();
    welcome_lbl = new QLabel(" >> ");
    terminal_cmd = new QLineEdit();
    terminal_cmd->installEventFilter(this);

    if (global_config->is_terminal_parameters_set())
    {
        set_terminal_interface(global_config->get_terminal_backgroud(),
            global_config->get_terminal_text_color(),
            global_config->get_terminal_font_family(),
            global_config->get_terminal_font_size());
    }
    else
    {
        set_terminal_interface();
    }

    QHBoxLayout *cmd_lay = new QHBoxLayout();
    cmd_lay->setSpacing(0);
    cmd_lay->addWidget(welcome_lbl);
    cmd_lay->addWidget(terminal_cmd);

    QVBoxLayout *terminal_lay = new QVBoxLayout();
    terminal_lay->setSpacing(1);
    terminal_lay->addWidget(terminal_text);
    terminal_lay->addLayout(cmd_lay);
    setLayout(terminal_lay);

    connect(terminal_cmd, SIGNAL(returnPressed()), this, SLOT(send_monitor_command()));
    connect(&monitor_socket, SIGNAL(readyRead()), this, SLOT(read_terminal()));
}

TerminalTab::~TerminalTab()
{

}


void TerminalTab::set_terminal_interface(QColor bckgrnd_color, QColor text_color, const QString &font_family, int font_size)
{
    QString color = text_color.name();

    terminal_text->setReadOnly(true);
    terminal_text->setFontPointSize(font_size);
    terminal_text->setFontFamily(font_family);
    terminal_text->setStyleSheet("background-color: " + bckgrnd_color.name() + "; border: 1px");
    terminal_text->setTextColor(text_color);


    QFont cmd_font = terminal_cmd->font();
    cmd_font.setPointSize(font_size);
    cmd_font.setFamily(font_family);
    terminal_cmd->setFont(cmd_font);
    terminal_cmd->setStyleSheet("background-color: " + bckgrnd_color.name() + "; border: 1px; color: " + color + "; height: " + QString::number(font_size * 2) + "px;");

    welcome_lbl->setStyleSheet("background-color: " + bckgrnd_color.name() + "; color: " + color + "; border: 1px; height: " + QString::number(font_size * 2) + "px; font: bold;");
    welcome_lbl->setFont(cmd_font);

    QString text = terminal_text->toPlainText();
    terminal_text->clear();
    terminal_text->insertPlainText(text);
    terminal_text->ensureCursorVisible();
}

QTextEdit * TerminalTab::get_terminal_text()
{
    return terminal_text;
}

bool TerminalTab::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == terminal_cmd)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Up)
            {
                QString cur_cmd = terminal_cmd->text();
                if (cur_cmd == "" && saved_terminal_cmds.size() > 0)
                {
                    terminal_cmd->setText(saved_terminal_cmds.at(0));
                }
                else
                {
                    int index = saved_terminal_cmds.indexOf(terminal_cmd->text());
                    if (index + 1 < saved_terminal_cmds.size())
                    {
                        terminal_cmd->setText(saved_terminal_cmds.at(index + 1));
                    }
                }
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {
                QString cur_cmd = terminal_cmd->text();
                if (cur_cmd == "" && saved_terminal_cmds.size() > 0)
                {
                    terminal_cmd->setText(saved_terminal_cmds.back());
                }
                else
                {
                    int index = saved_terminal_cmds.indexOf(terminal_cmd->text());
                    if (index > 0)
                    {
                        terminal_cmd->setText(saved_terminal_cmds.at(index - 1));
                    }
                }
                return true;
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

void TerminalTab::send_monitor_command()
{
    monitor_socket.write(terminal_cmd->text().toLocal8Bit() + "\n");
    terminal_text->insertPlainText(terminal_cmd->text() + "\n");
    if (!saved_terminal_cmds.contains(terminal_cmd->text()))
    {
        saved_terminal_cmds.append(terminal_cmd->text());
    }
    terminal_cmd->clear();
}

void TerminalTab::terminalTab_connect()
{
    terminal_text->clear();
    monitor_socket.connectToHost("127.0.0.1", 24);
    qDebug() << "Connecting 127.0.0.1:24";
}

void TerminalTab::read_terminal()
{
    QStringList terminal_answer = (QString::fromLocal8Bit(monitor_socket.readAll())).split('\n');
    foreach(QString line, terminal_answer)
    {
        if (!line.contains("[D") && !line.contains("[K"))
        {
            terminal_text->insertPlainText(line);
        }
    }
    //terminal_text->insertPlainText(monitor_socket.readAll());
    terminal_text->ensureCursorVisible();
}


void TerminalTab::save_terminal_interface_changes(QTextEdit *test_text)
{
    QString style = test_text->styleSheet();
    QStringList style_list = style.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    //foreach(QString str, style_list)
    //{
    //    qDebug() << str;
    //}
    int index = style_list.indexOf("background");

    set_terminal_interface(QColor("#" + style_list.at(index + 2)), test_text->textColor(), test_text->fontFamily(), test_text->fontPointSize());
    global_config->set_terminal_parameters(QColor("#" + style_list.at(index + 2)), test_text->textColor(), test_text->fontFamily(), test_text->fontPointSize());
}
