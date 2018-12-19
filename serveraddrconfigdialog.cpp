#include "serveraddrconfigdialog.h"
#include "ui_serveraddrconfigdialog.h"

#include <QGridLayout>
#include <QSettings>

#include "utility.h"

#define SVRS_ADDR_FILENAME ("ServersAddress.ini")
#define SVR1_IP_KEY ("Server1/Ip")
#define SVR1_PORT_KEY ("Server1/Port")
#define SVR2_IP_KEY ("Server2/Ip")
#define SVR2_PORT_KEY ("Server2/Port")

ServerAddrConfigDialog::ServerAddrConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerAddrConfigDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    auto svr1GroupLayout = new QGridLayout(ui->groupBoxSvr1);
    svr1GroupLayout->setObjectName(QStringLiteral("svr1GroupLayout"));
    svr1GroupLayout->addWidget(ui->labelSvr1Addr, 0, 0);
    svr1GroupLayout->addWidget(ui->lineEditSvr1Addr, 0, 1);
    svr1GroupLayout->addWidget(ui->labelSvr1Port, 1, 0);
    svr1GroupLayout->addWidget(ui->lineEditSvr1Port, 1, 1);

    auto svr2GroupLayout = new QGridLayout(ui->groupBoxSvr2);
    svr2GroupLayout->setObjectName(QStringLiteral("svr2GroupLayout"));
    svr2GroupLayout->addWidget(ui->labelSvr2Addr, 0, 0);
    svr2GroupLayout->addWidget(ui->lineEditSvr2Addr, 0, 1);
    svr2GroupLayout->addWidget(ui->labelSvr2Port, 1, 0);
    svr2GroupLayout->addWidget(ui->lineEditSvr2Port, 1, 1);

    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(onSaveConfig()));
    connect(ui->pushButtonClose, SIGNAL(clicked()), this, SLOT(onCloseDialog()));

    loadServerAddrConfig();
}

ServerAddrConfigDialog::~ServerAddrConfigDialog()
{
    delete ui;
}

void ServerAddrConfigDialog::onSaveConfig()
{
    auto svr1Addr = ui->lineEditSvr1Addr->text();
    auto svr1port = ui->lineEditSvr1Port->text();
    auto svr2Addr = ui->lineEditSvr2Addr->text();
    auto svr2port = ui->lineEditSvr1Port->text();

    QSettings setting(QString("%1\\%2").arg(QCoreApplication::applicationDirPath())
                      .arg(SVRS_ADDR_FILENAME), QSettings::IniFormat);
    setting.setValue(SVR1_IP_KEY, ui->lineEditSvr1Addr->text());
    setting.setValue(SVR1_PORT_KEY, ui->lineEditSvr1Port->text());
    setting.setValue(SVR2_IP_KEY, ui->lineEditSvr2Addr->text());
    setting.setValue(SVR2_PORT_KEY, ui->lineEditSvr2Port->text());

    emit serversAddrChanged(svr1Addr, svr1port.toUShort(), svr2Addr, svr2port.toUShort());

    close();
}

void ServerAddrConfigDialog::onCloseDialog()
{
    close();
}

void ServerAddrConfigDialog::loadServerAddrConfig()
{
    QString svr1Ip;
    ushort svr1Port;
    QString svr2Ip;
    ushort svr2Port;
    Utility::loadServerAddrConfig(svr1Ip, svr1Port, svr2Ip, svr2Port);
    ui->lineEditSvr1Addr->setText(svr1Ip);
    ui->lineEditSvr1Port->setText(QString().setNum(svr1Port));
    ui->lineEditSvr2Addr->setText(svr2Ip);
    ui->lineEditSvr2Port->setText(QString().setNum(svr2Port));
}
