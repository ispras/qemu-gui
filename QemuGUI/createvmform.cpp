#include "CreateVMForm.h"


CreateVMForm::CreateVMForm(QWidget *parent)
    : QWidget(parent)
{
    if (CreateVMForm::objectName().isEmpty())
        CreateVMForm::setObjectName(QStringLiteral("CreateVMForm"));
    resize(400, 340);
    setWindowTitle(QApplication::translate("CreateVMForm", "Create Virtual Machine", Q_NULLPTR));
    setWindowModality(Qt::WindowModality::ApplicationModal);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    QStringList format = { "RAW", "QCOW2", "QCOW", "COW", "VMDK", "CLOOP", "VPC(VHD)?" };
    QStringList os_type = { "Windows", "Linux", "Ubuntu", "MacOS", "Other" };

    name_edit = new QLineEdit(this);
    typeOS_combo = new QComboBox(this);
    verOS_combo = new QComboBox(this);
    ram_slider = new QSlider(Qt::Orientation::Horizontal, this);
    ram_spin = new QSpinBox(this);
    format_combo = new QComboBox(this);
    hdsize_spin = new QSpinBox(this);
    place_edit = new QLineEdit(this);
    place_btn = new QPushButton("...");
    place_lbl = new QLabel("Image place");
    format_lbl = new QLabel("Format");
    hddsize_lbl = new QLabel("Disk size");

    hdd_no_rb = new QRadioButton("No disk");
    hdd_exist_rb = new QRadioButton("Select exist disk");
    hdd_new_rb = new QRadioButton("Create new disk");

    okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    hdsize_spin->setMaximum(50000);
    hdsize_spin->setValue(1024);

    place_btn->setFixedWidth(30);

    format_combo->addItems(format);

    name_edit->setFixedWidth(330);
    typeOS_combo->setFixedWidth(330);
    verOS_combo->setFixedWidth(330);
    typeOS_combo->addItems(os_type);

    ram_spin->setMinimum(MIN_RAM_VALUE);
    ram_spin->setMaximum(MAX_RAM_VALUE);
    ram_spin->setValue(512);
    ram_slider->setRange(MIN_RAM_VALUE, MAX_RAM_VALUE);
    ram_slider->setValue(ram_spin->value());

    hdd_no_rb->setChecked(true);

    place_lbl->setVisible(false);
    place_edit->setVisible(false);
    place_btn->setVisible(false);

    format_combo->setFixedWidth(100);

    format_lbl->setVisible(false);
    hddsize_lbl->setVisible(false);
    format_combo->setVisible(false);
    hdsize_spin->setVisible(false);

    widget_placement();
    connect_signals();
    show();
}

CreateVMForm::~CreateVMForm()
{

}

void CreateVMForm::widget_placement()
{
    QGroupBox *common_gr = new QGroupBox("OS information");
    QGridLayout *common_lay = new QGridLayout();
    common_lay->addWidget(new QLabel("Name"), 0, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(name_edit, 0, 1, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(new QLabel("OS type"), 1, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(typeOS_combo, 1, 1, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(new QLabel("OS version"), 2, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(verOS_combo, 2, 1, Qt::AlignmentFlag::AlignLeft);
    common_lay->setColumnStretch(1, 100);

    common_gr->setLayout(common_lay);

    QGroupBox *memory_gr = new QGroupBox("Memory size");
    memory_gr->setMinimumWidth(this->width());

    QHBoxLayout *memory_lay = new QHBoxLayout();
    memory_lay->addWidget(new QLabel("RAM"));
    memory_lay->addWidget(ram_slider);
    memory_lay->addWidget(ram_spin);

    memory_gr->setLayout(memory_lay);

    QGroupBox *hdd_gr = new QGroupBox("Disk");
    QHBoxLayout *hdd_lay = new QHBoxLayout();

    hdd_lay->addWidget(hdd_no_rb);
    hdd_lay->addWidget(hdd_exist_rb);
    hdd_lay->addWidget(hdd_new_rb);
    hdd_lay->addStretch(50);

    hdd_gr->setLayout(hdd_lay);

    QHBoxLayout *place_lay = new QHBoxLayout();
    place_lay->addWidget(place_lbl);
    place_lay->addWidget(place_edit);
    place_lay->addWidget(place_btn);

    QHBoxLayout *hdd_prop_lay = new QHBoxLayout();
    hdd_prop_lay->addWidget(format_lbl);
    hdd_prop_lay->addWidget(format_combo);
    hdd_prop_lay->addStretch(10);
    hdd_prop_lay->addWidget(hddsize_lbl);
    hdd_prop_lay->addWidget(hdsize_spin);
    hdd_prop_lay->addStretch(40);

    QVBoxLayout *main_lay = new QVBoxLayout(this);
    main_lay->addWidget(common_gr);
    main_lay->addWidget(memory_gr);
    main_lay->addWidget(hdd_gr);
    main_lay->addLayout(place_lay);
    main_lay->addLayout(hdd_prop_lay);
    main_lay->addStretch(50);
    main_lay->addWidget(okCancelBtn);
}

void CreateVMForm::connect_signals()
{
    connect(ram_slider, &QSlider::valueChanged, ram_spin, &QSpinBox::setValue);
    connect(ram_spin, QOverload<int>::of(&QSpinBox::valueChanged), ram_slider, &QSlider::setValue);

    connect(hdd_no_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_no);
    connect(hdd_exist_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_exist);
    connect(hdd_new_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_new);

    //connect(okCancelBtn, &QDialogButtonBox::accepted, this, &QemuSettings::save_settings);
    connect(okCancelBtn, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(place_btn, SIGNAL(clicked()), this, SLOT(place_disk_or_project()));
}

void CreateVMForm::setVisibleWidgets_pt1(bool isVisible)
{
    format_combo->setVisible(isVisible);
    hdsize_spin->setVisible(isVisible);
    format_lbl->setVisible(isVisible);
    hddsize_lbl->setVisible(isVisible);
}

void CreateVMForm::setVisibleWidgets_pt2(bool isVisible)
{
    place_lbl->setVisible(isVisible);
    place_edit->setVisible(isVisible);
    place_btn->setVisible(isVisible);
}

void CreateVMForm::hdd_no(bool state)
{
    if (state)
    {
        setVisibleWidgets_pt1(false);
        setVisibleWidgets_pt2(false);
    }
}

void CreateVMForm::hdd_exist(bool state)
{
    if (state)
    {
        place_edit->clear();
        setVisibleWidgets_pt1(false);
        setVisibleWidgets_pt2(true);
    }
}

void CreateVMForm::hdd_new(bool state)
{
    if (state)
    {
        place_edit->clear();
        setVisibleWidgets_pt1(true);
        setVisibleWidgets_pt2(true);
    }
}

void CreateVMForm::place_disk_or_project()
{
    if (hdd_exist_rb->isChecked())
    {
        QString filename = QFileDialog::getOpenFileName(this, "Select image", "", "*.qcow *.qcow2 *.vdi *.raw"); // and other
        if (filename != "")
        {
            place_edit->setText(filename);
        }
    }
    else
    {
        QString directoryName = QFileDialog::getExistingDirectory(this, "Select directory", "");
        if (directoryName != "")
        {
            place_edit->setText(directoryName);
        }
    }
}
