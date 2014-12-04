#include "sensivitydialog.h"
#include "ui_sensivitydialog.h"

SensivityDialog::SensivityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensivityDialog)
{
    ui->setupUi(this);
}

SensivityDialog::~SensivityDialog()
{
    delete ui;
}

double SensivityDialog::getXSensivity()
{
    return ui->xAxisSpinBox->value();
}

double SensivityDialog::getYSensivity()
{
    return ui->yAxisSpinBox->value();
}
