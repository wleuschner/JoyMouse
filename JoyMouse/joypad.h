#ifndef JOYPAD_H
#define JOYPAD_H
#include<linux/input.h>

class Joypad
{
public:
    enum
    {
        BUTTON_DOWN,
        BUTTON_UP,
        AXIS_MOVED,
    };
    struct JOY_EVENT
    {
        int type;
        union
        {
            int button;
            int axis;
        };
        int value;
    };

    struct AXIS
    {
        int value;
        double sensivity;
    };

    Joypad(const char* device);
    ~Joypad();
    int getButton(int button);
    int getAxis(int axe);
    int getNumAxis();
    int getNumButtons();
    const char* getName();
    void setSensivity(int axe,double sensivity);
    int getSensivity(int axe);
    bool poll(JOY_EVENT *event);
private:
    int joystic_fd;

    char name[80];
    int *buttons;
    AXIS *axis;
    int numButtons;
    int numAxis;
};

#endif // JOYPAD_H
