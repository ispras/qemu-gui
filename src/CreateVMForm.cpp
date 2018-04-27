#include "CreateVMForm.h"


CreateVMForm::CreateVMForm(const QString &home_dir)
    : QWidget()
{
    if (CreateVMForm::objectName().isEmpty())
        CreateVMForm::setObjectName(QStringLiteral("CreateVMForm"));
    resize(400, 400);
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
    imageplace_lbl = new QLabel("Image path");
    format_lbl = new QLabel("Format");
    hddsize_lbl = new QLabel("Size");

    hdd_no_rb = new QRadioButton("No disk");
    hdd_exist_rb = new QRadioButton("Select exist disk");
    hdd_new_rb = new QRadioButton("Create new disk");

    error_lbl = new QLabel("");
    okcancel_btn = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    okcancel_btn->button(QDialogButtonBox::Ok)->setDefault(true);
    okcancel_btn->button(QDialogButtonBox::Cancel)->setAutoDefault(true);

    hddsize_spin->setMaximum(50000);
    hddsize_spin->setValue(1024);
    hddsize_slider->setRange(0, 50000);
    hddsize_slider->setValue(hddsize_spin->value());

    imageplace_btn->setFixedWidth(30);
    imageplace_btn->setAutoDefault(true);
    pathtovm_btn->setFixedWidth(30);
    pathtovm_btn->setAutoDefault(true);

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
    main_lay->addStretch(10);
    main_lay->addWidget(error_lbl);
    main_lay->addWidget(okcancel_btn);
}

#if QT_VERSION < 0x050700
template<typename... Args> struct QOverload {
    template<typename C, typename R>
    static constexpr auto of(R(C::*pmf)(Args...))->decltype(pmf) {
        return pmf;
    }
};
#endif

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

    connect(okcancel_btn, &QDialogButtonBox::accepted, this, &CreateVMForm::create_vm);
    connect(okcancel_btn, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(imageplace_btn, SIGNAL(clicked()), this, SLOT(place_disk_or_project()));
}

void CreateVMForm::select_dir()
{
    QString directory_name = QFileDialog::getExistingDirectory(this, "Select directory", default_path);
    if (directory_name != "")
    {
        path_to_vm = directory_name;
        QString path = directory_name + "/" + name_edit->text();
        pathtovm_edit->setText(path);
        if (QDir(path).exists() && name_edit->text() != "")
        {
            show_error_message("Directory with this name already exists");
            name_edit->setFocus();
        }
    }
}

void CreateVMForm::change_path(const QString &name)
{
    if (!pathtovm_edit->text().isEmpty())
    {
        QString new_name = name;
        if (!name.isEmpty() && (name[name.length() - 1].category() == QChar::Punctuation_Other || name[name.length() - 1].category() == QChar::Symbol_Math))
        {
            new_name = name.left(name.length() - 1);
            name_edit->setText(new_name);
            QToolTip::showText(QPoint(pos().x() + name_edit->pos().x(), pos().ry() + name_edit->pos().y()), "Please use next symbols: letter, digit, bracket, -, _", this);
        }

        QString path;
        if (!path_to_vm.isEmpty())
        {
            path = path_to_vm + "/" + new_name;
        }
        else
        {
            path = default_path + "/" + new_name;
        }

        pathtovm_edit->setText(path);
        if (QDir(path).exists() && !name_edit->text().isEmpty())
        {
            show_error_message("Directory with this name already exists");
            name_edit->setFocus();
        }
        else
        {
            show_error_message("");
        }
    }
}

void CreateVMForm::set_visible_widgets_for_new_hdd(bool isVisible)
{
    format_combo->setVisible(isVisible);
    hddsize_spin->setVisible(isVisible);
    format_lbl->setVisible(isVisible);
    hddsize_lbl->setVisible(isVisible);
    hddsize_slider->setVisible(isVisible);
}

void CreateVMForm::set_visible_widgets_for_existed_hdd(bool isVisible)
{
    imageplace_lbl->setVisible(isVisible);
    imageplace_edit->setVisible(isVisible);
    imageplace_btn->setVisible(isVisible);
}

void CreateVMForm::show_error_message(QString error_text)
{
    error_lbl->setStyleSheet("color: rgb(255, 0, 0); font: 12px");
    error_lbl->setText("  " + error_text);
}

void CreateVMForm::hdd_no(bool state)
{
    if (state)
    {
        show_error_message("");
        set_visible_widgets_for_new_hdd(false);
        set_visible_widgets_for_existed_hdd(false);
    }
}

void CreateVMForm::hdd_exist(bool state)
{
    if (state)
    {
        imageplace_edit->clear();
        set_visible_widgets_for_new_hdd(false);
        set_visible_widgets_for_existed_hdd(true);
    }
}

void CreateVMForm::hdd_new(bool state)
{
    if (state)
    {
        show_error_message("");
        imageplace_edit->clear();
        set_visible_widgets_for_new_hdd(true);
        set_visible_widgets_for_existed_hdd(false);
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
    if (name_edit->text().isEmpty())
    {
        show_error_message("Field 'Name' must be filled");
        name_edit->setFocus();
        return;
    }

    if (QDir(pathtovm_edit->text()).exists())
    {
        show_error_message("Directory with this name already exists");
        pathtovm_btn->setFocus();
        return;
    }

    if (imageplace_edit->isVisible() && imageplace_edit->text().isEmpty())
    {
        show_error_message("Field 'Image path' must be filled");
        imageplace_btn->setFocus();
        return;
    }

    VMConfig *configVM = new VMConfig();

    configVM->set_name(name_edit->text());
    configVM->set_dir_path(pathtovm_edit->text());
    configVM->add_image_path(imageplace_edit->text());
      
    emit createVM_new_vm_is_complete(configVM);
    close();
}

