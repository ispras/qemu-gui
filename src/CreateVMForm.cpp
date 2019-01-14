#include "QemuGUICommon.h"
#include "CreateVMForm.h"


CreateVMForm::CreateVMForm(const QString &home_dir, const QString &qemu_dir)
    : QWidget()
{
    if (CreateVMForm::objectName().isEmpty())
        CreateVMForm::setObjectName(QStringLiteral("CreateVMForm"));
    resize(400, 460);
    setWindowTitle(QApplication::translate("CreateVMForm", "Create Virtual Machine", Q_NULLPTR));
    setWindowIcon(QIcon(":Resources/qemu.png"));
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    default_path = home_dir;
    this->qemu_dir = qemu_dir;

    QStringList format = { "qcow2", "qcow", "cow", "raw" }; // "vmdk", "cloop", "VPC(VHD)?"};
    QStringList os_type = { "Windows", "Linux", "Ubuntu", "MacOS", "Other" };
    QStringList machines = { "pc-i440fx-2.13", "pc-1.3", "pc-q35-2.9" };
    QStringList cpus = { "486", "qemu32", "qemu64" };

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
    hddsize_lbl = new QLabel("Size (Mb)");

    machineCombo = new QComboBox(this);
    cpuCombo = new QComboBox(this);

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

    machineCombo->setFixedWidth(330);
    machineCombo->addItems(machines);
    cpuCombo->setFixedWidth(330);
    cpuCombo->addItems(cpus);

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

    QGroupBox *systemGr = new QGroupBox("System information");
    systemGr->setMinimumWidth(width());

    QVBoxLayout *systemLay = new QVBoxLayout();
    QHBoxLayout *machineLay = new QHBoxLayout();
    machineLay->addWidget(new QLabel("Machine"));
    machineLay->addWidget(machineCombo);
    QHBoxLayout *cpuLay = new QHBoxLayout();
    cpuLay->addWidget(new QLabel("CPU"));
    cpuLay->addWidget(cpuCombo);
    systemLay->addLayout(machineLay);
    systemLay->addLayout(cpuLay);

    systemGr->setLayout(systemLay);

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
    main_lay->addWidget(systemGr);
    main_lay->addWidget(memory_gr);
    main_lay->addWidget(hdd_gr);
    main_lay->addStretch(10);
    main_lay->addWidget(error_lbl);
    main_lay->addWidget(okcancel_btn);
}

void CreateVMForm::connect_signals()
{
    connect(pathtovm_btn, SIGNAL(clicked()), this, SLOT(select_dir()));
    connect(name_edit, SIGNAL(textEdited(const QString &)),
        this, SLOT(change_path(const QString &)));

    connect(ram_slider, &QSlider::valueChanged, ram_spin, &QSpinBox::setValue);
    connect(ram_spin, QOverload<int>::of(&QSpinBox::valueChanged),
        ram_slider, &QSlider::setValue);

    connect(hddsize_slider, &QSlider::valueChanged, hddsize_spin, &QSpinBox::setValue);
    connect(hddsize_spin, QOverload<int>::of(&QSpinBox::valueChanged),
        hddsize_slider, &QSlider::setValue);

    connect(hdd_no_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_no);
    connect(hdd_exist_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_exist);
    connect(hdd_new_rb, &QRadioButton::toggled, this, &CreateVMForm::hdd_new);

    connect(okcancel_btn, &QDialogButtonBox::accepted, this, &CreateVMForm::create_vm);
    connect(okcancel_btn, &QDialogButtonBox::rejected, this, &QWidget::close);

    connect(imageplace_btn, SIGNAL(clicked()), this, SLOT(place_disk()));
}

void CreateVMForm::select_dir()
{
    QString directory_name = QFileDialog::getExistingDirectory(this,
        "Select directory", default_path);
    if (directory_name != "")
    {
        path_to_vm = directory_name;
        QString path = set_path_to_vm(path_to_vm);
        input_verification(path, "");
    }
}

void CreateVMForm::change_path(const QString &name)
{
    input_verification("", name);
    set_path_to_vm(path_to_vm);
}

QString CreateVMForm::set_path_to_vm(const QString & home_path)
{
    QString path;
    if (!home_path.isEmpty())
    {
        path = home_path + "/" + name_edit->text();
    }
    else
    {
        path = default_path + "/" + name_edit->text();
    }
    pathtovm_edit->setText(path);
    return path;
}

bool CreateVMForm::input_verification(const QString &path, const QString &name)
{
    QString vm_path = path;

    if (!name.isEmpty())
    {
        QString new_name = name;

        for (int i = 0; i < new_name.length();)
        {
            if (new_name[i].category() == QChar::Punctuation_Other || 
                new_name[i].category() == QChar::Symbol_Math)
            {
                new_name = new_name.left(i) + new_name.right(new_name.length() - i - 1);
                name_edit->setText(new_name);
                name_edit->setCursorPosition(i);
                QToolTip::showText(QPoint(pos().x() + name_edit->pos().x(), 
                    pos().y() + name_edit->pos().y()),
                    "Please use next symbols: letter, digit, bracket, -, _", this);
            }
            else
                i++;
        }
        return true;
    }

    if (!vm_path.isEmpty())
    {
        if (QDir(vm_path).exists() && !name_edit->text().isEmpty())
        {
            show_error_message("Virtual machine with this name already exists");
            return false;
        }
        else
        {
            show_error_message("");
            return true;
        }
    }

    if (name_edit->text().isEmpty())
    {
        show_error_message("Field 'Name' must be filled");
        return false;
    }

    if (QDir(pathtovm_edit->text()).exists())
    {
        show_error_message("Virtual machine with this name already exists");
        return false;
    }

    if (imageplace_edit->isVisible() && imageplace_edit->text().isEmpty())
    {
        show_error_message("Field 'Image path' must be filled");
        return false;
    }
    return true;
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

void CreateVMForm::show_error_message(const QString &error_text)
{
    error_lbl->setStyleSheet("color: rgb(255, 0, 0); font: 12px");
    error_lbl->setText("  " + error_text);
}

void CreateVMForm::hdd_no(bool state)
{
    if (state)
    {
        show_error_message("");
        imageplace_edit->clear();
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
    if (state && QString().compare(qemu_dir, "") != 0)
    {
        show_error_message("");
        imageplace_edit->clear();
        set_visible_widgets_for_new_hdd(true);
        set_visible_widgets_for_existed_hdd(false);
    }
    else if (state)
    {
        QMessageBox::critical(this, "Error",
            "Didn\'t select Qemu installation directory");
        hdd_no_rb->setChecked(true);
    }
}

void CreateVMForm::place_disk()
{
    if (hdd_exist_rb->isChecked())
    {
        QString filename = QFileDialog::getOpenFileName(this, "Select image", 
            default_path, "*.qcow *.qcow2 *.vdi *.img *.raw"); // and other
        if (filename != "")
        {
            imageplace_edit->setText(filename);
        }
    }
}

void CreateVMForm::create_vm()
{
    if (!input_verification("", ""))
        return;

    configVM = new VMConfig(nullptr, pathtovm_edit->text());

    configVM->set_name(name_edit->text());
    configVM->addDeviceCpu(cpuCombo->currentText());
    configVM->addDeviceMachine(machineCombo->currentText());
    configVM->addDeviceMemory(QString::number(ram_spin->value()));

    if (!hdd_new_rb->isChecked())
    {
        configVM->addDefaultBus(imageplace_edit->text());
        emit createVM_new_vm_is_complete(configVM);
        close();
    }
    else
    {
        configVM->addDefaultBus(pathtovm_edit->text() + "/" + 
            name_edit->text() + "." + format_combo->currentText());
        configVM->createVMFolder(pathtovm_edit->text());
        setVisible(false);
        imgCreationDlg = new QProgressDialog("Creating the image...", "", 0, 0);
        imgCreationDlg->setCancelButton(nullptr);
        imgCreationDlg->setWindowTitle("Please Wait");
        imgCreationDlg->setRange(0, 0);
        imgCreationDlg->show();

        QString imageName = pathtovm_edit->text() + "/" + name_edit->text() + 
            "." + format_combo->currentText();

        QThread *thread = new QThread();
        QemuImgLauncher *imgLauncher = new QemuImgLauncher(qemu_dir, 
            format_combo->currentText(), imageName, hddsize_spin->value());

        imgLauncher->moveToThread(thread);
        connect(thread, SIGNAL(started()), imgLauncher, SLOT(startQemuImgCreateDisk()));
        connect(imgLauncher, SIGNAL(qemu_img_finished(int)), 
            this, SLOT(finish_qemu_img(int)));
        thread->start();
    }
}

void CreateVMForm::finish_qemu_img(int exitCode)
{
    if (exitCode == 0)
    {
        imgCreationDlg->setMaximum(1);
        imgCreationDlg->setValue(1);
        emit createVM_new_vm_is_complete(configVM);
        close();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Image wasn't created. Why? I don't know");
        configVM->remove_directory_vm();
        imgCreationDlg->close();
        delete configVM;
    }
}




