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

CustomLIULabel::CustomLIULabel(const QString &liuName, QWidget *parent)
    : QLabel(parent)
{
    initContentsLayout(liuName);

    cardStateTimer_ = new QTimer(this);
    connect(cardStateTimer_, SIGNAL(timeout()), this, SLOT(onUpdateCardStateTimer()));
    cardStateTimer_->start(kCardStateTimerInterval);
}

void CustomLIULabel::updateCardStateInfo(int slotIndex, int runningState, int cardType)
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

void CustomLIULabel::updateCardPortState(int slotIndex, int portId, int state, int type)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < kCardNumberPerLIU);

    auto lblLIUCard = vecLIUCards_.at(slotIndex);
    if (!lblLIUCard->isVisible())
        lblLIUCard->setVisible(true);

    lblLIUCard->updatePortState(portId, state, type);
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

void CustomLIULabel::initContentsLayout(const QString &liuName)
{
    if (objectName().isEmpty())
        setObjectName(QString("customLIULabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kLIULabelWidth, kLIULabelHeigth,
                                QString(":/images/liu_item.gif"));
    setAlignment(Qt::AlignCenter);

    layoutContents_ = new QGridLayout(this);
    layoutContents_->setObjectName(QString("layoutContents%1")
                                   .arg(Utility::generateUniqueObjectId()));
    layoutContents_->setHorizontalSpacing(1);
    layoutContents_->setVerticalSpacing(0);

    // LIU名称描述
    lblLIUName_ = new QLabel(tr("<b>Frame:%1</b>").arg(liuName), this);
    lblLIUName_->setObjectName(QString("lblLIUName%1").arg(Utility::generateUniqueObjectId()));
    lblLIUName_->setAlignment(Qt::AlignCenter);
    layoutContents_->addWidget(lblLIUName_, 0, 0, 2, 1, Qt::AlignCenter);
    layoutContents_->setColumnStretch(0, kLIUContentNameColumnWidth);

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
        layoutContents_->addWidget(lblCardSlotIndex, 0, i + 1, Qt::AlignCenter);

        auto lblLIUCard = new CustomCardLabel(this, isMPUCard);
        layoutContents_->addWidget(lblLIUCard, 1, i + 1, Qt::AlignCenter);
        layoutContents_->setColumnStretch(i + 1, kLIUContentCardColumnWidth);

        vecLIUCards_.push_back(lblLIUCard);
    }

    layoutContents_->setRowStretch(0, kLIUContentFirstRowHeight);
    layoutContents_->setRowStretch(1, kLIUContentLastRowHeight);
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
