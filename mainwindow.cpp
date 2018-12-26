#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include "mscmonitorform.h"
#include "cwpmonitorform.h"
#include "phonemonitorform.h"
#include "radiomonitorform.h"
#include "udpclientmanager.h"
#include "svraddrconfigdialog.h"

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

    QIcon cwpItemIcon(":images/toolbar_cwp.ico");
    auto switchToCWPAct = new QAction(cwpItemIcon, tr("CWP"), this);
    connect(switchToCWPAct, SIGNAL(triggered()), this, SLOT(onSwitchToCWP()));
    monitorDevicesMenu->addAction(switchToCWPAct);
    ui->mainToolBar->addAction(switchToCWPAct);

    QIcon phoneItemIcon(":images/toolbar_phone.ico");
    auto switchToPhoneAct = new QAction(phoneItemIcon, tr("Phone"), this);
    connect(switchToPhoneAct, SIGNAL(triggered()), this, SLOT(onSwitchToPhone()));
    monitorDevicesMenu->addAction(switchToPhoneAct);
    ui->mainToolBar->addAction(switchToPhoneAct);

    QIcon radioItemIcon(":images/toolbar_radio.ico");
    auto switchToRadioAct = new QAction(radioItemIcon, tr("Radio"), this);
    connect(switchToRadioAct, SIGNAL(triggered()), this, SLOT(onSwitchToRadio()));
    monitorDevicesMenu->addAction(switchToRadioAct);
    ui->mainToolBar->addAction(switchToRadioAct);

    auto configurationMenu = menuBar()->addMenu(tr("Configuration"));
    auto svrAddressAct = new QAction(tr("Server Address"), this);
    connect(svrAddressAct, SIGNAL(triggered()), this, SLOT(onConfigurateSvrAddr()));
    configurationMenu->addAction(svrAddressAct);

    mscMonitorForm = new MSCMonitorForm;
    ui->stackedWidget->addWidget(mscMonitorForm);
    ui->stackedWidget->setCurrentWidget(mscMonitorForm);

    cwpMonitorForm = new CWPMonitorForm;
    ui->stackedWidget->addWidget(cwpMonitorForm);

    phoneMonitorForm = new PhoneMonitorForm;
    ui->stackedWidget->addWidget(phoneMonitorForm);

    radioMonitorForm = new RadioMonitorForm;
    ui->stackedWidget->addWidget(radioMonitorForm);

    udpClientMgr = new UdpClientManager;
    connect(udpClientMgr, SIGNAL(heartbeatTimeout()), mscMonitorForm, SLOT(onHeartbeatTimeout()));
    connect(udpClientMgr, SIGNAL(reportMainCardState(char,char,char)),
            mscMonitorForm, SLOT(onReportMainCardState(char,char,char)));
    connect(udpClientMgr, SIGNAL(reportUserCardState(char,char,char,char)),
            mscMonitorForm, SLOT(onReportUserCardState(char,char,char,char)));
    connect(udpClientMgr, SIGNAL(reportDeviceInfo(char,char,QByteArray)),
            mscMonitorForm, SLOT(onReportDeviceInfo(char,char,QByteArray)));
    connect(udpClientMgr, SIGNAL(reportEthPortsState(char,char,char,char)),
            mscMonitorForm, SLOT(onReportEthPortsState(char,char,char,char)));
    connect(udpClientMgr, SIGNAL(reportUserCardPortState(char,char,char,char,char)),
            mscMonitorForm, SLOT(onReportUserCardPortState(char,char,char,char,char)));
    connect(udpClientMgr, SIGNAL(reportCWPState(char,char,QByteArray,QByteArray,QByteArray,QByteArray)),
            cwpMonitorForm, SLOT(onReportCWPState(char,char,QByteArray,QByteArray,QByteArray,QByteArray)));
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

    char user[32] = { 'z', 'h', 'a', 'n', 'g', 's', 'h', 'a', 'n' };
    char left[32] = { '1', '2', '3', '4' };
    char right[32] = { '5', '6', '7', '8' };
    char roles[96] = { 'a', 'd', 'm', 'i', 'n' };
    roles[32] = 'n';
    roles[33] = 'o';
    roles[34] = 'r';
    roles[35] = 'm';
    roles[36] = 'a';
    roles[37] = 'l';

    roles[64] = 's';
    roles[65] = 'u';
    roles[66] = 'p';
    roles[67] = 'e';
    roles[68] = 'r';

    QByteArray userName(user, 32);
    QByteArray leftPhone(left, 32);
    QByteArray rightPhone(right, 32);
    QByteArray userRoles(roles, 96);
    cwpMonitorForm->onReportCWPState(1, 2, userName, leftPhone, rightPhone, userRoles);
}

void MainWindow::onSwitchToPhone()
{
    qDebug() << tr("Switch to phone monitor...");
    //ui->stackedWidget->setCurrentWidget(phoneMonitorForm);

    // test onReportMainCardState and onReportUserCardState
    {
        char deviceId = 0x01;
        char mpuFlag = 0x00;
        char state = 0x01;
        mscMonitorForm->onReportMainCardState(deviceId, mpuFlag, 1);
        //mscMonitorForm->onReportEthPortsState(deviceId, mpuFlag, 0x01, 0x00);

        deviceId = 0x02;
        mpuFlag = 0x00;
        state = 0x01;
        mscMonitorForm->onReportMainCardState(deviceId, mpuFlag, 0x01);
        mscMonitorForm->onReportEthPortsState(deviceId, mpuFlag, 0x01, 0x00);

        deviceId = 0x03;
        mpuFlag = 0x00;
        state = 0x01;
        mscMonitorForm->onReportMainCardState(deviceId, mpuFlag, state);
        mscMonitorForm->onReportEthPortsState(deviceId, mpuFlag, 0x01, 0x00);

        deviceId = 0x04;
        mpuFlag = 0x01;
        state = 0x01;
        mscMonitorForm->onReportMainCardState(deviceId, mpuFlag, 00);
        mscMonitorForm->onReportEthPortsState(deviceId, mpuFlag, 0x01, 0x01);
    }

    char deviceId = 0x06;
    for (char i = 3; i < 15; i++) {
        char slotIndex = i;
        char state = 0x01;
        char type = i % 8;
        mscMonitorForm->onReportUserCardState(deviceId, slotIndex, type, state);
        mscMonitorForm->onReportUserCardPortState(deviceId, slotIndex, 0x00, 0x00, 0x01);
    }
}

void MainWindow::onSwitchToRadio()
{
    qDebug() << tr("Switch to radio monitor...");
    //ui->stackedWidget->setCurrentWidget(radioMonitorForm);

    // test onReportDeviceInfo
    char deviceId[] = { 0x01, 0x02, 0x03, 0x04, 0x06};
    char deviceType[] = { 0x00, 0x00, 0x01, 0x01, 0x01 };
    char name[][7] = { { 'S', 'V', 'R', '1' },
                            { 'S', 'V', 'R', '2' },
                            { '2', '3', '0', '-', '2', '3', '1' },
                            { '2', '3', '2', '-', '2', '3', '3' },
                            { '2', '3', '4', '-', '2', '3', '5' } };

    for (int i = 0; i < 5; i++) {
        QByteArray deviceName(name[i], sizeof(name[i]) / sizeof(name[i][0]));
        mscMonitorForm->onReportDeviceInfo(deviceId[i], deviceType[i], deviceName);
    }
}

void MainWindow::onConfigurateSvrAddr()
{
    auto svrAddrConfigDlg = new SvrAddrConfigDialog;
    connect(svrAddrConfigDlg, SIGNAL(svrAddrChanged(QString,ushort,QString,ushort)),
            udpClientMgr, SLOT(onSvrAddrChanged(QString,ushort,QString,ushort)));
    svrAddrConfigDlg->exec();
}
