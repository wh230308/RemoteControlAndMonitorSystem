#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MSCMonitorForm;
class CWPMonitorForm;
class PhoneMonitorForm;
class RadioMonitorForm;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSwitchToMSC();
    void onSwitchToCWP();
    void onSwitchToPhone();
    void onSwitchToRadio();

private:
    Ui::MainWindow *ui;
    MSCMonitorForm *mscMonitorForm;
    CWPMonitorForm *cwpMonitorForm;
    PhoneMonitorForm *phoneMonitorForm;
    RadioMonitorForm *radioMonitorForm;
};

#endif // MAINWINDOW_H
