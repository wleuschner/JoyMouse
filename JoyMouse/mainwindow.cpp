#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QX11Info>
#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>
#include "mapdialog.h"
#include "mapaxisdialog.h"
#include "sensivitydialog.h"

#include <X11/extensions/XTest.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/extensions/XTest.h>
#include "joypad.h"
#include "joypaderror.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    xAxisSensivity=1.0;
    yAxisSensivity=1.0;
    axisEdits=0;
    buttonEdits=0;
    joymouses=0;
    joypads=0;

    createMenu();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setToolTip(tr("JoyMouse"));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":Assets/Gamepad.jpg"));
    trayIcon->show();

    createJoystickComboBox();
    createButtonGroupBox();
    createAxisGroupBox();
    createProfileComboBox();
    joymouses = (JoyMouse**)malloc(sizeof(JoyMouse*)*num_joypads);
    if(num_joypads>0)
    {
        for(unsigned int i=0;i<num_joypads;i++)
        {
            joymouses[i] = new JoyMouse(joypads[i],true);
        }
        connect(ui->mapButton,SIGNAL(clicked()),SLOT(showMapDialog()));
        connect(ui->mapAxisButton,SIGNAL(clicked()),SLOT(showMapAxisDialog()));
        connect(ui->buttonBox,SIGNAL(accepted()),SLOT(updateMapping()));
        connect(ui->sensivityButton,SIGNAL(clicked()),SLOT(showSensivityDialog()));
    }
    else
    {
        QMessageBox::information(this,"Information","You have at least one joypad plugged\n"
                                 "before the start to use the program.");
        ui->mapAxisButton->setEnabled(false);
        ui->mapButton->setEnabled(false);
        ui->saveProfileButton->setEnabled(false);
        ui->joypadComboBox->setEnabled(false);
        ui->activatedCheckBox->setEnabled(false);
        ui->profileComboBox->setEnabled(false);
        connect(ui->buttonBox,SIGNAL(accepted()),SLOT(close()));
    }

    connect(ui->actionQuit,SIGNAL(triggered()),SLOT(closeHandler()));
    connect(ui->saveProfileButton,SIGNAL(clicked()),SLOT(saveProfileHandler()));

    connect(ui->profileComboBox,SIGNAL(currentIndexChanged(int)),SLOT(createAxisGroupBox()));
    connect(ui->profileComboBox,SIGNAL(currentIndexChanged(int)),SLOT(createButtonGroupBox()));
    connect(ui->profileComboBox,SIGNAL(currentIndexChanged(int)),SLOT(createButtonAxisGroups()));

    startTimer(10);

}

void MainWindow::createMenu()
{
    QAction* closeAction = new QAction(tr("Close"),this);
    QAction* configureAction = new QAction(tr("Configure"),this);
    trayIconMenu=new QMenu(this);
    trayIconMenu->addAction(configureAction);
    trayIconMenu->addAction(closeAction);

    connect(closeAction,SIGNAL(triggered()),SLOT(closeHandler()));
    connect(configureAction,SIGNAL(triggered()),SLOT(configureHandler()));
}

void MainWindow::createButtonGroupBox()
{
    if(num_joypads>0)
    {
        int currentIndex = ui->joypadComboBox->itemData(ui->joypadComboBox->currentIndex()).toInt();
        Joypad* j = joypads[currentIndex];
        int numButtons = j->getNumButtons();

        if(buttonEdits!=0)
        {
            QLayout *layout = ui->buttonGroupBox->layout();
            while(!layout->isEmpty())
            {
                delete layout->itemAt(0)->layout()->itemAt(0)->widget();
                delete layout->itemAt(0)->layout()->itemAt(0)->widget();
                delete layout->itemAt(0)->layout();
            }
            delete buttonEdits;
        }
        buttonEdits = new QLineEdit*[numButtons];

        QVBoxLayout* vlayout = (QVBoxLayout*)ui->buttonGroupBox->layout();
        for(int i=0;i<numButtons;i++)
        {
            QHBoxLayout *layout = new QHBoxLayout();
            QLabel* label = new QLabel(QString(tr("Button %0:")).arg(i+1));
            buttonEdits[i] = new QLineEdit();
            buttonEdits[i]->setReadOnly(true);
            layout->addWidget(label);
            layout->addWidget(buttonEdits[i]);
            vlayout->addLayout(layout);
            label->show();
            buttonEdits[i]->show();
        }
    }
}

void MainWindow::createAxisGroupBox()
{
    if(num_joypads>0)
    {
        int currentIndex = ui->joypadComboBox->itemData(ui->joypadComboBox->currentIndex()).toInt();
        Joypad* j = joypads[currentIndex];
        int numAxis = j->getNumAxis();

        if(axisEdits!=0)
        {
            QLayout* layout = ui->axisGroupBox->layout();
            while(!layout->isEmpty())
            {
                delete layout->itemAt(0)->layout()->itemAt(0)->widget();
                delete layout->itemAt(0)->layout()->itemAt(0)->widget();
                delete layout->itemAt(0)->layout();
            }
            delete axisEdits;
        }

        axisEdits = new QLineEdit*[numAxis];

        QVBoxLayout* vlayout = (QVBoxLayout*)ui->axisGroupBox->layout();
        for(int i=0;i<numAxis;i++)
        {
            QHBoxLayout *layout = new QHBoxLayout();
            QLabel* label = new QLabel(QString(tr("Axis %0:")).arg(i+1));
            axisEdits[i] = new QLineEdit();
            axisEdits[i]->setReadOnly(true);
            layout->addWidget(label);
            layout->addWidget(axisEdits[i]);
            vlayout->addLayout(layout);
            label->show();
            axisEdits[i]->show();
        }
    }
}

void MainWindow::createJoystickComboBox()
{
    QDir path("/dev/input");
    QStringList files = path.entryList(QStringList("js*"),QDir::System);
    num_joypads = 0;
    if(!files.isEmpty())
    {
        joypads = (Joypad**)malloc(sizeof(Joypad*)*files.length());
        for(int i=0;i<files.length();i++)
        {
            const char* devPath = path.absoluteFilePath(files[i]).toStdString().c_str();
            try
            {
                joypads[i] = new Joypad(devPath);
                ui->joypadComboBox->addItem(joypads[i]->getName(),i);
            }
            catch(JoypadError e)
            {
                QMessageBox::warning(this,"ERROR",QString(e.getJoypadFile().c_str())+QString(":")+QString(e.getErrorMessage().c_str()));
                joypads[i] = 0;
            }
        }
        num_joypads = files.length();
    }
}

void MainWindow::createProfileComboBox()
{
    if(num_joypads>0)
    {
        QString currentJoypad = ui->joypadComboBox->currentText();
        QDir path(".");
        QStringList files = path.entryList(QStringList("*.profile"),QDir::AllEntries);
        for(QStringList::Iterator i = files.begin();i!=files.end();i++)
        {
            QSettings profile(path.absoluteFilePath(*i),QSettings::NativeFormat);
            QString joypad = profile.value("joypad").toString();
            if(joypad==currentJoypad)
            {
                QString name = profile.value("name").toString();
                ui->profileComboBox->addItem(name,path.absoluteFilePath(*i));
            }
        }
        createButtonAxisGroups();
    }
}

void MainWindow::createButtonAxisGroups()
{
    if(num_joypads>0)
    {
        QString currentJoypad = ui->joypadComboBox->currentText();
        QSettings profile(ui->profileComboBox->itemData(ui->profileComboBox->currentIndex()).toString(),QSettings::NativeFormat);
        QString joypad = profile.value("joypad").toString();
        unsigned int numButtons = profile.value("numbuttons").toInt();
        unsigned int numAxis = profile.value("numaxis").toInt();
        if(joypad==currentJoypad)
        {
            for(unsigned int i=0;i<numAxis;i++)
            {
                if(profile.contains(QString("axis%0").arg(i)))
                {
                    QString axe=profile.value(QString("axis%0").arg(i)).toString();
                    axisEdits[i]->setText(axe);
                    if(axe=="XAXIS")
                    {
                        if(profile.contains("xaxis-sens"))
                        {
                            xAxisSensivity=profile.value(QString("xaxis-sens")).toDouble();
                        }
                        else
                        {
                            xAxisSensivity=1.0;
                        }
                    }
                    else if(axe=="YAXIS");
                    {
                        if(profile.contains("yaxis-sens"))
                        {
                            xAxisSensivity=profile.value(QString("yaxis-sens")).toDouble();
                        }
                        else
                        {
                            xAxisSensivity=1.0;
                        }
                    }
                }
            }
            for(unsigned int i=0;i<numButtons;i++)
            {
                if(profile.contains(QString("button%0").arg(i)))
                {
                    buttonEdits[i]->setText(profile.value(QString("button%0").arg(i)).toString());
                }
            }
        }
    }
}

void MainWindow::closeHandler()
{
    trayIcon->setVisible(false);
    this->close();
}

void MainWindow::configureHandler()
{
    this->show();
}

void MainWindow::loadProfileHandler()
{
}

void MainWindow::saveProfileHandler()
{
    Joypad* j = joypads[ui->joypadComboBox->currentIndex()];
    QString name = QInputDialog::getText(this,"Profile Name","Name: ");
    QSettings profile(QString("%0.profile").arg(name),QSettings::NativeFormat);
    profile.clear();

    profile.setValue("name",name);
    profile.setValue("joypad",ui->joypadComboBox->currentText());
    profile.setValue("numbuttons",j->getNumButtons());
    profile.setValue("numaxis",j->getNumAxis());
    profile.setValue("xaxis-sens",xAxisSensivity);
    profile.setValue("yaxis-sens",yAxisSensivity);

    //Save Axismappings
    for(int i=0;i<j->getNumAxis();i++)
    {
        QString t = axisEdits[i]->text();
        if(!t.isEmpty())
        {
            profile.setValue(QString("axis%0").arg(i),t);
        }
    }

    //Save Buttonmappings
    for(int i=0;i<j->getNumButtons();i++)
    {
        QString t = buttonEdits[i]->text();
        if(!t.isEmpty())
        {
            profile.setValue(QString("button%0").arg(i),t);
        }
    }
    QMessageBox::information(this,"Profile","Profile Saved");
}

void MainWindow::updateMapping()
{
    JoyMouse* jm = joymouses[ui->joypadComboBox->currentIndex()];
    jm->setActivated(false);
    jm->clearMappings();
    Joypad *j = joypads[ui->joypadComboBox->currentIndex()];
    if(ui->activatedCheckBox->isChecked())
    {
        jm->setActivated(true);
    }
    else
    {
        jm->setActivated(false);
    }
    for(int i=0;i < j->getNumAxis();i++)
    {
        QString t = axisEdits[i]->text();
        if(t=="XAXIS")
        {
            jm->mapAxis(i,JoyMouse::XAXIS);
            j->setSensivity(i,xAxisSensivity);
        }
        else if(t=="YAXIS")
        {
            jm->mapAxis(i,JoyMouse::YAXIS);
            j->setSensivity(i,yAxisSensivity);
        }
        else if(t=="SCROLLAXIS")
        {
            jm->mapAxis(i,JoyMouse::SCROLLPOSAXIS);
        }
        else if(t=="PROGRAM")
        {
            QStringList l = t.split(":");
            jm->mapAxis(i,l[0].toStdString().c_str());
        }
        else if(!t.isEmpty())
        {
            QStringList l = t.split(":");
            unsigned k1 = stringToKey(l[0]);
            unsigned k2 = stringToKey(l[1]);
            jm->mapAxisKeySym(i,k1,k2);
        }
    }

    for(int i=0;i < j->getNumButtons(); i++)
    {
        QString t = buttonEdits[i]->text();
        if(t=="XAXIS")
        {
            jm->mapButton(i,JoyMouse::XAXIS);
        }
        else if(t=="YAXIS")
        {
            jm->mapButton(i,JoyMouse::YAXIS);
        }
        else if(t=="RIGHTMOUSE")
        {
            jm->mapButton(i,JoyMouse::RIGHTBUTTON);
        }
        else if(t=="LEFTMOUSE")
        {
            jm->mapButton(i,JoyMouse::LEFTBUTTON);
        }
        else if(t=="SCROLLPOSAXIS")
        {
            jm->mapButton(i,JoyMouse::SCROLLPOSAXIS);
        }
        else if(t=="SCROLLNEGAXIS")
        {
            jm->mapButton(i,JoyMouse::SCROLLNEGAXIS);
        }
        else if(t=="PROGRAM")
        {
            QStringList l = t.split(":");
            jm->mapAxis(i,l[0].toStdString().c_str());
        }
        else if(!t.isEmpty())
        {
            unsigned int k = stringToKey(t);
            jm->mapButtonKeySym(i,k);
        }
    }
    jm->setActivated(true);
    this->close();
}

void MainWindow::showMapDialog()
{
    int index = ui->joypadComboBox->currentIndex();
    MapDialog md(joypads[index],joymouses[index]);
    if(md.exec()==QDialog::Accepted)
    {
        int button = md.getButton();
        if(md.getType()==MapDialog::AXIS_MAPPING)
        {
            if(md.getAxis()==JoyMouse::SCROLLNEGAXIS)
            {
                buttonEdits[button]->setText("SCROLLNEGAXIS");
            }
            else if(md.getAxis()==JoyMouse::SCROLLPOSAXIS)
            {
                buttonEdits[button]->setText("SCROLLPOSAXIS");
            }
        }
        else if(md.getType()==MapDialog::KEY_MAPPING)
        {
            buttonEdits[button]->setText(QString(md.getKey()));
        }
        else if(md.getType()==MapDialog::PROGRAM_MAPPING)
        {
            buttonEdits[button]->setText(QString("PROGRAM:%0").arg(md.getProgram()));
        }
    }
}

void MainWindow::showMapAxisDialog()
{
    int index = ui->joypadComboBox->currentIndex();
    MapAxisDialog md(joypads[index],joymouses[index]);
    if(md.exec()==QDialog::Accepted)
    {
        int axis = md.getAxis();
        if(md.getType()==MapAxisDialog::AXIS_MAPPING)
        {
            int type = md.getMouseAxis();
            if(type==JoyMouse::XAXIS)
            {
                axisEdits[axis]->setText("XAXIS");
            }
            else if(type==JoyMouse::YAXIS)
            {
                axisEdits[axis]->setText("YAXIS");
            }
            else if(type==JoyMouse::SCROLLPOSAXIS)
            {
                axisEdits[axis]->setText("SCROLLPOSAXIS");
            }
        }
        else if(md.getType()==MapDialog::KEY_MAPPING)
        {
            QPair<QString,QString> pair = md.getKey();
            axisEdits[axis]->setText(pair.first+":"+pair.second);
        }
        else if(md.getType()==MapDialog::PROGRAM_MAPPING)
        {
            axisEdits[axis]->setText(QString("PROGRAM:%0").arg(md.getProgram()));
        }
    }
}

void MainWindow::showSensivityDialog()
{
    SensivityDialog sd;
    if(sd.exec()==QDialog::Accepted)
    {
        xAxisSensivity=sd.getXSensivity();
        yAxisSensivity=sd.getYSensivity();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }
}

unsigned int MainWindow::stringToKey(QString t)
{
    if(t=="<SPACE>")
    {
        return XK_space;
    }
    else if(t=="<TAB>")
    {
        return XK_Tab;
    }
    else if(t=="<SHIFTL>")
    {
        return XK_Shift_L;
    }
    else if(t=="<SHIFTR>")
    {
        return XK_Shift_R;
    }
    else if(t=="<CAPS>")
    {
        return XK_Caps_Lock;
    }
    else if(t=="<STRGL>")
    {
        return XK_Control_L;
    }
    else if(t=="<STRGR>")
    {
        return XK_Control_R;
    }
    else if(t=="<ALTL>")
    {
        return XK_Alt_L;
    }
    else if(t=="<ALTGR>")
    {
        return XK_Alt_R;
    }
    else if(t=="<ESC>")
    {
        return XK_Escape;
    }
    else if(t=="<LEFT>")
    {
        return XK_Left;
    }
    else if(t=="<RIGHT>")
    {
        return XK_Right;
    }
    else if(t=="<UP>")
    {
        return XK_Up;
    }
    else if(t=="<DOWN>")
    {
        return XK_Down;
    }
    else if(t=="<RETURN>")
    {
        return XK_Return;
    }
    else if(t=="<BACKSPACE>")
    {
        return XK_BackSpace;
    }
    else if(t=="<F1>")
    {
        return XK_F1;
    }
    else if(t=="<F2>")
    {
        return XK_F2;
    }
    else if(t=="<F3>")
    {
        return XK_F3;
    }
    else if(t=="<F4>")
    {
        return XK_F4;
    }
    else if(t=="<F5>")
    {
        return XK_F5;
    }
    else if(t=="<F6>")
    {
        return XK_F6;
    }
    else if(t=="<F7>")
    {
        return XK_F7;
    }
    else if(t=="<F8>")
    {
        return XK_F8;
    }
    else if(t=="<F9>")
    {
        return XK_F9;
    }
    else if(t=="<F10>")
    {
        return XK_F10;
    }
    else if(t=="<F11>")
    {
        return XK_F11;
    }
    else if(t=="<F12>")
    {
        return XK_F12;
    }
    else if(t=="<POS>")
    {
        return XK_Home;
    }
    else if(t=="<END>")
    {
        return XK_End;
    }
    else if(t=="<INS>")
    {
        return XK_Insert;
    }
    else if(t=="<PAGEUP>")
    {
        return XK_Page_Up;
    }
    else if(t=="<PAGEDOWN>")
    {
        return XK_Page_Down;
    }
    else if(t=="<PRINT>")
    {
        return XK_Print;
    }
    else if(t=="<NUMLOCK>")
    {
        return XK_Num_Lock;
    }
    else if(t=="<SUPERL>")
    {
        return XK_Super_L;
    }
    else if(t=="<SUPERR>")
    {
        return XK_Super_R;
    }
    else if(t=="<VOLUP>")
    {
        return XF86XK_AudioRaiseVolume;
    }
    else if(t=="<VOLDOWN>")
    {
        return XF86XK_AudioLowerVolume;
    }
    else if(t=="<MUTE>")
    {
        return XF86XK_AudioMute;
    }
    else if(t=="<NEXT>")
    {
        return XF86XK_AudioNext;
    }
    else if(t=="<PREV>")
    {
        return XF86XK_AudioPrev;
    }
    else if(t=="<PLAY>")
    {
        return XF86XK_AudioPlay;
    }
    else if(t=="<STOP>")
    {
        return XF86XK_AudioStop;
    }
    else if(t=="<MEDIAPLAYER>")
    {
        return XF86XK_Music;
    }
    return XStringToKeysym(t.toStdString().c_str());
}

QString MainWindow::keyToString(unsigned int key)
{
    return XKeysymToString(key);
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    for(unsigned int i=0;i<num_joypads;i++)
    {
        if(joymouses[i]->isActivated())
        {
            try
            {
                joymouses[i]->poll();
            }
            catch(JoypadError er)
            {
                joymouses[i]->setActivated(false);
                QMessageBox::warning(this, "Error",QString(er.getJoypadFile().c_str())+QString(":")+QString(er.getErrorMessage().c_str()));
            }
        }
    }
}

MainWindow::~MainWindow()
{
    for(unsigned int i=0;i<num_joypads;i++)
    {
        delete joymouses[i];
        delete joypads[i];
    }
    delete ui;
}
