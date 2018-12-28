#include "mscmonitorform.h"

#include <algorithm>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollArea>
#include <QLabel>
#include <QResizeEvent>
#include <QTimer>
#include <QGroupBox>
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
    Q_ASSERT((deviceType == 0x00) || (deviceType == 0x01));
    LOG_TRACE("report the device info, deviceId:%d, deviceType:%d, deviceName:%s",
              deviceId, deviceType, deviceName.constData());

    if (mapDevs_.contains(deviceId))
        return;

    if (deviceType == 0) {
        addSvrItem(deviceId, deviceType, QString::fromUtf8(deviceName));
    } else if (deviceType == 1) {
        setFormVerticalScroll();
        addLIUItem(deviceId, deviceType, QString::fromUtf8(deviceName));
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
    slotIndex = (slotIndex > 0x07) ? (slotIndex + 0x02) : slotIndex;
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

    slotIndex = (slotIndex > 0x07) ? (slotIndex + 0x02) : slotIndex;
    updateUserCardPortState(deviceId, slotIndex, static_cast<int>(portId),  type, state);
}

void MSCMonitorForm::resizeEvent(QResizeEvent *event)
{
    int width = event->size().width();
    if (width > formWidth_) {
        auto hFactor_ = static_cast<double>(width) / formWidth_;
        formWidth_ *= hFactor_;
        svrLabelWidth_ *= hFactor_;
        networkBusLabelWidth_ *= hFactor_;
        liuLabelWidth_ *= hFactor_;
    } else if (width < formWidth_) {
        scrollAreaWidgetContents_->setMinimumWidth(formWidth_);
    }

    int height = event->size().height();
    if (height > formHeight_) {
        auto vFactor_ = static_cast<double>(height) / formHeight_;
        formHeight_ *= vFactor_;
        svrLabelHeight_ *= vFactor_;
        svrItemsSpace_ *= vFactor_;
        networkBusLabelHeight_ *= vFactor_;
        liuLabelHeight_ *= vFactor_;
        liuItemsSpace_ *= vFactor_;
    } else if (height < formHeight_) {
        scrollAreaWidgetContents_->setMinimumHeight(formHeight_);
    }
}

void MSCMonitorForm::initFormContentsLayout()
{
    // 添加Form的顶层水平布局器，放置QScrollArea控件
    auto layoutFormContents = new QHBoxLayout(this);
    layoutFormContents->setObjectName(QStringLiteral("layoutFormContents"));
    layoutFormContents->setContentsMargins(0, 0, 0, 0);
    layoutFormContents->setSpacing(0);

    // QScrollArea控件及其内容Widget控件
    auto scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QStringLiteral("scrollArea"));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setWidgetResizable(true);
    layoutFormContents->addWidget(scrollArea);

    scrollAreaWidgetContents_ = new QWidget();
    scrollAreaWidgetContents_->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
    scrollArea->setWidget(scrollAreaWidgetContents_);

    // 添加内容Widget控件的水平布局器，放置SVR、Bus、LIU等监控项QLabel控件
    auto layoutWidgetContents = new QHBoxLayout(scrollAreaWidgetContents_);
    layoutWidgetContents->setObjectName(QStringLiteral("layoutWidgetContents"));
    layoutWidgetContents->setContentsMargins(20, 0, 20, 0);
    layoutWidgetContents->setSpacing(0);

    // 添加放置SVR的垂直子布局
    layoutSvrContents_ = new QVBoxLayout();
    layoutSvrContents_->setObjectName(QStringLiteral("layoutSvrContents"));
    layoutSvrContents_->setContentsMargins(0, 0, 0, 0);
    layoutSvrContents_->setSpacing(svrItemsSpace_);
    layoutWidgetContents->addLayout(layoutSvrContents_, svrLabelWidth_);

    int svrTopSpacerMinH = (networkBusLabelHeight_ / 3) - (svrLabelHeight_ / 2);
    // SVR顶部和底部空白条控件
    auto svrTopSpacer = new QSpacerItem(10, svrTopSpacerMinH);
    layoutSvrContents_->addSpacerItem(svrTopSpacer);
    int svrBottomSpacerMinH = (networkBusLabelHeight_ * 2 / 3) + (svrLabelHeight_ / 2);
    auto svrBottomSpacer = new QSpacerItem(10, svrBottomSpacerMinH);
    layoutSvrContents_->addSpacerItem(svrBottomSpacer);

    // 添加放置Bus的垂直子布局
    layoutNetworkBusContents_ = new QVBoxLayout();
    layoutNetworkBusContents_->setObjectName(QStringLiteral("layoutNetworkBusContents"));
    layoutNetworkBusContents_->setContentsMargins(0, 0, 0, 0);
    layoutNetworkBusContents_->setSpacing(0);
    layoutWidgetContents->addLayout(layoutNetworkBusContents_, networkBusLabelWidth_);

    addNetworkBusItem();

    // 添加放置LIU的垂直子布局
    layoutLIUContents_ = new QVBoxLayout();
    layoutLIUContents_->setObjectName(QStringLiteral("layoutLIUContents"));
    layoutLIUContents_->setContentsMargins(0, liuItemsSpace_, 0, 0);
    layoutLIUContents_->setSpacing(liuItemsSpace_);
    layoutWidgetContents->addLayout(layoutLIUContents_, liuLabelWidth_);

    // LIU底部空白条控件
    int liuBottomSpacerMaxH = formHeight_ - liuItemsSpace_ - liuLabelHeight_;
    auto liuBottomSpacer = new QSpacerItem(10, liuBottomSpacerMaxH, QSizePolicy::Minimum,
                                           QSizePolicy::Maximum);
    layoutLIUContents_->addSpacerItem(liuBottomSpacer);
}

void MSCMonitorForm::setFormVerticalScroll()
{
    if (++liuItemCount_ > kDefaultLIUItemsNum) {
        formHeight_ = kLIUUnitHeight * liuItemCount_ +  liuItemsSpace_ * (liuItemCount_ + 1);
        scrollAreaWidgetContents_->setMinimumHeight(formHeight_);
    }
}

void MSCMonitorForm::addSvrItem(int deviceId, int deviceType, const QString &svrItemName)
{
    auto lblSvrItem = new CustomSvrLabel(svrLabelWidth_, svrLabelHeight_, svrItemName,
                                         scrollAreaWidgetContents_);
    layoutSvrContents_->insertWidget(layoutSvrContents_->count() - 1, lblSvrItem);
    layoutSvrContents_->setAlignment(lblSvrItem, Qt::AlignHCenter | Qt::AlignTop);

    if (!mapDevs_.contains(deviceId)) {
        mapDevs_.insert(deviceId, new Device(deviceType, lblSvrItem));
    }
}

void MSCMonitorForm::addNetworkBusItem()
{
    auto labelNetworkBus = new QLabel(scrollAreaWidgetContents_);
    labelNetworkBus->setObjectName(QString("labelNetworkBus%1")
                                   .arg(Utility::generateUniqueObjectId()));
    Utility::fillLabelWithImage(labelNetworkBus, networkBusLabelWidth_, networkBusLabelHeight_,
                                QString(":/images/network_bus.gif"));
    layoutNetworkBusContents_->addWidget(labelNetworkBus);
    layoutNetworkBusContents_->setAlignment(labelNetworkBus, Qt::AlignHCenter | Qt::AlignTop);
}

void MSCMonitorForm::addLIUItem(int deviceId, int deviceType, const QString &liuItemName)
{
    auto groupBox = new QGroupBox(tr("Frame:%1").arg(liuItemName), scrollAreaWidgetContents_);
    groupBox->setObjectName(QString("groupBox%1").arg(Utility::generateUniqueObjectId()));
    layoutLIUContents_->insertWidget(layoutLIUContents_->count() - 1, groupBox);

    auto layoutContents = new QHBoxLayout(groupBox);
    layoutContents->setObjectName(QString("layoutContents%1").arg(Utility::generateUniqueObjectId()));
    layoutContents->setContentsMargins(5, 5, 5, 5);

    auto lblLIUItem = new CustomLIULabel(liuLabelWidth_, liuLabelHeight_, scrollAreaWidgetContents_);
    layoutContents->addWidget(lblLIUItem);

    if (!mapDevs_.contains(deviceId)) {
        mapDevs_.insert(deviceId, new Device(deviceType, lblLIUItem));
    }
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
