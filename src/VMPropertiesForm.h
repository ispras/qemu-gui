#ifndef VMPROPERTIESFORM_H
#define VMPROPERTIESFORM_H

#include "QtWidgets"

class VMPropertiesForm : public QGroupBox
{
    Q_OBJECT

public:
    VMPropertiesForm(QWidget *parent = 0, const QString &caption = "");

private:
    QLineEdit *kernelEdit;
    QPushButton *kernelBtn;
    QLineEdit *initrdEdit;
    QPushButton *initrdBtn;

private slots:
    void setSystemFile();

public:
    QString getKernel() const;
    QString getInitrd() const;
    void setKernel(const QString &filename);
    void setInitrd(const QString &filename);


};

#endif // VMPROPERTIESFORM_H

