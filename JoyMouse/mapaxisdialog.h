#ifndef MAPAXISDIALOG_H
#define MAPAXISDIALOG_H

#include <QDialog>
#include "joypad.h"
#include "joymouse.h"

namespace Ui {
class MapAxisDialog;
}

class MapAxisDialog : public QDialog
{
    Q_OBJECT

public:
    enum
    {
        AXIS_MAPPING,
        KEY_MAPPING,
        PROGRAM_MAPPING
    };

    explicit MapAxisDialog(Joypad* j,JoyMouse* jm,QWidget *parent = 0);
    MapAxisDialog(Joypad* j,JoyMouse* jm,int axis,QWidget *parent = 0);
    ~MapAxisDialog();

    int getAxis();
    int getType();
    QString getProgram();
    QPair<QString,QString> getKey();
    int getMouseAxis();

protected:
    void timerEvent(QTimerEvent* e);

private slots:
    void mouseRadioToggled();
    void keyRadioToggled();
    void programRadioToggled();

private:
    int axis;
    Joypad *joypad;
    JoyMouse *joymouse;
    Ui::MapAxisDialog *ui;
};

#endif // MAPAXISDIALOG_H
