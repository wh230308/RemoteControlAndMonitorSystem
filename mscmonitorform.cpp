#include "mscmonitorform.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QLabel>
#include <QResizeEvent>

#include "utility.h"


MSCMonitorForm::MSCMonitorForm(QWidget *parent) : QWidget(parent)
{
    auto topLevelLayout = new QHBoxLayout(this);
    topLevelLayout->setObjectName(QStringLiteral("topLevelLayout"));
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QStringLiteral("scrollArea"));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    topLevelLayout->addWidget(scrollArea);

    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(0, 0, scrollArea->width(), scrollArea->height());
    scrollArea->setWidget(scrollAreaWidgetContents);

    contentsLayout = new QHBoxLayout(scrollAreaWidgetContents);
    contentsLayout->setObjectName(QStringLiteral("contentsLayout"));
    contentsLayout->setContentsMargins(FormMargin, FormMargin, FormMargin, 0);
    contentsLayout->setSpacing(0);

    initSvrItemsLayout();
    initNetworkBusLayout();
    initLIUItemsLayout();
}

MSCMonitorForm::~MSCMonitorForm()
{
}

void MSCMonitorForm::resizeEvent(QResizeEvent *event)
{
    scrollAreaWidgetContents->setGeometry(0, 0, scrollArea->width(), scrollArea->height());

    formWidth = event->size().width();
    formHeight = event->size().height();
    checkChildWidgetsSizeToScroll(formWidth, formHeight);
}

void MSCMonitorForm::initSvrItemsLayout()
{
    svrItemsLayout = new QVBoxLayout();
    svrItemsLayout->setObjectName(QStringLiteral("svrItemsLayout"));
    svrItemsLayout->setContentsMargins(0, 0, 0, 0);
    svrItemsLayout->setSpacing(50);
    contentsLayout->addLayout(svrItemsLayout);
    contentsLayout->setStretch(0, 3);

    auto svrTopSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    svrItemsLayout->addSpacerItem(svrTopSpacer);

    auto svrBottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    svrItemsLayout->addSpacerItem(svrBottomSpacer);
}

void MSCMonitorForm::initNetworkBusLayout()
{
    networkBusItemsLayout = new QVBoxLayout();
    networkBusItemsLayout->setObjectName(QStringLiteral("networkBusItemsLayout"));
    networkBusItemsLayout->setContentsMargins(0, 0, 0, 0);
    networkBusItemsLayout->setSpacing(0);
    networkBusItemsLayout->setDirection(QBoxLayout::BottomToTop);
    contentsLayout->addLayout(networkBusItemsLayout);
    contentsLayout->setStretch(0, 3);

    addNetworkBusItem();
}

void MSCMonitorForm::initLIUItemsLayout()
{
    liuItemsLayout = new QVBoxLayout();
    liuItemsLayout->setObjectName(QStringLiteral("liuItemsLayout"));
    liuItemsLayout->setContentsMargins(0, 0, 0, 0);
    liuItemsLayout->setSpacing(LIUItemsSpace);
    contentsLayout->addLayout(liuItemsLayout);
    contentsLayout->setStretch(2, 20);

    auto bottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    liuItemsLayout->addSpacerItem(bottomSpacer);
}

void MSCMonitorForm::addSvrItem(int deviceId, int deviceType, const QString &svrName)
{
    static int objectId = 0;

    auto labelSvr = new QLabel(scrollAreaWidgetContents);
    labelSvr->setObjectName(QString("labelSvr%1").arg(objectId));
    Utility::fillLabelWithImage(labelSvr, SvrLabelWidth, SvrLabelHeight, QString(":/images/svr.gif"));
    svrItemsLayout->insertWidget(svrItemsLayout->count() - 1, labelSvr);
    svrItemsLayout->setAlignment(labelSvr, Qt::AlignHCenter | Qt::AlignTop);

    auto svrInnerLayout = new QVBoxLayout(labelSvr);
    svrInnerLayout->setObjectName(QString("svrInnerLayout%1").arg(objectId));
    svrInnerLayout->setContentsMargins(9, 9, 9, 3);

    auto labelSvrName = new QLabel(QString("<h2>%1</h2>").arg(svrName), labelSvr);
    labelSvrName->setObjectName(QString("labelSvrName%1").arg(objectId));
    svrInnerLayout->addWidget(labelSvrName);
    svrInnerLayout->setAlignment(labelSvrName, Qt::AlignHCenter | Qt::AlignBottom);

    int key = (deviceId << 16) | deviceType;
    deviceList.insert(key, static_cast<void *>(new SvrItem{ labelSvr, labelSvrName }));

    objectId++;
    liuItemCount++;
}

void MSCMonitorForm::addNetworkBusItem()
{
    static int objectId = 0;

    auto labelNetworkBus = new QLabel(scrollAreaWidgetContents);
    labelNetworkBus->setObjectName(QString("labelNetworkBus%1").arg(objectId));
    Utility::fillLabelWithImage(labelNetworkBus, NetworkBusLabelWidth, NetworkBusLabelHeight,
                                QString(":/images/network_bus.gif"));
    networkBusItemsLayout->addWidget(labelNetworkBus);
    networkBusItemsLayout->setAlignment(labelNetworkBus, Qt::AlignHCenter | Qt::AlignTop);

    objectId++;
}

void MSCMonitorForm::addLIUItem(int deviceId, int deviceType, const QString &liuItemName)
{
    static int objectId = 0;
    auto liuItem = new LIUItem;

    liuItem->labelLIU = new QLabel(scrollAreaWidgetContents);
    liuItem->labelLIU->setObjectName(QString("labelLIUItem%1").arg(objectId));
    Utility::fillLabelWithImage(liuItem->labelLIU, LIULabelWidth, LIULabelHeigth,
                                QString(":/images/liu_item.gif"));
    liuItemsLayout->insertWidget(liuItemsLayout->count() - 1, liuItem->labelLIU);
    liuItemsLayout->setAlignment(liuItem->labelLIU, Qt::AlignHCenter | Qt::AlignTop);

    liuItem->labelLIUName = new QLabel(QString("<h2>Frame:%1</h2>").arg(liuItemName),
                                       liuItem->labelLIU);
    addCardsToLIUItem(liuItem->labelLIU, liuItem->labelLIUName, liuItem->liuCardList);

    int key = (deviceId << 16) | deviceType;
    deviceList.insert(key, static_cast<void *>(liuItem));

    objectId++;
}

void MSCMonitorForm::addCardsToLIUItem(QLabel *labelLIUItem, QLabel *labelLIUItemName,
                                       QVector<LIUCard *> &liuCardList)
{
    static int objectId = 0;

    auto liuItemContenstLayout = new QGridLayout(labelLIUItem);
    liuItemContenstLayout->setObjectName(QString("liuItemContenstLayout%1").arg(objectId));
    liuItemContenstLayout->setHorizontalSpacing(0);

    // LIU名称
    labelLIUItemName->setObjectName(QString("labelLIUItemName%1").arg(objectId));
    labelLIUItemName->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    liuItemContenstLayout->addWidget(labelLIUItemName, 0, 0, 2, 1);
    liuItemContenstLayout->setColumnStretch(0, LIUItemLabelNameHStretch);

    for (int i = 0; i < CardNumberPerLIUItem; i++) {
        // 板卡槽位序号
        auto labelCardId = new QLabel(tr("<h3>%1</h3>").arg(i + 1), labelLIUItem);
        labelCardId->setObjectName(QString("labelCardId%1").arg(objectId * 10 + i));
        labelCardId->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        liuItemContenstLayout->addWidget(labelCardId, 0, i + 1);

        // 板卡面板
        initCardLayout(i + 1, labelLIUItem, liuItemContenstLayout, liuCardList);
    }

    objectId++;
}

void MSCMonitorForm::initCardLayout(int index, QWidget *parent, QGridLayout *parentLayout,
                                    QVector<LIUCard *> &liuCardList)
{
    static int objectId = 0;
    auto liuCard = new LIUCard;
    liuCardList.push_back(liuCard);

    // 板卡
    liuCard->labelLIUCard = new QLabel(parent);
    liuCard->labelLIUCard->setObjectName(QString("labelLIUCard%1").arg(objectId));
    Utility::fillLabelWithImage(liuCard->labelLIUCard, CardLabelWidth, CardLabelHeight,
                                QString(":/images/card.gif"));
    parentLayout->addWidget(liuCard->labelLIUCard, 1, index);
    parentLayout->setColumnStretch(index, CardLabelHStretch);

    auto cardContenstLayout = new QGridLayout(liuCard->labelLIUCard);
    cardContenstLayout->setObjectName(QString("cardContenstLayout%1").arg(objectId));
    cardContenstLayout->setHorizontalSpacing(7);
    cardContenstLayout->setVerticalSpacing(0);

    // 板卡类型名称
    liuCard->labelTypeName = new QLabel(liuCard->labelLIUCard);
    liuCard->labelTypeName->setObjectName(QString("cardTypeName%1").arg(objectId));
    cardContenstLayout->addWidget(liuCard->labelTypeName, 0, 0, 1, 2,
                                  Qt::AlignCenter | Qt::AlignBottom);

    // 板卡运行状态灯
    liuCard->labelRunningStateLamp = new QLabel(liuCard->labelLIUCard);
    liuCard->labelRunningStateLamp->setObjectName(QString("cardRunningStateLamp%1").arg(objectId));
    Utility::fillLabelWithImage(liuCard->labelRunningStateLamp, CardStateLampLabelWidth,
                                CardStateLampLabelHeight, QString(":/images/lamp_off.png"));
    cardContenstLayout->addWidget(liuCard->labelRunningStateLamp, 1, 0, Qt::AlignRight);

    // 板卡运行状态描述
    auto cardRunningStateDesc= new QLabel(tr("<b>Run</b>"), liuCard->labelLIUCard);
    cardRunningStateDesc->setObjectName(QString("cardRunningStateDesc%1").arg(objectId));
    cardRunningStateDesc->setFont(QFont(QString("Microsoft YaHei"), 6, 1));
    cardContenstLayout->addWidget(cardRunningStateDesc, 1, 1, Qt::AlignLeft);

    // 板卡端口1状态
    auto cardPort1Number = new QLabel(tr("<b>1</b>"), liuCard->labelLIUCard);
    cardPort1Number->setObjectName(QString("cardPort1Number%1").arg(objectId));
    cardPort1Number->setFont(QFont(QString("Microsoft YaHei"), 6, 1));
    cardContenstLayout->addWidget(cardPort1Number, 2, 0, Qt::AlignRight);

    liuCard->labelCardPort1Lamp = new QLabel(liuCard->labelLIUCard);
    liuCard->labelCardPort1Lamp->setObjectName(QString("labelCardPort1Lamp%1").arg(objectId));
    Utility::fillLabelWithImage(liuCard->labelCardPort1Lamp, CardStateLampLabelWidth,
                                CardStateLampLabelHeight, QString(":/images/lamp_off.png"));
    cardContenstLayout->addWidget(liuCard->labelCardPort1Lamp, 2, 1);

    // 板卡端口2状态
    auto cardPort2Number = new QLabel(tr("<b>2</b>"), liuCard->labelLIUCard);
    cardPort2Number->setObjectName(QString("cardPort2Number%1").arg(objectId));
    cardPort2Number->setFont(QFont(QString("Microsoft YaHei"), 6, 1));
    cardContenstLayout->addWidget(cardPort2Number, 3, 0, Qt::AlignRight);

    liuCard->labelCardPort2Lamp = new QLabel(liuCard->labelLIUCard);
    liuCard->labelCardPort2Lamp->setObjectName(QString("labelCardPort2Lamp%1").arg(objectId));
    Utility::fillLabelWithImage(liuCard->labelCardPort2Lamp, CardStateLampLabelWidth,
                                CardStateLampLabelHeight, QString(":/images/lamp_off.png"));
    cardContenstLayout->addWidget(liuCard->labelCardPort2Lamp, 3, 1);

    cardContenstLayout->setRowStretch(0, 60);
    cardContenstLayout->setRowStretch(1, 20);
    cardContenstLayout->setRowStretch(2, 15);
    cardContenstLayout->setRowStretch(3, 15);
    cardContenstLayout->setRowStretch(4, 66);

    objectId++;
}

void MSCMonitorForm::checkChildWidgetsSizeToScroll(int formWidth, int formHeight)
{
    int totalChildWidgetsWidth = SvrLabelWidth + NetworkBusLabelWidth + LIULabelWidth
            + FormMargin * 2;
    if (totalChildWidgetsWidth > formWidth)
        scrollAreaWidgetContents->setMinimumWidth(totalChildWidgetsWidth);

    int totalChildWidgetsHeight = 0;
    if (liuItemCount <= DefaultLIUItemsNumber)
        totalChildWidgetsHeight = FormMargin + NetworkBusLabelHeight;
    else
        totalChildWidgetsHeight = FormMargin + LIULabelHeigth * liuItemCount
                + LIUItemsSpace * (liuItemCount - 1);

    if (totalChildWidgetsHeight > formHeight)
        scrollAreaWidgetContents->setMinimumHeight(totalChildWidgetsHeight);
}
