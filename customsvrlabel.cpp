#include "customsvrlabel.h"

#include <QVBoxLayout>
#include <QDebug>

#include "utility.h"

static const QString svrImgPath(":/images/svr.gif");
static const QString svrImgPathOnline(":/images/svr_online.jpg");
static const QString svrImgPathOffline(":/images/svr_offline.jpg");
static const QString svrImgPathSingleEth(":/images/svr_single_eth.jpg");

CustomSvrLabel::CustomSvrLabel(const QString &svrName, QWidget *parent) : QLabel(parent)
{
    initContentsLayout(svrName);
}

void CustomSvrLabel::updateRunningState(int runningState)
{
    if (runningState == 0) {
        isRunning_ = false;
        Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPathOffline);
    }
    else if (runningState == 1) {
        isRunning_ = true;
        if (isDoubleEths_)
            Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPathOnline);
        else
            Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPathSingleEth);
    }
}

void CustomSvrLabel::updateEthPortsState(int port1State, int port2State)
{
    if (!isRunning_) {
        Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPathOffline);
        return;
    }

    if ((port1State == 0) || (port2State == 0))
        Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPathSingleEth);
}

void CustomSvrLabel::initContentsLayout(const QString &svrName)
{
    if (objectName().isEmpty())
        setObjectName(QString("customSvrLabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, svrImgPath);
    setAlignment(Qt::AlignCenter);

    auto contentsLayout = new QVBoxLayout(this);
    contentsLayout->setObjectName(QString("contentsLayout%1").arg(Utility::generateUniqueObjectId()));
    contentsLayout->setContentsMargins(9, 9, 9, 3);

    auto lblSvrName = new QLabel(QString("<h2>%1</h2>").arg(svrName), this);
    lblSvrName->setObjectName(QString("lblSvrName%1").arg(Utility::generateUniqueObjectId()));
    lblSvrName->setAlignment(Qt::AlignCenter);
    contentsLayout->addWidget(lblSvrName);
    contentsLayout->setAlignment(lblSvrName, Qt::AlignHCenter | Qt::AlignBottom);
}
