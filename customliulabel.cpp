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

void CustomLIULabel::updateCardRunningState(int slotIndex, int state, int type)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < kCardNumberPerLIU);
    Q_ASSERT(type >= 0);
    Q_ASSERT(type < cardTyepDescList.size());

    auto liuCard = liuCardList_.at(slotIndex);
    if (!liuCard->isVisible())
        liuCard->setVisible(true);

    // 第9和第10槽位是MPU
    int typeIndex = ((slotIndex == 0x08) || (slotIndex == 0x09)) ? 0x08 : type;
    liuCard->updateCardTypeName(cardTyepDescList.at(typeIndex));
    liuCard->updateRunningState(state);
}

void CustomLIULabel::updateCardPortState(int slotIndex, int portId, int state, int type)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < kCardNumberPerLIU);
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId < CustomCardLabel::kMaximumPortId);
    Q_ASSERT(type >= 0);
    Q_ASSERT(type < 11);

    auto liuCard = liuCardList_.at(slotIndex);
    if (!liuCard->isVisible())
        liuCard->setVisible(true);

    liuCard->updatePortState(portId, state, type);
}

void CustomLIULabel::initContentsLayout(const QString &liuName)
{
    if (objectName().isEmpty())
        setObjectName(QString("customLIULabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kLIULabelWidth, kLIULabelHeigth, QString(":/images/liu_item.gif"));
    setAlignment(Qt::AlignCenter);

    contenstLayout_ = new QGridLayout(this);
    contenstLayout_->setObjectName(QString("contenstLayout%1").arg(Utility::generateUniqueObjectId()));
    contenstLayout_->setHorizontalSpacing(1);
    contenstLayout_->setVerticalSpacing(0);

    // LIU名称描述
    labelLIUName_ = new QLabel(tr("<b>Frame:%1</b>").arg(liuName), this);
    labelLIUName_->setObjectName(QString("labelLIUName%1").arg(Utility::generateUniqueObjectId()));
    labelLIUName_->setAlignment(Qt::AlignCenter);
    contenstLayout_->addWidget(labelLIUName_, 0, 0, 2, 1, Qt::AlignCenter);
    contenstLayout_->setColumnStretch(0, kLIUContentNameColumnWidth);

    // 板卡及其对应槽位号
    int index = 0;
    for (int i = 0; i < kCardNumberPerLIU; i++) {
        bool isMPUCard = true;
        QString cardSlotIndexDesc;
        if ((i != 8) && (i != 9)) {
            cardSlotIndexDesc = tr("<b>P%1</b>").arg(++index);
            isMPUCard = false;
        }

        auto labelCardSlotIndex = new QLabel(cardSlotIndexDesc, this);
        labelCardSlotIndex->setObjectName(QString("labelCardSlotIndex%1")
                                          .arg(Utility::generateUniqueObjectId()));
        labelCardSlotIndex->setAlignment(Qt::AlignCenter);
        contenstLayout_->addWidget(labelCardSlotIndex, 0, i + 1, Qt::AlignCenter);

        auto liuCard = new CustomCardLabel(this, isMPUCard);
        contenstLayout_->addWidget(liuCard, 1, i + 1, Qt::AlignCenter);
        contenstLayout_->setColumnStretch(i + 1, kLIUContentCardColumnWidth);

        liuCardList_.push_back(liuCard);
    }

    contenstLayout_->setRowStretch(0, kLIUContentFirstRowHeight);
    contenstLayout_->setRowStretch(1, kLIUContentLastRowHeight);
}

void CustomLIULabel::onUpdateCardStateTimer()
{
    static int parity = 0;

    foreach (auto item, liuCardList_) {
        if (item->isCardRunning()) {
            item->flickerRunningStateLamp(parity);
        }
    }

    parity++;
}
