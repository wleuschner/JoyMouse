#include "joymouse.h"
#include<QX11Info>
#include<map>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/keysym.h>
#include<X11/extensions/XTest.h>
#include<X11/extensions/XInput.h>

JoyMouse::JoyMouse(Joypad* jp,bool activated)
{
    this->activated=activated;
    this->jp=jp;
    this->dpy=QX11Info::display();
    this->lastAxisDirections = (int*)malloc(sizeof(int)*jp->getNumAxis());
    memset(this->lastAxisDirections,0,sizeof(int)*jp->getNumAxis());
}

JoyMouse::~JoyMouse()
{
    free(lastAxisDirections);
    buttons.clear();
    axis.clear();
}

void JoyMouse::poll()
{
    Joypad::JOY_EVENT event;
    if(jp->poll(&event))
    {
        switch(event.type)
        {
        case Joypad::BUTTON_DOWN:
            if(buttons.find(event.button)!=buttons.end())
            {
                map_st *m = buttons[event.button];
                switch (m->type) {
                case LEFTBUTTON:
                    createMouseClick(LEFT,True);
                    break;
                case RIGHTBUTTON:
                    createMouseClick(RIGHT,True);
                    break;
                case MIDDLEBUTTON:
                    createMouseClick(MIDDLE,True);
                    break;
                case SCROLLPOSAXIS:
                    createMouseClick(4,True);
                    break;
                case SCROLLNEGAXIS:
                    createMouseClick(5,True);
                    break;
                case KEY:
                    createButtonPress(m->keyPos,True);
                    break;
                case PROGRAM:
                    system(m->command);
                }
            }
            break;
        case Joypad::BUTTON_UP:
            if(buttons.find(event.button)!=buttons.end())
            {
                map_st *m = buttons[event.button];
                switch (m->type) {
                case LEFTBUTTON:
                    createMouseClick(LEFT,False);
                    break;
                case RIGHTBUTTON:
                    createMouseClick(RIGHT,False);
                    break;
                case MIDDLEBUTTON:
                    createMouseClick(MIDDLE,False);
                    break;
                case SCROLLPOSAXIS:
                    createMouseClick(4,False);
                    break;
                case SCROLLNEGAXIS:
                    createMouseClick(5,False);
                    break;
                case KEY:
                    createButtonPress(m->keyPos,False);
                    break;
                }
            }
            break;
        }
    }

    int x=0;
    int y=0;
    for(std::map<int,map_st*>::iterator i = axis.begin();i!=axis.end();i++)
    {
        map_st* m = i->second;
        switch(m->type)
        {
        case XAXIS:
            x=8*jp->getAxis(m->axis)/32764;
            break;
        case YAXIS:
            y=8*jp->getAxis(m->axis)/32764;
            break;
        case SCROLLNEGAXIS:
            if(jp->getAxis(m->axis)>16382)
            {
                createMouseClick(4,True);
                createMouseClick(4,False);
            }
            else if(jp->getAxis(m->axis)<-16382)
            {
                createMouseClick(5,True);
                createMouseClick(5,False);
            }
            break;
        case SCROLLPOSAXIS:
            break;
        case KEY:
            if(jp->getAxis(m->axis)>16382)
            {
                if(lastAxisDirections[m->axis]==-1)
                {
                    createButtonPress(m->keyNeg,FALSE);
                }
                createButtonPress(m->keyPos,TRUE);
                lastAxisDirections[m->axis]=1;
            }
            else if(jp->getAxis(m->axis)<-16382)
            {
                if(lastAxisDirections[m->axis]==1)
                {
                    createButtonPress(m->keyPos,FALSE);
                }
                createButtonPress(m->keyNeg,TRUE);
                lastAxisDirections[m->axis]=-1;
            }
            else
            {
                if(lastAxisDirections[m->axis]==1)
                {
                    createButtonPress(m->keyPos,FALSE);
                }
                if(lastAxisDirections[m->axis]==-1)
                {
                    createButtonPress(m->keyNeg,FALSE);
                }
                lastAxisDirections[m->axis]=0;
            }
            break;
        case PROGRAM:
            system(m->command);
            break;
        }
    }
    if(x!=0 || y!=0)
    {
        createMouseMovement(x,y);
    }
}

void JoyMouse::setActivated(bool activated)
{
    this->activated=activated;
}

bool JoyMouse::isActivated()
{
    return activated;
}

void JoyMouse::clearMappings()
{
    buttons.clear();
    axis.clear();
}

void JoyMouse::mapAxisKeySym(int axis,unsigned int ksNeg,unsigned int ksPos)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->axis=axis;
    m->type=KEY;
    m->keyPos=XKeysymToKeycode(dpy,ksNeg);
    m->keyNeg=XKeysymToKeycode(dpy,ksPos);
    this->axis[axis] = m;
}

void JoyMouse::mapAxis(int axis,int type)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->axis=axis;
    m->type=type;
    this->axis[axis] = m;
}

void JoyMouse::mapAxis(int axis,const char* path)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->axis=axis;
    m->type=PROGRAM;
    m->command=path;
    this->axis[axis] = m;
}

void JoyMouse::mapAxisKey(int axis,const char* keyNeg,const char* keyPos)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->axis=axis;
    m->type=KEY;
    m->keyPos=XKeysymToKeycode(dpy,XStringToKeysym(keyPos));
    m->keyNeg=XKeysymToKeycode(dpy,XStringToKeysym(keyNeg));
    this->axis[axis] = m;
}

JoyMouse::map_st JoyMouse::getAxisMapping(int axe)
{
    return *axis[axe];
}

void JoyMouse::mapButtonKeySym(int button,unsigned int ks)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->button=button;
    m->type=KEY;
    m->keyPos=XKeysymToKeycode(dpy,ks);
    this->buttons[button] = m;
}

void JoyMouse::mapButton(int button,int type)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->button=button;
    m->type=type;
    buttons[button] = m;
}

void JoyMouse::mapButton(int button,const char* path)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->button=button;
    m->type=PROGRAM;
    m->command=path;
    buttons[button] = m;
}

void JoyMouse::mapButtonKey(int button,const char* key)
{
    map_st* m = (map_st*)malloc(sizeof(map_st));
    m->button=button;
    m->type=KEY;
    m->keyPos=XKeysymToKeycode(dpy,XStringToKeysym(key));
    buttons[button] = m;
}

JoyMouse::map_st JoyMouse::getButtonMapping(int button)
{
    return *buttons[button];
}

void JoyMouse::createMouseMovement(int x, int y)
{
    XTestFakeRelativeMotionEvent(dpy,x,y,CurrentTime);
}

void JoyMouse::createMouseClick(int button,int down)
{
    XTestFakeButtonEvent(dpy,button,down,CurrentTime);
}

void JoyMouse::createButtonPress(int key,int down)
{
    XTestFakeKeyEvent(dpy,key,down,CurrentTime);
}
