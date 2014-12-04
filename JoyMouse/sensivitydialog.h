#ifndef SENSIVITYDIALOG_H
#define SENSIVITYDIALOG_H

#include <QDialog>

namespace Ui {
class SensivityDialog;
}

class SensivityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SensivityDialog(QWidget *parent = 0);
    ~SensivityDialog();

    double getXSensivity();
    double getYSensivity();


private:
    Ui::SensivityDialog *ui;
};

#endif // SENSIVITYDIALOG_H
