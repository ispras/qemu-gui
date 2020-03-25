#include "TerminalTab.h"
#include "GlobalConfig.h"

TerminalTab::TerminalTab(QWidget *parent)
    : QWidget(parent)
{
    terminalText = new QTextEdit();
    welcomeLbl = new QLabel(" >> ");
    terminalCmd = new QLineEdit();
    terminalCmd->installEventFilter(this);

    setTerminalInterface(GlobalConfig::get_terminal_backgroud(),
        GlobalConfig::get_terminal_text_color(),
        GlobalConfig::get_terminal_font_family(),
        GlobalConfig::get_terminal_font_size());

    QHBoxLayout *cmd_lay = new QHBoxLayout();
    cmd_lay->setSpacing(0);
    cmd_lay->addWidget(welcomeLbl);
    cmd_lay->addWidget(terminalCmd);

    QVBoxLayout *terminal_lay = new QVBoxLayout();
    terminal_lay->setSpacing(1);
    terminal_lay->addWidget(terminalText);
    terminal_lay->addLayout(cmd_lay);
    setLayout(terminal_lay);

    connect(terminalCmd, SIGNAL(returnPressed()), this, SLOT(sendMonitorCommand()));
    connect(&monitorSocket, SIGNAL(readyRead()), this, SLOT(readTerminal()));

    currentCommandIndex = -1;
}

TerminalTab::~TerminalTab()
{

}


void TerminalTab::setTerminalInterface(QColor bckgrnd_color, QColor text_color,
    const QString &font_family, int font_size)
{
    QString color = text_color.name();

    terminalText->setReadOnly(true);
    terminalText->setFontPointSize(font_size);
    terminalText->setFontFamily(font_family);
    terminalText->setStyleSheet("background-color: " + bckgrnd_color.name()
        + "; border: 1px");
    terminalText->setTextColor(text_color);


    QFont cmd_font = terminalCmd->font();
    cmd_font.setPointSize(font_size);
    cmd_font.setFamily(font_family);
    terminalCmd->setFont(cmd_font);
    terminalCmd->setStyleSheet("background-color: " + bckgrnd_color.name()
        + "; border: 1px; color: " + color + "; height: "
        + QString::number(font_size * 2) + "px;");

    welcomeLbl->setStyleSheet("background-color: " + bckgrnd_color.name()
        + "; color: " + color + "; border: 1px; height: "
        + QString::number(font_size * 2) + "px; font: bold;");
    welcomeLbl->setFont(cmd_font);

    QString text = terminalText->toPlainText();
    terminalText->clear();
    terminalText->insertPlainText(text);
    terminalText->ensureCursorVisible();
}

QTextEdit *TerminalTab::getTerminalText()
{
    return terminalText;
}

void TerminalTab::setCommmandLineFocus()
{
    terminalCmd->setFocus();
}

bool TerminalTab::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == terminalCmd)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Up)
            {
                if (currentCommandIndex + 1 < savedTerminalCmds.size())
                {
                    terminalCmd->setText(savedTerminalCmds.at(++currentCommandIndex));
                }
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Down)
            {
                if (currentCommandIndex - 1 >= 0)
                {
                    terminalCmd->setText(savedTerminalCmds.at(--currentCommandIndex));
                }
                else
                {
                    currentCommandIndex = -1;
                    terminalCmd->setText("");
                }
                return true;
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

void TerminalTab::sendMonitorCommand()
{
    monitorSocket.write(terminalCmd->text().toLocal8Bit() + "\n");
    QTextCursor cursor(terminalText->textCursor());
    cursor.movePosition(QTextCursor::End);
    terminalText->setTextCursor(cursor);
    terminalText->insertPlainText(terminalCmd->text() + "\n");
    savedTerminalCmds.push_front(terminalCmd->text());
    terminalCmd->clear();
    currentCommandIndex = -1;
}

void TerminalTab::terminalTab_connect(int port)
{
    terminalText->clear();
    monitorSocket.connectPort(port);
}

void TerminalTab::readTerminal()
{
    QStringList terminal_answer = (QString::fromLocal8Bit(monitorSocket.readAll())).split('\n');
    foreach(QString line, terminal_answer)
    {
        if (!line.contains("[D") && !line.contains("[K"))
        {
            terminalText->insertPlainText(line);
        }
    }
    terminalText->ensureCursorVisible();
}


void TerminalTab::terminalTab_abort()
{
    monitorSocket.abort();
}

void TerminalTab::save_terminal_interface_changes(QTextEdit *test_text)
{
    QString style = test_text->styleSheet();
    QStringList style_list = style.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    int index = style_list.indexOf("background");

    setTerminalInterface(QColor("#" + style_list.at(index + 2)), test_text->textColor(), 
        test_text->fontFamily(), test_text->fontPointSize());
    GlobalConfig::set_terminal_parameters(QColor("#" + style_list.at(index + 2)),
        test_text->textColor(), test_text->fontFamily(), test_text->fontPointSize());
}
