#include "QemuInstallationsForm.h"
#include "QemuList.h"

QemuInstallationsForm::QemuInstallationsForm(QWidget *parent)
    : QDialog(parent)
{
    setWindowIcon(QIcon(":Resources/qemu.png"));
    setWindowTitle("Qemu installation folders");
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    //qemu_install_dir_list = new QListWidget();
    QPushButton *add_install_dir_btn = new QPushButton("Add QEMU");
    add_install_dir_btn->setAutoDefault(true);
    QPushButton *del_install_dir_btn = new QPushButton("Delete QEMU");
    del_install_dir_btn->setAutoDefault(true);

    QHBoxLayout *buttons_lay = new QHBoxLayout();
    buttons_lay->addWidget(add_install_dir_btn);
    buttons_lay->addWidget(del_install_dir_btn);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(buttons_lay);

    list = new QTableWidget();
    layout->addWidget(list);
    list->setColumnCount(2);

    setLayout(layout);

    connect(add_install_dir_btn, &QPushButton::clicked,
        [=]() { editListItem(-1, "New installation", "", ""); }
    );
    connect(del_install_dir_btn, SIGNAL(clicked()),
        this, SLOT(deleteInstallation()));
    connect(list, &QTableWidget::cellDoubleClicked,
        [=](int row, int column)
        {
            editListItem(row, "Edit installation",
                list->item(row, 0)->text(),
                list->item(row, 1)->text());
        }
    );

    reloadList();
}

void QemuInstallationsForm::reloadList()
{
    const QemuList::Items &inst = QemuList::getAllQemuInstallations();
    list->setRowCount(inst.count());
    list->setHorizontalHeaderLabels({"Name", "Path"});
    size_t i = 0;
    foreach (QString name, inst.keys())
    {
        list->setItem(i, 0, new QTableWidgetItem(name));
        list->setItem(i, 1, new QTableWidgetItem(inst.value(name)));
        ++i;
    }
}

void QemuInstallationsForm::deleteInstallation()
{
    if (list->currentRow() >= 0)
    {
        int answer = QMessageBox::question(this, "Deleting", "Are you sure?",
            QMessageBox::Yes, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            QString name = list->item(list->currentRow(), 0)->text();
            QemuList::delQemuInstallation(name);
            reloadList();
        }
    }
}

void QemuInstallationsForm::editListItem(int item, const QString &caption,
    const QString &name, const QString &dir)
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowIcon(QIcon(":Resources/qemu.png"));
    dlg->setWindowTitle(caption);
    dlg->setModal(true);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout *topLay = new QHBoxLayout();
    topLay->addWidget(new QLabel("Installation name"));
    nameEdit = new QLineEdit();
    topLay->addWidget(nameEdit);

    QHBoxLayout *bottomLay = new QHBoxLayout();
    bottomLay->addWidget(new QLabel("Binaries path"));
    pathEdit = new QLineEdit();
    bottomLay->addWidget(pathEdit);
    QPushButton *findButton = new QPushButton("...");
    findButton->setFixedWidth(30);
    bottomLay->addWidget(findButton);

    QVBoxLayout *mainLay = new QVBoxLayout();
    mainLay->addLayout(topLay);
    mainLay->addLayout(bottomLay);
    QDialogButtonBox *okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel);
    mainLay->addWidget(okCancelBtn);

    dlg->setLayout(mainLay);

    nameEdit->setText(name);
    pathEdit->setText(dir);

    dlg->open();

    connect(findButton, &QPushButton::clicked,
        [=]()
        {
            QString path = QFileDialog::getExistingDirectory(this, "Select Qemu directory", "");
            if (path != "")
            {
                pathEdit->setText(path);
            }
        });

    connect(okCancelBtn, &QDialogButtonBox::rejected,
        [=]() { dlg->close(); });
    connect(okCancelBtn, &QDialogButtonBox::accepted,
        [=]()
        {
            if (nameEdit->text() != "" && pathEdit->text() != "")
            {
                if (item == -1 || nameEdit->text() != name)
                {
                    if (QemuList::getQemuDir(nameEdit->text()) != "")
                    {
                        QMessageBox::critical(this, "Error", nameEdit->text() + " is already added");
                        return;
                    }
                }
                if (item != -1)
                {
                    QemuList::delQemuInstallation(name);
                }
                QemuList::addQemuInstallation(nameEdit->text(), pathEdit->text());
                reloadList();
                dlg->close();
            }
            else
            {
                QMessageBox::critical(this, "Error", "Name and path should be specified");
            }
        });
}
