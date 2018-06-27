#include "ConnectionSettingsForm.h"

ConnectionSettingsForm::ConnectionSettingsForm(GlobalConfig *global_config, QWidget *parent)
    : QWidget(parent)
{
    if (ConnectionSettingsForm::objectName().isEmpty())
        ConnectionSettingsForm::setObjectName(QStringLiteral("ConnectionSettings"));
    resize(200, 100);

    setWindowTitle(QApplication::translate("ConnectionSettings", "Connection settings", Q_NULLPTR));
    setWindowIcon(QIcon(":Resources/qemu.png"));
    setWindowModality(Qt::ApplicationModal);
    //setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    this->global_config = global_config;

    qmp_line = new QLineEdit();
    monitor_line = new QLineEdit();
    QDialogButtonBox *savecancel_btn = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);

    qmp_line->setFixedWidth(50);
    monitor_line->setFixedWidth(50);
    qmp_line->setValidator(new QRegExpValidator(QRegExp("[1-9][0-9]*"), this));
    monitor_line->setValidator(new QRegExpValidator(QRegExp("[1-9][0-9]*"), this));
    qmp_line->setText(global_config->get_port_qmp());
    monitor_line->setText(global_config->get_port_monitor());

    QGridLayout *ports_lay = new QGridLayout();
    ports_lay->addWidget(new QLabel("QMP port"), 0, 0, Qt::AlignmentFlag::AlignLeft);
    ports_lay->addWidget(qmp_line, 0, 1);
    ports_lay->addWidget(new QLabel("Monitor port"), 1, 0, Qt::AlignmentFlag::AlignLeft);
    ports_lay->addWidget(monitor_line, 1, 1);

    QVBoxLayout *main_lay = new QVBoxLayout(this);
    main_lay->addLayout(ports_lay);
    main_lay->addWidget(savecancel_btn);

    show();

    connect(savecancel_btn, &QDialogButtonBox::accepted, this, &ConnectionSettingsForm::save_connection_settings);
    connect(savecancel_btn, &QDialogButtonBox::rejected, this, &QWidget::close);
}

ConnectionSettingsForm::~ConnectionSettingsForm()
{
}


void ConnectionSettingsForm::closeEvent(QCloseEvent *event)
{
}

void ConnectionSettingsForm::save_connection_settings()
{
    if (qmp_line->text().isEmpty() || monitor_line->text().isEmpty() || qmp_line->text() == monitor_line->text())
    {
        QMessageBox::critical(this, "Input error", "You are wrong");
    }
    else
    {
        global_config->set_port_qmp(qmp_line->text());
        global_config->set_port_monitor(monitor_line->text());
        emit done_connection_settings(qmp_line->text(), monitor_line->text());
        close();
    }
}



