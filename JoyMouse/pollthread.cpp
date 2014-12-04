#include "pollthread.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <QX11Info>
#include <X11/extensions/XTest.h>

PollThread::PollThread(Joypad* jp,QObject *parent) :
    QThread(parent)
{
    this->jp=jp;
    running = true;
}

void PollThread::run()
{
    Joypad::JOY_EVENT event;
    while(running)
    {
            usleep(10*1000);
            if(jp->poll(&event))
            {
                switch(event.type)
                {
                case Joypad::BUTTON_DOWN:
                    if(event.button==0)
                    {
                        createMouseClick(LEFT,True);
                    }
                    if(event.button==1)
                    {
                        createMouseClick(RIGHT,True);
                    }
                    break;
                case Joypad::BUTTON_UP:
                    if(event.button==0)
                    {
                        createMouseClick(LEFT,False);
                    }
                    if(event.button==1)
                    {
                        createMouseClick(RIGHT,False);
                    }
                    break;
                }
            }
            int x=8*jp->getAxis(0)/32764;
            int y=8*jp->getAxis(1)/32764;
            createMouseMovement(x,y);
    }
}

void PollThread::createMouseMovement(int x, int y)
{
    XEvent e;
    Display *dpy = QX11Info::display();
    XQueryPointer(dpy,RootWindow(dpy,0),&e.xbutton.root,&e.xbutton.window,
                  &e.xbutton.x_root,&e.xbutton.y_root,
                  &e.xbutton.x,&e.xbutton.y,&e.xbutton.state);
    XTestFakeMotionEvent(QX11Info::display(),0,e.xbutton.x+x,e.xbutton.y+y,CurrentTime);
}

void PollThread::createMouseClick(int button,int down)
{
    Display *dpy = QX11Info::display();
    XTestFakeButtonEvent(dpy,button,down,CurrentTime);
}

void PollThread::createButtonPress(int key,int down)
{
    Display *dpy = QX11Info::display();
    XTestFakeKeyEvent(dpy,key,down,CurrentTime);
}

void PollThread::stopRunning()
{
    running=false;
}
