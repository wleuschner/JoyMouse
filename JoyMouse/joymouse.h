#ifndef JOYMOUSE_H
#define JOYMOUSE_H
#include <QX11Info>
#include <map>
#include "joypad.h"


class JoyMouse
{
public:
    enum TYPE
    {
        XAXIS,
        YAXIS,
        SCROLLPOSAXIS,
        SCROLLNEGAXIS,
        LEFTBUTTON,
        MIDDLEBUTTON,
        RIGHTBUTTON,
        KEY,
        PROGRAM,
    };

    struct map_st
    {
        int type;
        union
        {
            int axis;
            int button;
        };
        union
        {
            struct
            {
                int keyPos;
                int keyNeg;
            };
            const char* command;
        };
    };

    JoyMouse(Joypad* jp,bool activated=false);
    ~JoyMouse();
    void poll();

    void setActivated(bool activated);
    bool isActivated();

    void clearMappings();
    void mapAxisKeySym(int axis,unsigned int ksNeg,unsigned int ksPos);
    void mapAxis(int axis,int type);
    void mapAxis(int axis,const char* path);
    void mapAxisKey(int axis,const char* keyNeg,const char* keyPos);
    map_st getAxisMapping(int axe);

    void mapButtonKeySym(int axis,unsigned int ks);
    void mapButton(int axis,int type);
    void mapButton(int axis,const char* path);
    void mapButtonKey(int axis,const char* key);
    map_st getButtonMapping(int button);
private:
    enum
    {
        LEFT=1,
        MIDDLE=2,
        RIGHT=3
    };

    void createMouseMovement(int x,int y);
    void createMouseClick(int button,int down);
    void createButtonPress(int key,int down);

    bool activated;

    int *lastAxisDirections;
    std::map<int,map_st*> axis;
    std::map<int,map_st*> buttons;

    Display* dpy;
    Joypad* jp;
};

#endif // JOYMOUSE_H
