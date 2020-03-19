#include "ConsoleTab.h"

ConsoleTab::ConsoleTab()
{
    consoleText = new QTextEdit();
    consoleText->setReadOnly(true);

    consoleText->setStyleSheet("background-color: black; border: 1px; \
        font-family: \"Courier New\"; color: white; font-size: 14px");

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addWidget(consoleText);

    setLayout(mainLay);
}

ConsoleTab::~ConsoleTab()
{

}

void ConsoleTab::setTextCursor()
{
    QTextCursor cursor(consoleText->textCursor());
    cursor.movePosition(QTextCursor::End);
    consoleText->setTextCursor(cursor);
}

void ConsoleTab::addConsoleText(const QString &text)
{
    setTextCursor();
    consoleText->insertPlainText(text + "\n\n");
    consoleText->ensureCursorVisible();
}

