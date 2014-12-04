#ifndef POLLTHREAD_H
#define POLLTHREAD_H

#include <QThread>
#include "joypad.h"

class PollThread : public QThread
{
    Q_OBJECT

public:
    explicit PollThread(Joypad* jp,QObject *parent = 0);
    void stopRunning();

signals:

public slots:

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
    void run();
    Joypad* jp;
    bool running;
};

#endif // POLLTHREAD_H
