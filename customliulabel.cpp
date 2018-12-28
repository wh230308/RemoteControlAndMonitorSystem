#include "customliulabel.h"

#include <QGridLayout>
#include <QTimer>
#include <QtDebug>

#include "utility.h"
#include "customcardlabel.h"

static const QStringList cardTyepDescList = {
    QObject::tr("ASL"),
    QObject::tr("ALT"),
    QObject::tr("EM"),
    QObject::tr("MTK"),
    QObject::tr("ISDN"),
    QObject::tr("DTK"),
    QObject::tr("DSL"),
    QObject::tr("DIU"),
    QObject::tr("MPU"),
    QString(),
    QString(),
    QString(),
    QString(),
    QString(),
    QString(),
    QString(),
    QString(),
    QObject::tr("RIU"),
};

CustomLIULabel::CustomLIULabel(int width, int height, QWidget *parent)
    : QLabel(parent)
{
    initContentsLayout(width, height);

    cardStateTimer_ = new QTimer(this);
    connect(cardStateTimer_, SIGNAL(timeout()), this, SLOT(onUpdateCardStateTimer()));
    cardStateTimer_->start(kCardStateTimerInterval);
}

CustomLIULabel::~CustomLIULabel()
{
    cardStateTimer_->stop();
    vecLIUCards_.clear();
}

void CustomLIULabel::updateCardRunningState(int slotIndex, int cardType, int runningState)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < kCardNumberPerLIU);
    Q_ASSERT(cardType >= 0);
    Q_ASSERT(cardType < cardTyepDescList.size());

    auto liuCard = vecLIUCards_.at(slotIndex);
    if (!liuCard->isVisible())
        liuCard->setVisible(true);

    // 第9和第10槽位是MPU
    int typeIndex = ((slotIndex == 0x08) || (slotIndex == 0x09)) ? 0x08 : cardType;
    liuCard->updateCardTypeName(cardTyepDescList.at(typeIndex));
    liuCard->updateRunningState(runningState);
}

void CustomLIULabel::updateCardPortState(int slotIndex, int portId, int type, int state)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < kCardNumberPerLIU);

    auto lblLIUCard = vecLIUCards_.at(slotIndex);
    if (!lblLIUCard->isVisible())
        lblLIUCard->setVisible(true);

    lblLIUCard->updatePortState(portId, type, state);
}

void CustomLIULabel::updateCardEthPortsState(int mpuFlag, int port1State, int port2State)
{
    Q_ASSERT((mpuFlag == 0x00) || (mpuFlag == 0x01));

    int slotIndex = (mpuFlag == 0x00) ? 8 : 9;
    auto lblLIUCard = vecLIUCards_.at(slotIndex);
    if (!lblLIUCard->isVisible())
        lblLIUCard->setVisible(true);

    lblLIUCard->updateEthPortsState(port1State, port2State);
}

void CustomLIULabel::initContentsLayout(int width, int height)
{
    if (objectName().isEmpty())
        setObjectName(QString("customLIULabel%1").arg(Utility::generateUniqueObjectId()));
    Utility::fillLabelWithImage(this, width, height, QString(":/images/liu_item.gif"));
    setAlignment(Qt::AlignCenter);

    layoutContents_ = new QGridLayout(this);
    layoutContents_->setObjectName(QString("layoutContents%1")
                                   .arg(Utility::generateUniqueObjectId()));
    layoutContents_->setContentsMargins(10, 5, 10, 5);

    // 板卡及其对应槽位号
    int index = 0;
    for (int i = 0; i < kCardNumberPerLIU; i++) {
        bool isMPUCard = true;
        QString cardSlotIndexDesc;
        if ((i != 8) && (i != 9)) {
            cardSlotIndexDesc = tr("<b>P%1</b>").arg(++index);
            isMPUCard = false;
        }

        auto lblCardSlotIndex = new QLabel(cardSlotIndexDesc, this);
        lblCardSlotIndex->setObjectName(QString("lblCardSlotIndex%1")
                                        .arg(Utility::generateUniqueObjectId()));
        lblCardSlotIndex->setAlignment(Qt::AlignCenter);
        layoutContents_->addWidget(lblCardSlotIndex, 0, i, Qt::AlignCenter);

        auto lblLIUCard = new CustomCardLabel(isMPUCard, this);
        layoutContents_->addWidget(lblLIUCard, 1, i, Qt::AlignCenter);

        vecLIUCards_.push_back(lblLIUCard);
    }

    layoutContents_->setRowStretch(0, kLIUContentsFirstRowHeight);
    layoutContents_->setRowStretch(1, kLIUContentsLastRowHeight);
}

void CustomLIULabel::onUpdateCardStateTimer()
{
    static int parity = 0;

    foreach (auto item, vecLIUCards_) {
        if (item->isCardRunning()) {
            item->flickerRunningStateLamp(parity);
        }
    }

    parity++;
}
