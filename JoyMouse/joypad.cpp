#include "joypad.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <libexplain/ioctl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include "joypaderror.h"

Joypad::Joypad(const char* device)
{
    joystic_fd = open(device,O_RDONLY|O_NONBLOCK);
    if(joystic_fd < 0)
    {
        throw "Error";
    }
    if(ioctl(joystic_fd,JSIOCGNAME(80),name)<0)
    {
        throw JoypadError(device,"ERROR: Could not get Joypad Name");
    }
    if(ioctl(joystic_fd,JSIOCGAXES,&numAxis)<0)
    {
        throw JoypadError(device,"ERROR: Could not get number of buttons");
    }
    if(ioctl(joystic_fd,JSIOCGBUTTONS,&numButtons)<0)
    {
        throw JoypadError(device,"ERROR: Could not get number of axis");
    }
    //fcntl(joystic_fd,O_RDONLY|O_NONBLOCK);
    axis = (AXIS*)malloc(sizeof(AXIS)*numAxis);
    buttons = (int*)malloc(sizeof(int)*numButtons);

    for(int i=0;i<numAxis;i++)
    {
        axis[i].sensivity=1.0f;
    }
}

Joypad::~Joypad()
{
    close(joystic_fd);
    free(axis);
    free(buttons);
}

int Joypad::getNumAxis()
{
    return numAxis;
}

int Joypad::getNumButtons()
{
    return numButtons;
}

int Joypad::getAxis(int axe)
{
    return axis[axe].value*axis[axe].sensivity;
}

int Joypad::getButton(int button)
{
    return buttons[button];
}

const char* Joypad::getName()
{
    return name;
}

void Joypad::setSensivity(int axe,double sensivity)
{
    axis[axe].sensivity = sensivity;
}

int Joypad::getSensivity(int axe)
{
    return axis[axe].sensivity;
}

bool Joypad::poll(Joypad::JOY_EVENT* event)
{
    static struct js_event js;
    if(read(joystic_fd,&js,sizeof(struct js_event))<0)
    {
        if(errno==EIO || errno==EFAULT || errno == EBADF)
        {
            throw JoypadError(name,"Could not poll joypad. Maybe it is disconnected?");
        }
    }
    switch(js.type & ~JS_EVENT_INIT)
    {
    case JS_EVENT_AXIS:
        if(axis[js.number].value!=js.value)
        {
            axis[js.number].value=js.value;
            event->type=Joypad::AXIS_MOVED;
            event->value=js.value;
            event->axis=js.number;
            return true;
        }
        break;
    case JS_EVENT_BUTTON:
        if(buttons[js.number]!=js.value)
        {
            if(buttons[js.number]==0)
            {
                event->type=Joypad::BUTTON_DOWN;
            }
            else
            {
                event->type=Joypad::BUTTON_UP;
            }
            buttons[js.number]=js.value;
            event->value=js.value;
            event->axis=js.number;
            return true;
        }
        break;
    }
    return false;
}
