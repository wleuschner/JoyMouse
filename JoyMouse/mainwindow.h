#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QLineEdit>
#include "joypad.h"
#include "joymouse.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void closeHandler();
    void configureHandler();
    void loadProfileHandler();
    void saveProfileHandler();

    void createButtonGroupBox();
    void createAxisGroupBox();
    void createJoystickComboBox();
    void createProfileComboBox();
    void createButtonAxisGroups();

    void updateMapping();
    void showMapDialog();
    void showMapAxisDialog();
    void showSensivityDialog();

private:
    unsigned int stringToKey(QString t);
    QString keyToString(unsigned int key);

    void timerEvent(QTimerEvent *e);
    void closeEvent(QCloseEvent *event);
    void createMenu();

    double xAxisSensivity;
    double yAxisSensivity;


    QMenu *trayIconMenu;
    QSystemTrayIcon *trayIcon;
    QLineEdit** axisEdits;
    QLineEdit** buttonEdits;

    unsigned int num_joypads;

    JoyMouse** joymouses;
    Joypad** joypads;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
