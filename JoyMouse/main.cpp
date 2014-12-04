#include<QApplication>
#include<QSplashScreen>
#include<QTimer>
#include<QX11Info>
#include"mainwindow.h"

#include<stdlib.h>
#include<unistd.h>
#include<X11/extensions/XTest.h>
#include"joypad.h"

int main(int argc,char**argv)
{
    int ev,er,ma,mi;
    QApplication app(argc,argv);
    if(!XTestQueryExtension(QX11Info::display(),&ev,&er,&ma,&mi))
    {
        return -5;
    }
    QSplashScreen sc(QPixmap(":Assets/Gamepad.jpg"));
    sc.show();
    MainWindow mw;
    QTimer::singleShot(5000,&sc,SLOT(close()));
    return app.exec();
}

