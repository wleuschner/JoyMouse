#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QDialog>
#include "joymouse.h"
#include "joypad.h"

namespace Ui {
class MapDialog;
}

class MapDialog : public QDialog
{
    Q_OBJECT

public:
    enum
    {
        AXIS_MAPPING,
        KEY_MAPPING,
        PROGRAM_MAPPING
    };
    explicit MapDialog(Joypad* j,JoyMouse* jm,QWidget *parent = 0);
    MapDialog(Joypad* j,JoyMouse* jm,int button,QWidget *parent = 0);

    ~MapDialog();

    int getButton();
    int getType();
    QString getProgram();
    QString getKey();
    int getAxis();

protected:
    void timerEvent(QTimerEvent* e);

private slots:
    void mouseRadioToggled();
    void keyRadioToggled();
    void programRadioToggled();

private:
    int currentButton;
    Joypad *joypad;
    JoyMouse *joymouse;
    Ui::MapDialog *ui;
};

#endif // MAPDIALOG_H
