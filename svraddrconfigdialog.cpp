#include "svraddrconfigdialog.h"
#include "ui_svraddrconfigdialog.h"

#include <QGridLayout>
#include <QSettings>

#include "utility.h"

SvrAddrConfigDialog::SvrAddrConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SvrAddrConfigDialog)
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

    loadSvrAddrConfig();
}

SvrAddrConfigDialog::~SvrAddrConfigDialog()
{
    delete ui;
}

void SvrAddrConfigDialog::onSaveConfig()
{
    auto svr1Addr = ui->lineEditSvr1Addr->text();
    auto svr1Port = ui->lineEditSvr1Port->text();
    auto svr2Addr = ui->lineEditSvr2Addr->text();
    auto svr2Port = ui->lineEditSvr2Port->text();

    Utility::saveSvrAddrConfig(svr1Addr, svr1Port.toUShort(), svr2Addr, svr2Port.toUShort());
    emit svrAddrChanged(svr1Addr, svr1Port.toUShort(), svr2Addr, svr2Port.toUShort());

    close();
}

void SvrAddrConfigDialog::onCloseDialog()
{
    close();
}

void SvrAddrConfigDialog::loadSvrAddrConfig()
{
    QString svr1Ip;
    ushort svr1Port;
    QString svr2Ip;
    ushort svr2Port;
    if (!Utility::loadSvrAddrConfig(svr1Ip, svr1Port, svr2Ip, svr2Port))
        return;

    ui->lineEditSvr1Addr->setText(svr1Ip);
    ui->lineEditSvr1Port->setText(QString().setNum(svr1Port));
    ui->lineEditSvr2Addr->setText(svr2Ip);
    ui->lineEditSvr2Port->setText(QString().setNum(svr2Port));
}
