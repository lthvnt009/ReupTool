// Vị trí: /src/dialogs/BatchEditTagsDialog.cpp
// Phiên bản: 1.0 (Mới)

#include "batchedittagsdialog.h"
#include "ui_batchedittagsdialog.h"

BatchEditTagsDialog::BatchEditTagsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchEditTagsDialog)
{
    ui->setupUi(this);
    connect(ui->actionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BatchEditTagsDialog::onActionChanged);
    onActionChanged(0); // Khởi tạo trạng thái ban đầu
}

BatchEditTagsDialog::~BatchEditTagsDialog()
{
    delete ui;
}

BatchEditTagsDialog::Action BatchEditTagsDialog::getAction() const
{
    return static_cast<Action>(ui->actionComboBox->currentIndex());
}

QString BatchEditTagsDialog::getTag1() const
{
    return ui->tag1LineEdit->text();
}

QString BatchEditTagsDialog::getTag2() const
{
    return ui->tag2LineEdit->text();
}

void BatchEditTagsDialog::onActionChanged(int index)
{
    bool isReplace = (static_cast<Action>(index) == Replace);
    ui->tag2Label->setVisible(isReplace);
    ui->tag2LineEdit->setVisible(isReplace);
}
