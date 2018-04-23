#include "CreateVMForm.h"


CreateVMForm::CreateVMForm(QString home_dir)
    : QWidget()
{
    if (CreateVMForm::objectName().isEmpty())
        CreateVMForm::setObjectName(QStringLiteral("CreateVMForm"));
    resize(400, 380);
    setWindowTitle(QApplication::translate("CreateVMForm", "Create Virtual Machine", Q_NULLPTR));
    setWindowIcon(QIcon(":Resources/qemu.png"));
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    default_path = home_dir;

    QStringList format = { "QCOW2", "QCOW", "COW", "RAW", "VMDK", "CLOOP", "VPC(VHD)?" };
    QStringList os_type = { "Windows", "Linux", "Ubuntu", "MacOS", "Other" };

    name_edit = new QLineEdit(this);
    pathtovm_edit = new QLineEdit(this);
    pathtovm_btn = new QPushButton("...");
    typeOS_combo = new QComboBox(this);
    verOS_combo = new QComboBox(this);
    ram_slider = new QSlider(Qt::Orientation::Horizontal, this);
    ram_spin = new QSpinBox(this);
    format_combo = new QComboBox(this);
    hddsize_slider = new QSlider(Qt::Orientation::Horizontal, this);
    hddsize_spin = new QSpinBox(this);
    imageplace_edit = new QLineEdit(this);
    imageplace_btn = new QPushButton("...");
    imageplace_lbl = new QLabel("Image place");
    format_lbl = new QLabel("Format");
    hddsize_lbl = new QLabel("Size");

    hdd_no_rb = new QRadioButton("No disk");
    hdd_exist_rb = new QRadioButton("Select exist disk");
    hdd_new_rb = new QRadioButton("Create new disk");

    okCancelBtn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    hddsize_spin->setMaximum(50000);
    hddsize_spin->setValue(1024);
    hddsize_slider->setRange(0, 50000);
    hddsize_slider->setValue(hddsize_spin->value());

    imageplace_btn->setFixedWidth(30);
    pathtovm_btn->setFixedWidth(30);

    format_combo->addItems(format);

    pathtovm_edit->setText(home_dir + "/");
    pathtovm_edit->setReadOnly(true);
    imageplace_edit->setReadOnly(true);
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

    imageplace_lbl->setVisible(false);
    imageplace_edit->setVisible(false);
    imageplace_btn->setVisible(false);

    format_combo->setFixedWidth(330);

    format_lbl->setVisible(false);
    hddsize_lbl->setVisible(false);
    format_combo->setVisible(false);
    hddsize_spin->setVisible(false);
    hddsize_slider->setVisible(false);

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
    QHBoxLayout *dir_lay = new QHBoxLayout();
    dir_lay->addWidget(pathtovm_edit);
    dir_lay->addWidget(pathtovm_btn);
    
    QGridLayout *common_lay = new QGridLayout();
    common_lay->addWidget(new QLabel("Name"), 0, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(name_edit, 0, 1, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(new QLabel("Path to VM"), 1, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addLayout(dir_lay, 1, 1);
    common_lay->addWidget(new QLabel("OS type"), 2, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(typeOS_combo, 2, 1, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(new QLabel("OS version"), 3, 0, Qt::AlignmentFlag::AlignLeft);
    common_lay->addWidget(verOS_combo, 3, 1, Qt::AlignmentFlag::AlignLeft);
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

    QHBoxLayout *place_lay = new QHBoxLayout();
    place_lay->addWidget(imageplace_lbl);
    place_lay->addWidget(imageplace_edit);
    place_lay->addWidget(imageplace_btn);

    // new hdd
    QHBoxLayout *new_hdd_size_lay = new QHBoxLayout();
    new_hdd_size_lay->addWidget(hddsize_lbl);
    new_hdd_size_lay->addWidget(hddsize_slider);
    new_hdd_size_lay->addWidget(hddsize_spin);

    QHBoxLayout *new_hdd_format_lay = new QHBoxLayout();
    new_hdd_format_lay->addWidget(format_lbl);
    new_hdd_format_lay->addWidget(format_combo);

    QVBoxLayout *new_hdd_lay = new QVBoxLayout();
    new_hdd_lay->addLayout(new_hdd_size_lay);
    new_hdd_lay->addLayout(new_hdd_format_lay);

    QVBoxLayout *hdd_all = new QVBoxLayout();
    hdd_all->addLayout(hdd_lay);
    hdd_all->addStretch(10);
    hdd_all->addLayout(place_lay);
    hdd_all->addLayout(new_hdd_lay);

    hdd_gr->setLayout(hdd_all);


    QVBoxLayout *main_lay = new QVBoxLayout(this);
    main_lay->addWidget(common_gr);
    main_lay->addWidget(memory_gr);
    main_lay->addWidget(hdd_gr);
    main_lay->addStretch(50);
    main_lay->addWidget(okCancelBtn);
}


void CreateVMForm::connect_signals()
{
    connect(pathtovm_btn, SIGNAL(clicked()), this, SLOT(select_dir()));
    connect(name_edit, SIGNAL(textEdited(const QString &)), this, SLOT(change_path(const QString &)));

    connect(ram_slider, &QSlider::valueChanged, ram_spin, &QSpinBox::setValue);
    connect(ram_spin, QOverload<int>::of(&QSpinBox::valueChanged), ram_slider, &QSlider::setValue);

    connect(hddsize_slider, &QSlider::valueChanged, hddsize_spin, &QSpinBox::setValue);
    connect(hddsize_spin, QOverload<int>::of(&QSpinBox::valueChanged), hddsize_slider, &QSlider::setValue);

    connect(hdd_no_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_no);
    connect(hdd_exist_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_exist);
    connect(hdd_new_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_new);

    connect(okCancelBtn, &QDialogButtonBox::accepted, this, &CreateVMForm::create_vm);
    connect(okCancelBtn, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(imageplace_btn, SIGNAL(clicked()), this, SLOT(place_disk_or_project()));
}

void CreateVMForm::select_dir()
{
    QString directory_name = QFileDialog::getExistingDirectory(this, "Select directory", default_path);
    if (directory_name != "")
    {
        QString path = directory_name + "/" + name_edit->text();
        pathtovm_edit->setText(path);
        if (QDir(path).exists() && name_edit->text() != "")
        {
            QMessageBox::critical(this, "Error", "Directory with this name already exists");
            name_edit->setFocus();
        }
    }
}

void CreateVMForm::change_path(const QString &name)
{
    if (pathtovm_edit->text() != "")
    {
        QString path = pathtovm_edit->text();
        int index = path.lastIndexOf('/');
        path = path.left(index + 1);
        path.append(name);
        pathtovm_edit->setText(path);
        if (QDir(path).exists())
        {
            QMessageBox::critical(this, "Error", "Directory with this name already exists");
            name_edit->setFocus();
        }
    }
}

void CreateVMForm::setVisibleWidgetsForNewHDD(bool isVisible)
{
    format_combo->setVisible(isVisible);
    hddsize_spin->setVisible(isVisible);
    format_lbl->setVisible(isVisible);
    hddsize_lbl->setVisible(isVisible);
    hddsize_slider->setVisible(isVisible);
}

void CreateVMForm::setVisibleWidgetsForExistedHDD(bool isVisible)
{
    imageplace_lbl->setVisible(isVisible);
    imageplace_edit->setVisible(isVisible);
    imageplace_btn->setVisible(isVisible);
}

void CreateVMForm::hdd_no(bool state)
{
    if (state)
    {
        setVisibleWidgetsForNewHDD(false);
        setVisibleWidgetsForExistedHDD(false);
    }
}

void CreateVMForm::hdd_exist(bool state)
{
    if (state)
    {
        imageplace_edit->clear();
        setVisibleWidgetsForNewHDD(false);
        setVisibleWidgetsForExistedHDD(true);
    }
}

void CreateVMForm::hdd_new(bool state)
{
    if (state)
    {
        imageplace_edit->clear();
        setVisibleWidgetsForNewHDD(true);
        setVisibleWidgetsForExistedHDD(false);
    }
}

void CreateVMForm::place_disk_or_project()
{
    if (hdd_exist_rb->isChecked())
    {
        QString filename = QFileDialog::getOpenFileName(this, "Select image", default_path, "*.qcow *.qcow2 *.vdi *.raw"); // and other
        if (filename != "")
        {
            imageplace_edit->setText(filename);
        }
    }
}

void CreateVMForm::create_vm()
{
    bool form_is_complete = false;
    if (name_edit->text() != "" && !QDir(pathtovm_edit->text()).exists())
    {
        if (imageplace_edit->isVisible())
        {
            if (imageplace_edit->text() != "")
            {
                form_is_complete = true;
            }
        }
        else 
        {
            form_is_complete = true;
        }
    }
    
    if (form_is_complete)
    {
        VMConfig *configVM = new VMConfig();

        configVM->vm_config_set_name(name_edit->text());
        configVM->vm_config_set_dir_path(pathtovm_edit->text());
        configVM->vm_config_set_ram_size(ram_spin->value());

        if (!hdd_new_rb->isChecked())
        {
            configVM->vm_config_set_hdd(NEW_HDD);
            configVM->vm_config_set_image_path(imageplace_edit->text());
        }
        else if (hdd_exist_rb->isChecked())
        {
            configVM->vm_config_set_hdd(EXIST_HDD);
            configVM->vm_config_set_image_path(imageplace_edit->text());
        }

        emit createVM_new_vm_is_complete(configVM);
        close();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Not all fields have been filled right");
    }
}

