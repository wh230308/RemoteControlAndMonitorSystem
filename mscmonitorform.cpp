#include "mscmonitorform.h"

#include <algorithm>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QLabel>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>

#include "utility.h"
#include "log.hpp"
#include "customsvrlabel.h"
#include "customliulabel.h"
#include "customcardlabel.h"
#include "mscdatadeclaration.h"


MSCMonitorForm::MSCMonitorForm(QWidget *parent) : QWidget(parent)
{
    initFormContentsLayout();

    devStateTimer_ = new QTimer(this);
    connect(devStateTimer_, SIGNAL(timeout()), this, SLOT(onUpdateDeviceStateTimer()));
    devStateTimer_->start(kDeviceStateTimerInterval);
}

MSCMonitorForm::~MSCMonitorForm()
{
    if (devStateTimer_ != nullptr)
        devStateTimer_->stop();
}

void MSCMonitorForm::onHeartbeatTimeout()
{
}

void MSCMonitorForm::onReportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName)
{
    LOG_TRACE("report the device info, deviceId:%d, deviceType:%d, deviceName:%s",
              deviceId, deviceType, deviceName.constData());

    if (mapDevs_.contains(deviceId))
        return;

    if (deviceType == 0) {
        addSvrItem(deviceId, deviceType, QString::fromUtf8(deviceName));
    } else if (deviceType == 1) {
        addLIUItem(deviceId, deviceType, QString::fromUtf8(deviceName));
        checkChildWidgetsSizeToScroll(formWidth_, formHeight_);
    }
}

void MSCMonitorForm::onReportMainCardState(char deviceId, char mpuFlag, char state)
{
    Q_ASSERT((mpuFlag == 0x00) || (mpuFlag == 0x01));
    Q_ASSERT((state == 0x00) || (state == 0x01));
    LOG_TRACE("report the SVR or LIU's main card state, deviceId:%d, mpuFlag:%d, state:%d",
              deviceId, mpuFlag, state);

    // 设备为LIU主控板时需设置槽位号为第8或第9槽
    int mpuSlotIndex = (mpuFlag == 0x00) ? 8 : 9;
    updateDeviceState(deviceId, mpuSlotIndex, 0x00, state);
}

void MSCMonitorForm::onReportUserCardState(char deviceId, char slotIndex, char type, char state)
{
    Q_ASSERT(slotIndex >= 0x00);
    Q_ASSERT(slotIndex < CustomLIULabel::kCardNumberPerLIU);
    Q_ASSERT(type >= 0x00);
    Q_ASSERT((type <= 0x07) || (type == 0x10));
    Q_ASSERT((state == 0x00) || (state == 0x01));
    LOG_TRACE("report user card state, deviceId:%d, slotIndex:%d, type:%d, state:%d",
              deviceId, slotIndex, type, state);

    // 界面布局板卡时，主控板和用户板排成一排，共18个槽位，主控板占据第8、9槽位，其余槽位由用户板占用。
    // 而上报的用户板槽位号是从0~15，因此用户板槽位号大于8时往右偏移2个位置
    slotIndex = (slotIndex > 0x08) ? (slotIndex + 0x02) : slotIndex;
    updateDeviceState(deviceId, slotIndex, type, state);
}

void MSCMonitorForm::onReportEthPortsState(char deviceId, char mpuFlag, char port1State,
                                           char port2State)
{
    Q_ASSERT((mpuFlag == 0x00) || (mpuFlag == 0x01));
    Q_ASSERT((port1State == 0x00) || (port1State == 0x01));
    Q_ASSERT((port2State == 0x00) || (port2State == 0x01));
    LOG_TRACE("report the SVR or LIU's main card ethernet port state, deviceId:%d, mpuFlag:%d, "
              "port1State:%d, port2State:%d", mpuFlag, deviceId, port1State, port2State);

    updateEthPortsState(deviceId, mpuFlag, port1State, port2State);
}

void MSCMonitorForm::onReportUserCardPortState(char deviceId, char slotIndex, char portId,
                                               char type, char state)
{
    Q_ASSERT(slotIndex >= 00);
    Q_ASSERT(slotIndex < CustomLIULabel::kCardNumberPerLIU);
    Q_ASSERT(type >= 0x00);
    Q_ASSERT(type <= 0x0a);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    LOG_TRACE("report the user card port state, deviceId:%d, slotIndex:%d, portId:%d, type:%d, "
              "state:%d", deviceId, slotIndex, static_cast<int>(portId), type, state);

    slotIndex = (slotIndex > 0x08) ? (slotIndex + 0x02) : slotIndex;
    updateUserCardPortState(deviceId, slotIndex, static_cast<int>(portId),  type, state);
}

void MSCMonitorForm::resizeEvent(QResizeEvent *event)
{
    widgetScrollAreaContent_->setGeometry(0, 0, scrollArea_->width(), scrollArea_->height());

    formWidth_ = event->size().width();
    formHeight_ = event->size().height();
    checkChildWidgetsSizeToScroll(formWidth_, formHeight_);
}

void MSCMonitorForm::initFormContentsLayout()
{
    auto topLevelLayout = new QHBoxLayout(this);
    topLevelLayout->setObjectName(QStringLiteral("topLevelLayout"));
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setObjectName(QStringLiteral("scrollArea"));
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setAlignment(Qt::AlignCenter);
    topLevelLayout->addWidget(scrollArea_);

    widgetScrollAreaContent_ = new QWidget();
    widgetScrollAreaContent_->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
    widgetScrollAreaContent_->setGeometry(0, 0, scrollArea_->width(), scrollArea_->height());
    scrollArea_->setWidget(widgetScrollAreaContent_);

    layoutcontents_ = new QHBoxLayout(widgetScrollAreaContent_);
    layoutcontents_->setObjectName(QStringLiteral("contentsLayout"));
    layoutcontents_->setContentsMargins(kFormMargin, kFormMargin, kFormMargin, 0);
    layoutcontents_->setSpacing(0);

    // svr布局
    layoutSvrItems_ = new QVBoxLayout();
    layoutSvrItems_->setObjectName(QStringLiteral("svrItemsLayout"));
    layoutSvrItems_->setContentsMargins(0, 0, 0, 0);
    layoutSvrItems_->setSpacing(50);
    layoutcontents_->addLayout(layoutSvrItems_, kSvrHStretch);

    int topSpacerMinH = kNetworkBusLabelHeight / 3 - (kSvrLabelHeight / 2);
    auto svrTopSpacer = new QSpacerItem(10, topSpacerMinH, QSizePolicy::Minimum, QSizePolicy::Minimum);
    layoutSvrItems_->addSpacerItem(svrTopSpacer);

    auto svrBottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layoutSvrItems_->addSpacerItem(svrBottomSpacer);

    // network bus布局
    layoutNetworkBusItems_ = new QVBoxLayout();
    layoutNetworkBusItems_->setObjectName(QStringLiteral("networkBusItemsLayout"));
    layoutNetworkBusItems_->setContentsMargins(0, 0, 0, 0);
    layoutNetworkBusItems_->setSpacing(0);
    layoutcontents_->addLayout(layoutNetworkBusItems_, kNetworkBusHStretch);

    addNetworkBusItem();

    // LIU布局
    layoutLIUItems_ = new QVBoxLayout();
    layoutLIUItems_->setObjectName(QStringLiteral("liuItemsLayout"));
    layoutLIUItems_->setContentsMargins(0, 0, 0, 0);
    layoutLIUItems_->setSpacing(kLIUItemsSpace);
    layoutcontents_->addLayout(layoutLIUItems_, kLIUItemsHStretch);

    auto bottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    layoutLIUItems_->addSpacerItem(bottomSpacer);
}

void MSCMonitorForm::addSvrItem(int deviceId, int deviceType, const QString &svrItemName)
{
    Q_ASSERT((deviceType == 0x00) || (deviceType == 0x01));

    auto lblSvrItem = new CustomSvrLabel(svrItemName, widgetScrollAreaContent_);
    layoutSvrItems_->insertWidget(layoutSvrItems_->count() - 1, lblSvrItem);
    layoutSvrItems_->setAlignment(lblSvrItem, Qt::AlignHCenter | Qt::AlignTop);

    if (!mapDevs_.contains(deviceId)) {
        mapDevs_.insert(deviceId, new Device(deviceType, lblSvrItem));
    }
}

void MSCMonitorForm::addNetworkBusItem()
{
    auto labelNetworkBus = new QLabel(widgetScrollAreaContent_);
    labelNetworkBus->setObjectName(QString("labelNetworkBus%1")
                                   .arg(Utility::generateUniqueObjectId()));
    Utility::fillLabelWithImage(labelNetworkBus, kNetworkBusLabelWidth, kNetworkBusLabelHeight,
                                QString(":/images/network_bus.gif"));
    layoutNetworkBusItems_->addWidget(labelNetworkBus);
    layoutNetworkBusItems_->setAlignment(labelNetworkBus, Qt::AlignHCenter | Qt::AlignTop);
}

void MSCMonitorForm::addLIUItem(int deviceId, int deviceType, const QString &liuItemName)
{
    Q_ASSERT((deviceType == 0x00) || (deviceType == 0x01));

    auto lblLIUItem = new CustomLIULabel(liuItemName, widgetScrollAreaContent_);
    layoutLIUItems_->insertWidget(layoutLIUItems_->count() - 1, lblLIUItem);
    layoutLIUItems_->setAlignment(lblLIUItem, Qt::AlignHCenter | Qt::AlignTop);

    if (!mapDevs_.contains(deviceId)) {
        mapDevs_.insert(deviceId, new Device(deviceType, lblLIUItem));
    }
}

void MSCMonitorForm::checkChildWidgetsSizeToScroll(int formWidth, int formHeight)
{
    int totalChildWidgetsWidth = kSvrLabelWidth + kNetworkBusLabelWidth + kLIULabelWidth
            + kFormMargin * 2;
    if (totalChildWidgetsWidth > formWidth)
        widgetScrollAreaContent_->setMinimumWidth(totalChildWidgetsWidth);

    int totalChildWidgetsHeight = 0;
    if (liuItemCount_ <= kDefaultLIUItemsNumber)
        totalChildWidgetsHeight = kFormMargin + kNetworkBusLabelHeight;
    else
        totalChildWidgetsHeight = kFormMargin + kLIULabelHeigth * liuItemCount_
                + kLIUItemsSpace * (liuItemCount_ - 1);

    if (totalChildWidgetsHeight > formHeight)
        widgetScrollAreaContent_->setMinimumHeight(totalChildWidgetsHeight);
}

void MSCMonitorForm::updateDeviceState(int deviceId, int slotIndex, int type, int state)
{
    int key = (deviceId << 16) | slotIndex;
    if (!mapDevStateInfos_.contains(key)) {
        mapDevStateInfos_.insert(key, new DeviceState(true, deviceId, slotIndex, type, state));
    } else {
        auto stateInfo = mapDevStateInfos_.value(key);
        stateInfo->isChanged = true;
        stateInfo->cardType = type;
        stateInfo->runningState = state;
    }
}

void MSCMonitorForm::updateEthPortsState(int deviceId, int mpuFlag, int port1State, int port2State)
{
    int slotIndex = (mpuFlag == 0x00) ? 8 : 9;
    int key = (deviceId << 16) | slotIndex;
    if (!mapDevStateInfos_.contains(key)) {
        LOG_ERROR("cannot find the SVR or LIU's MPU card, deviceId:%d, mpuFlag:%d",
                  deviceId, mpuFlag);
        return;
    }

    auto stateInfo = mapDevStateInfos_.value(key);
    stateInfo->isChanged = true;
    if (stateInfo->ethPortsState == nullptr)
        stateInfo->ethPortsState  = new EthPortsState;
    stateInfo->ethPortsState->mpuFlag = mpuFlag;
    stateInfo->ethPortsState->port1State = port1State;
    stateInfo->ethPortsState->port2State = port2State;
}

void MSCMonitorForm::updateUserCardPortState(int deviceId, int slotIndex, int portId, int type,
                                             int state)
{
    int key = (deviceId << 16) | slotIndex;
    if (!mapDevStateInfos_.contains(key)) {
        LOG_ERROR("cannot find the user card, deviceId:%d, slotIndex:%d", deviceId, slotIndex);
        return;
    }

    auto stateInfo = mapDevStateInfos_.value(key);
    stateInfo->isChanged = true;
    auto &cardPortStates = stateInfo->cardPortStates;
    if (cardPortStates[portId] != nullptr) {
        cardPortStates[portId]->type = type;
        cardPortStates[portId]->state = state;
    } else {
        cardPortStates[portId] = new CardPortState{ portId, type, state };
    }
}

void MSCMonitorForm::onUpdateDeviceStateTimer()
{
    foreach (auto item, mapDevStateInfos_) {
        if (item->isChanged && mapDevs_.contains(item->deviceId)) {
            item->isChanged = false;
            auto device = mapDevs_.value(item->deviceId);
            if (device->type == 0) {
                device->lblSvrItem->updateRunningState(item->runningState);
                if (item->ethPortsState != nullptr)
                    device->lblSvrItem->updateEthPortsState(item->ethPortsState->port1State,
                                                            item->ethPortsState->port2State);
            } else if (device->type == 1) {
                device->lblLIUItem->updateCardRunningState(item->slotIndex, item->cardType,
                                                           item->runningState);
                if (item->ethPortsState != nullptr)
                    device->lblLIUItem->updateCardEthPortsState(item->ethPortsState->mpuFlag,
                                                                item->ethPortsState->port1State,
                                                                item->ethPortsState->port2State);
                auto &cardPortStates = item->cardPortStates;
                for (int i = 0; i < 2; i++) {
                    if (cardPortStates[i] != nullptr) {
                        device->lblLIUItem->updateCardPortState(item->slotIndex,
                                                                cardPortStates[i]->portId,
                                                                cardPortStates[i]->type,
                                                                cardPortStates[i]->state);
                    }
                }
            }
        }
    }
}
