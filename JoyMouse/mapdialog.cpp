#include "mapdialog.h"
#include "ui_mapdialog.h"

MapDialog::MapDialog(Joypad* j,JoyMouse* jm,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);
    this->joymouse=jm;
    this->joypad=j;
    connect(ui->MouseRadio,SIGNAL(clicked()),SLOT(mouseRadioToggled()));
    connect(ui->keyRadio,SIGNAL(clicked()),SLOT(keyRadioToggled()));
    connect(ui->ProgramRadio,SIGNAL(clicked()),SLOT(programRadioToggled()));
    keyRadioToggled();

    startTimer(10);
}

MapDialog::MapDialog(Joypad* j,JoyMouse* jm,int button,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);
    this->joymouse=jm;
    this->joypad=j;
    connect(ui->MouseRadio,SIGNAL(clicked()),SLOT(mouseRadioToggled()));
    connect(ui->keyRadio,SIGNAL(clicked()),SLOT(keyRadioToggled()));
    connect(ui->ProgramRadio,SIGNAL(clicked()),SLOT(programRadioToggled()));
    keyRadioToggled();
    ui->buttonEdit->setText(QString("Button %0").arg(button+1));
    currentButton=button;
}

int MapDialog::getButton()
{
    return currentButton;
}

int MapDialog::getType()
{
    if(ui->MouseRadio->isChecked())
    {
        return AXIS_MAPPING;
    }
    else if(ui->keyRadio->isChecked())
    {
        return KEY_MAPPING;
    }
    else if(ui->ProgramRadio->isChecked())
    {
        return PROGRAM_MAPPING;
    }
    return -1;
}

QString MapDialog::getProgram()
{
    return ui->keyProgramLineEdit->text();
}

QString MapDialog::getKey()
{
    QStringList items({tr("None"),tr("Volume UP"),tr("Volume Down"),tr("Mute"),tr("Next Song"),tr("Previos song"),tr("Play"),tr("Stop")});
    int index = ui->axisComboBox->currentIndex();
    if(index==0)
    {
        return ui->keyProgramLineEdit->text();
    }
    else
    {
        //TODO Handle special keys;
        switch(index)
        {
        case 1:
            return "<VOLUP>";
        case 2:
            return "<VOLDOWN>";
        case 3:
            return "<MUTE>";
        case 4:
            return "<NEXT>";
        case 5:
            return "<PREV>";
        case 6:
            return "<PLAY>";
        case 7:
            return "<STOP>";
        }
    }
    return "";
}

int MapDialog::getAxis()
{
    int index = ui->axisComboBox->currentIndex();
    switch(index)
    {
    case 0:
        return JoyMouse::SCROLLNEGAXIS;
    case 1:
        return JoyMouse::SCROLLPOSAXIS;
    }
    return -1;
}

void MapDialog::mouseRadioToggled()
{
    QStringList items({tr("Negative Scroll"),tr("Positive Scroll")});
    ui->axisComboBox->clear();
    ui->axisComboBox->addItems(items);
    ui->axisComboBox->setVisible(true);
    ui->keyProgramLineEdit->setVisible(false);
}

void MapDialog::keyRadioToggled()
{
    QStringList items({tr("None"),tr("Volume UP"),tr("Volume Down"),tr("Mute"),tr("Next Song"),tr("Previos song"),tr("Play"),tr("Stop")});
    ui->axisComboBox->clear();
    ui->axisComboBox->addItems(items);
    ui->axisComboBox->setVisible(true);
    ui->keyProgramLineEdit->setVisible(true);
}

void MapDialog::programRadioToggled()
{
    ui->axisComboBox->setVisible(false);
    ui->keyProgramLineEdit->setVisible(true);
}

void MapDialog::timerEvent(QTimerEvent* e)
{
    Joypad::JOY_EVENT event;
    if(joypad->poll(&event))
    {
        switch(event.type)
        {
        case Joypad::BUTTON_UP:
            currentButton=event.button;
            ui->buttonEdit->setText(QString("Button %0").arg(event.button+1));
            break;
        }
    }
}

MapDialog::~MapDialog()
{
    delete ui;
}
