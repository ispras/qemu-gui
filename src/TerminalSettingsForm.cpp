#include "TerminalSettingsForm.h"

TerminalSettingsForm::TerminalSettingsForm(QTextEdit *terminal, QWidget *parent)
    : QWidget(parent)
{
    if (TerminalSettingsForm::objectName().isEmpty())
        TerminalSettingsForm::setObjectName(QStringLiteral("TerminalSettings"));
    resize(484, 214);
    
    setWindowTitle(QApplication::translate("TerminalSettings", "Terminal interface settings", Q_NULLPTR));
    setWindowIcon(QIcon(":Resources/qemu.png"));
    //setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);


    QPushButton *color_background_btn = new QPushButton("Select color", this);
    QPushButton *color_text_btn = new QPushButton("Select color", this);
    QFontComboBox *font_combo = new QFontComboBox(this);
    QSpinBox *font_size_spin = new QSpinBox(this);
    test_text = new QTextEdit(this);
    QDialogButtonBox *savecancel_btn = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);

    QGroupBox *font_group = new QGroupBox("Text settings", this);
    QGroupBox *background_group = new QGroupBox("Background settings", this);

    QHBoxLayout *font_lay = new QHBoxLayout();
    font_lay->addWidget(font_combo);
    font_lay->addWidget(font_size_spin);

    QVBoxLayout *text_lay = new QVBoxLayout();
    text_lay->addLayout(font_lay);
    text_lay->addWidget(color_text_btn);

    font_group->setLayout(text_lay);

    QVBoxLayout *back_lay = new QVBoxLayout();
    back_lay->addWidget(color_background_btn);

    background_group->setLayout(back_lay);

    QVBoxLayout *btns_lay = new QVBoxLayout();
    btns_lay->addWidget(font_group);
    btns_lay->addWidget(background_group);
    btns_lay->addStretch(20);
    btns_lay->addWidget(savecancel_btn);

    QHBoxLayout *main_lay = new QHBoxLayout(this);
    main_lay->addLayout(btns_lay);
    main_lay->addWidget(test_text);

    font_combo->setFontFilters(QFontComboBox::FontFilter::MonospacedFonts);
    font_combo->setCurrentText(terminal->fontFamily());
    font_size_spin->setValue(terminal->fontPointSize());
    font_size_spin->setMinimum(2);
    // current values
    test_text->setFontPointSize(terminal->fontPointSize());
    test_text->setFontFamily(terminal->fontFamily());
    test_text->setStyleSheet(terminal->styleSheet());
    test_text->setTextColor(terminal->textColor());
    test_text->setText("Hello, world!");

    show();

    connect(color_background_btn, SIGNAL(clicked()), this, SLOT(set_background_color()));
    connect(color_text_btn, SIGNAL(clicked()), this, SLOT(set_text_color()));
    connect(font_combo, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(set_test_font(const QFont &)));
    connect(font_size_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &TerminalSettingsForm::set_text_size);

    connect(savecancel_btn, &QDialogButtonBox::accepted, this, &TerminalSettingsForm::save_terminal_interface_changes);
    connect(savecancel_btn, &QDialogButtonBox::rejected, this, &QWidget::close);
}

TerminalSettingsForm::~TerminalSettingsForm()
{
}


void TerminalSettingsForm::closeEvent(QCloseEvent *event)
{
    emit terminal_settings_form_close();
}

void TerminalSettingsForm::set_background_color()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid())
    {
        test_text->setStyleSheet("background-color: " + color.name() + "; border: 1px");
    }
}

void TerminalSettingsForm::set_text_color()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid())
    {
        test_text->setTextColor(color);
        test_text->setText("Hello, world!");
    }
}

void TerminalSettingsForm::set_text_size(int size)
{
    test_text->setFontPointSize(size);
    test_text->setText("Hello, world!");
}

void TerminalSettingsForm::set_test_font(const QFont &font)
{
    test_text->setFontFamily(font.family());
    test_text->setText("Hello, world!");
}

void TerminalSettingsForm::save_terminal_interface_changes()
{
    emit save_terminal_settings(test_text);
    close();
}


