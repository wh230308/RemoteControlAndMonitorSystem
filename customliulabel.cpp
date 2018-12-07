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

    cardStateTimer = new QTimer(this);
    connect(cardStateTimer, SIGNAL(timeout()), this, SLOT(onUpdateCardStateTimer()));
    cardStateTimer->start(500);
}

void CustomLIULabel::updateCardRunningState(int slotIndex, int state, int type)
{
    Q_ASSERT(slotIndex >= 0);
    Q_ASSERT(slotIndex < CardNumberPerLIU);
    Q_ASSERT(type >= 0);
    Q_ASSERT(type < cardTyepDescList.size());

    auto liuCard = liuCardList.at(slotIndex);
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
    Q_ASSERT(slotIndex < CardNumberPerLIU);
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId < CustomCardLabel::MaximumPortId);
    Q_ASSERT(type >= 0);
    Q_ASSERT(type < 11);

    auto liuCard = liuCardList.at(slotIndex);
    if (!liuCard->isVisible())
        liuCard->setVisible(true);

    bool isMPUCard = ((slotIndex == 0x08) || (slotIndex == 0x09)) ? true : false;
    liuCard->updatePortState(portId, state, type, isMPUCard);
}

void CustomLIULabel::initContentsLayout(const QString &liuName)
{
    if (objectName().isEmpty())
        setObjectName(QString("customLIULabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, LIULabelWidth, LIULabelHeigth, QString(":/images/liu_item.gif"));
    setAlignment(Qt::AlignCenter);

    contenstLayout = new QGridLayout(this);
    contenstLayout->setObjectName(QString("contenstLayout%1").arg(Utility::generateUniqueObjectId()));
    contenstLayout->setHorizontalSpacing(1);
    contenstLayout->setVerticalSpacing(0);

    // 板卡类型名称描述
    labelLIUName = new QLabel(tr("<b>Frame:%1</b>").arg(liuName), this);
    labelLIUName->setObjectName(QString("labelLIUName%1").arg(Utility::generateUniqueObjectId()));
    labelLIUName->setAlignment(Qt::AlignCenter);
    contenstLayout->addWidget(labelLIUName, 0, 0, 2, 1, Qt::AlignCenter);
    contenstLayout->setColumnStretch(0, 190);

    // 板卡及其对应槽位号
    for (int i = 0; i < CardNumberPerLIU; i++) {
        auto labelCardSlotIndex = new QLabel(tr("<b>%1</b>").arg(i + 1), this);
        labelCardSlotIndex->setObjectName(QString("labelCardSlotIndex%1")
                                          .arg(Utility::generateUniqueObjectId()));
        labelCardSlotIndex->setAlignment(Qt::AlignCenter);
        contenstLayout->addWidget(labelCardSlotIndex, 0, i + 1, Qt::AlignCenter);

        auto liuCard = new CustomCardLabel(this);
        contenstLayout->addWidget(liuCard, 1, i + 1, Qt::AlignCenter);
        contenstLayout->setColumnStretch(i + 1, 52);

        liuCardList.push_back(liuCard);
    }

    contenstLayout->setRowStretch(0, 22);
    contenstLayout->setRowStretch(1, 178);
}

void CustomLIULabel::onUpdateCardStateTimer()
{
    static int parity = 0;

    foreach (auto item, liuCardList) {
        if (item->isCardRunning()) {
            item->flickerRunningStateLamp(parity);
        }
    }

    parity++;
}
