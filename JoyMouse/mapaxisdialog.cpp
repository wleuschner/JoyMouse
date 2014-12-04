#include "mapaxisdialog.h"
#include "ui_mapaxisdialog.h"

MapAxisDialog::MapAxisDialog(Joypad* j,JoyMouse* jm,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapAxisDialog)
{
    ui->setupUi(this);
    this->joypad=j;
    this->joymouse=jm;
    keyRadioToggled();
    connect(ui->MouseRadio_2,SIGNAL(clicked()),SLOT(mouseRadioToggled()));
    connect(ui->keyRadio_2,SIGNAL(clicked()),SLOT(keyRadioToggled()));
    connect(ui->ProgramRadio_2,SIGNAL(clicked()),SLOT(programRadioToggled()));
    startTimer(10);
}

MapAxisDialog::MapAxisDialog(Joypad* j,JoyMouse* jm,int axis,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapAxisDialog)
{
    ui->setupUi(this);
    this->joypad=j;
    this->joymouse=jm;
    keyRadioToggled();
    connect(ui->MouseRadio_2,SIGNAL(clicked()),SLOT(mouseRadioToggled()));
    connect(ui->keyRadio_2,SIGNAL(clicked()),SLOT(keyRadioToggled()));
    connect(ui->ProgramRadio_2,SIGNAL(clicked()),SLOT(programRadioToggled()));
    this->axis=axis;
    ui->axisEdit->setText(QString("Axis %0").arg(axis+1));
}

MapAxisDialog::~MapAxisDialog()
{
    delete ui;
}

int MapAxisDialog::getAxis()
{
    return axis;
}

int MapAxisDialog::getType()
{
    if(ui->MouseRadio_2->isChecked())
    {
        return AXIS_MAPPING;
    }
    else if(ui->keyRadio_2->isChecked())
    {
        return KEY_MAPPING;
    }
    else if(ui->ProgramRadio_2->isChecked())
    {
        return PROGRAM_MAPPING;
    }
    return -1;
}

QString MapAxisDialog::getProgram()
{
    return ui->negativeEdit->text();
}

QPair<QString,QString> MapAxisDialog::getKey()
{
    QPair<QString,QString> pair;
    pair.first=ui->negativeEdit->text();
    pair.second=ui->positiveEdit->text();
    return pair;
}

int MapAxisDialog::getMouseAxis()
{
    int index = ui->axisComboBox_2->currentIndex();
    switch(index)
    {
    case 0:
        return JoyMouse::XAXIS;
    case 1:
        return JoyMouse::YAXIS;
    case 2:
        return JoyMouse::SCROLLNEGAXIS;
    case 3:
        return JoyMouse::SCROLLPOSAXIS;
    }

    return -1;
}


void MapAxisDialog::mouseRadioToggled()
{
    QStringList items({tr("X-Axis"),tr("Y-Axis"),tr("Negative Scroll"),tr("Positive Scroll")});
    ui->axisComboBox_2->clear();
    ui->axisComboBox_2->addItems(items);
    ui->axisComboBox_2->setVisible(true);
    ui->positiveLabel->setVisible(false);
    ui->negativeLabel->setVisible(false);
    ui->positiveEdit->setVisible(false);
    ui->negativeEdit->setVisible(false);
}

void MapAxisDialog::keyRadioToggled()
{
    QStringList items({tr("None"),tr("Volume UP"),tr("Volume Down"),tr("Mute"),tr("Next Song"),tr("Previos song"),tr("Play"),tr("Stop")});
    ui->axisComboBox_2->clear();
    ui->axisComboBox_2->addItems(items);
    ui->axisComboBox_2->setVisible(true);
    ui->positiveLabel->setVisible(true);
    ui->negativeLabel->setVisible(true);
    ui->positiveEdit->setVisible(true);
    ui->negativeEdit->setVisible(true);}

void MapAxisDialog::programRadioToggled()
{
    ui->axisComboBox_2->setVisible(false);
    ui->positiveLabel->setVisible(false);
    ui->negativeLabel->setVisible(true);
    ui->positiveEdit->setVisible(false);
    ui->negativeEdit->setVisible(true);
}

void MapAxisDialog::timerEvent(QTimerEvent* e)
{
    Joypad::JOY_EVENT event;
    if(joypad->poll(&event))
    {
        switch(event.type)
        {
        case Joypad::AXIS_MOVED:
            axis=event.axis;
            ui->axisEdit->setText(QString("Axis %0").arg(event.axis+1));
            break;
        }
    }
}
