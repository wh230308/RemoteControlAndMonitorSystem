#include "customsvrlabel.h"

#include <QVBoxLayout>
#include <QDebug>

#include "utility.h"


CustomSvrLabel::CustomSvrLabel(const QString &svrName, QWidget *parent) : QLabel(parent)
{
    initContentsLayout(svrName);
}

void CustomSvrLabel::updateRunningState(int runningState)
{
    qDebug() << tr("updata svr running state, runningState:%1").arg(runningState);
}

void CustomSvrLabel::updateEthPortsState(int port1State, int port2State)
{
    qDebug() << tr("update svr ethernet state, port1State:%1, port2State%2")
                .arg(port1State).arg(port2State);
}

void CustomSvrLabel::initContentsLayout(const QString &svrName)
{
    if (objectName().isEmpty())
        setObjectName(QString("customSvrLabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kSvrLabelWidth, kSvrLabelHeight, QString(":/images/svr.gif"));
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
