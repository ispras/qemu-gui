#include "VMPropertiesForm.h"

VMPropertiesForm::VMPropertiesForm(QWidget *parent, const QString &caption)
{
    QGroupBox *form = this;
    form->setTitle(caption);

    kernelEdit = new QLineEdit(this);
    initrdEdit = new QLineEdit(this);
    kernelBtn = new QPushButton("...");
    initrdBtn = new QPushButton("...");

    const char regExpForName[] = "[A-Za-z0-9_-][A-Za-z0-9 _-s:/.~()\"]+";
    kernelEdit->setValidator(new QRegExpValidator(QRegExp(regExpForName), this));
    initrdEdit->setValidator(new QRegExpValidator(QRegExp(regExpForName), this));

    kernelBtn->setFixedWidth(30);
    kernelBtn->setAutoDefault(true);
    initrdBtn->setFixedWidth(30);
    initrdBtn->setAutoDefault(true);

    QGridLayout *kernelLay = new QGridLayout();
    kernelLay->addWidget(new QLabel("Kernel"), 0, 0);
    kernelLay->addWidget(kernelEdit, 0, 1);
    kernelLay->addWidget(kernelBtn, 0, 2);
    kernelLay->addWidget(new QLabel("Initial ram disk"), 1, 0);
    kernelLay->addWidget(initrdEdit, 1, 1);
    kernelLay->addWidget(initrdBtn, 1, 2);

    form->setLayout(kernelLay);

    connect(kernelBtn, SIGNAL(clicked()), this, SLOT(setSystemFile()));
    connect(initrdBtn, SIGNAL(clicked()), this, SLOT(setSystemFile()));
}

void VMPropertiesForm::setSystemFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select file",
        "", "*.*"); // what about format?
    if (!filename.isEmpty())
    {
        if (sender() == kernelBtn)
        {
            kernelEdit->setText(filename);
        }
        else
        {
            initrdEdit->setText(filename);
        }
    }
}

QString VMPropertiesForm::getKernel() const
{
    return kernelEdit->text();
}

QString VMPropertiesForm::getInitrd() const
{
    return initrdEdit->text();
}

void VMPropertiesForm::setKernel(const QString &filename)
{
    kernelEdit->setText(filename);
}

void VMPropertiesForm::setInitrd(const QString &filename)
{
    initrdEdit->setText(filename);
}

