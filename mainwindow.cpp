#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "mscmonitorform.h"
#include "cwpmonitorform.h"
#include "phonemonitorform.h"
#include "radiomonitorform.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(800, 600);

    auto monitorDevicesMenu = menuBar()->addMenu(tr("Monitor Devices"));

    QIcon mscItemIcon(":images/toolbar_msc.ico");
    auto switchToMSCAct = new QAction(mscItemIcon, tr("MSC"), this);
    connect(switchToMSCAct, SIGNAL(triggered()), this, SLOT(onSwitchToMSC()));
    monitorDevicesMenu->addAction(switchToMSCAct);
    ui->mainToolBar->addAction(switchToMSCAct);

    QIcon phoneItemIcon(":images/toolbar_phone.ico");
    auto switchToPhoneAct = new QAction(phoneItemIcon, tr("Phone"), this);
    connect(switchToPhoneAct, SIGNAL(triggered()), this, SLOT(onSwitchToPhone()));
    monitorDevicesMenu->addAction(switchToPhoneAct);
    ui->mainToolBar->addAction(switchToPhoneAct);

    QIcon cwpItemIcon(":images/toolbar_cwp.ico");
    auto switchToCWPAct = new QAction(cwpItemIcon, tr("CWP"), this);
    connect(switchToCWPAct, SIGNAL(triggered()), this, SLOT(onSwitchToCWP()));
    monitorDevicesMenu->addAction(switchToCWPAct);
    ui->mainToolBar->addAction(switchToCWPAct);

    QIcon radioItemIcon(":images/toolbar_radio.ico");
    auto switchToRadioAct = new QAction(radioItemIcon, tr("Radio"), this);
    connect(switchToRadioAct, SIGNAL(triggered()), this, SLOT(onSwitchToRadio()));
    monitorDevicesMenu->addAction(switchToRadioAct);
    ui->mainToolBar->addAction(switchToRadioAct);

    //auto monitorConfigMenu = menuBar()->addMenu(tr("Configuration"));

    mscMonitorForm = new MSCMonitorForm;
    ui->stackedWidget->addWidget(mscMonitorForm);
    ui->stackedWidget->setCurrentWidget(mscMonitorForm);

    cwpMonitorForm = new CWPMonitorForm;
    ui->stackedWidget->addWidget(cwpMonitorForm);

    phoneMonitorForm = new PhoneMonitorForm;
    ui->stackedWidget->addWidget(phoneMonitorForm);

    radioMonitorForm = new RadioMonitorForm;
    ui->stackedWidget->addWidget(radioMonitorForm);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSwitchToMSC()
{
    qDebug() << tr("Switch to MSC monitor...");
    ui->stackedWidget->setCurrentWidget(mscMonitorForm);
}

void MainWindow::onSwitchToCWP()
{
    qDebug() << tr("Switch to CWP monitor...");
    ui->stackedWidget->setCurrentWidget(cwpMonitorForm);
}

void MainWindow::onSwitchToPhone()
{
    qDebug() << tr("Switch to phone monitor...");
    ui->stackedWidget->setCurrentWidget(phoneMonitorForm);
}

void MainWindow::onSwitchToRadio()
{
    qDebug() << tr("Switch to radio monitor...");
    ui->stackedWidget->setCurrentWidget(radioMonitorForm);
}
