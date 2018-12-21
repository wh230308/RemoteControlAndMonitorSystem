#include "mscmonitorform.h"

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
#include "customliulabel.h"
#include "customcardlabel.h"


MSCMonitorForm::MSCMonitorForm(QWidget *parent) : QWidget(parent)
{
    initFormContentsLayout();

    deviceStateTimer = new QTimer(this);
    connect(deviceStateTimer, SIGNAL(timeout()), this, SLOT(onUpdateDeviceStateTimer()));
    deviceStateTimer->start(DeviceStateTimerInterval);
}

MSCMonitorForm::~MSCMonitorForm()
{
    if (deviceStateTimer != nullptr)
        deviceStateTimer->stop();
}

void MSCMonitorForm::onHeartbeatTimeout()
{
}

void MSCMonitorForm::onReportMainCardState(char deviceId, char slotIndex, char state)
{
    LOG_TRACE("main card state changed, deviceId:%d, slotIndex:%d, state:%d",
              deviceId, slotIndex, state);

    int mainCardSlotIndex = 0;
    if (slotIndex == 0x00)
        mainCardSlotIndex = 0x08;
    else if (slotIndex == 0x01)
        mainCardSlotIndex = 0x09;

    updateCardRunningState(deviceId, mainCardSlotIndex, state, 0x00);
}

void MSCMonitorForm::onReportUserCardState(char deviceId, char slotIndex, char state, char type)
{
    LOG_TRACE("user card state changed, deviceId:%d, slotIndex:%d, state:%d, type:%d",
              deviceId, slotIndex, state, type);

    // 界面布局板卡时，主控板和用户板排成一排，共18个槽位，主控板占据第8、9槽位，其余槽位由用户板占用。
    // 而上报的用户板槽位号是从0~15，因此用户板槽位号大于8时往右偏移2个位置
    if (slotIndex > 0x08)
        slotIndex += 0x02;
    updateCardRunningState(deviceId, slotIndex, state, type);
}

void MSCMonitorForm::onReportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName)
{
    LOG_TRACE("report the device info, deviceId:%d, deviceType:%d, deviceName:%s",
              deviceId, deviceType, deviceName.constData());

    if (deviceList.contains(deviceId))
        return;

    if (deviceType == 0) {
        addSvrItem(deviceId, deviceType, QString::fromUtf8(deviceName));
    } else if (deviceType == 1) {
        addLIUItem(deviceId, deviceType, QString::fromUtf8(deviceName));
        checkChildWidgetsSizeToScroll(formWidth, formHeight);
    }
}

void MSCMonitorForm::onReportMPUNetworkPortsState(char mpuIndex, char deviceId, char port1State,
                                                  char port2State)
{
    LOG_TRACE("report the mpu network port state, mpuIndex:%d, deviceId:%d, port1State:%d, port2State:%d",
              mpuIndex, deviceId, port1State, port2State);

    updateMPUNetworkPortsState(mpuIndex, deviceId, port1State, port2State);
}

void MSCMonitorForm::onReportUserCardPortState(char portId, char deviceId, char slotIndex,
                                               char state, char type)
{
    LOG_TRACE("report the user card port state, portId:%d, deviceId:%d, slotIndex:%d, state:%d, type:%d",
              static_cast<int>(portId), deviceId, slotIndex, state, type);

    updateUserCardPortState(static_cast<int>(portId), deviceId, slotIndex, state, type);
}

void MSCMonitorForm::resizeEvent(QResizeEvent *event)
{
    scrollAreaWidgetContents->setGeometry(0, 0, scrollArea->width(), scrollArea->height());

    formWidth = event->size().width();
    formHeight = event->size().height();
    qDebug() << tr("Form size:(%1*%2)").arg(formWidth).arg(formHeight);
    checkChildWidgetsSizeToScroll(formWidth, formHeight);
}

void MSCMonitorForm::initFormContentsLayout()
{
    auto topLevelLayout = new QHBoxLayout(this);
    topLevelLayout->setObjectName(QStringLiteral("topLevelLayout"));
    topLevelLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QStringLiteral("scrollArea"));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setAlignment(Qt::AlignCenter);
    topLevelLayout->addWidget(scrollArea);

    scrollAreaWidgetContents = new QWidget();
    scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
    scrollAreaWidgetContents->setGeometry(0, 0, scrollArea->width(), scrollArea->height());
    scrollArea->setWidget(scrollAreaWidgetContents);

    contentsLayout = new QHBoxLayout(scrollAreaWidgetContents);
    contentsLayout->setObjectName(QStringLiteral("contentsLayout"));
    contentsLayout->setContentsMargins(FormMargin, FormMargin, FormMargin, 0);
    contentsLayout->setSpacing(0);

    // svr布局
    svrItemsLayout = new QVBoxLayout();
    svrItemsLayout->setObjectName(QStringLiteral("svrItemsLayout"));
    svrItemsLayout->setContentsMargins(0, 0, 0, 0);
    svrItemsLayout->setSpacing(50);
    contentsLayout->addLayout(svrItemsLayout, SvrHStretch);

    int topSpacerMinH = NetworkBusLabelHeight / 3 - (SvrLabelHeight / 2);
    auto svrTopSpacer = new QSpacerItem(10, topSpacerMinH, QSizePolicy::Minimum, QSizePolicy::Minimum);
    svrItemsLayout->addSpacerItem(svrTopSpacer);

    auto svrBottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    svrItemsLayout->addSpacerItem(svrBottomSpacer);

    // network bus布局
    networkBusItemsLayout = new QVBoxLayout();
    networkBusItemsLayout->setObjectName(QStringLiteral("networkBusItemsLayout"));
    networkBusItemsLayout->setContentsMargins(0, 0, 0, 0);
    networkBusItemsLayout->setSpacing(0);
    contentsLayout->addLayout(networkBusItemsLayout, NetworkBusHStretch);

    addNetworkBusItem();

    // LIU布局
    liuItemsLayout = new QVBoxLayout();
    liuItemsLayout->setObjectName(QStringLiteral("liuItemsLayout"));
    liuItemsLayout->setContentsMargins(0, 0, 0, 0);
    liuItemsLayout->setSpacing(LIUItemsSpace);
    contentsLayout->addLayout(liuItemsLayout, LIUItemsHStretch);

    auto bottomSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    liuItemsLayout->addSpacerItem(bottomSpacer);
}

void MSCMonitorForm::addSvrItem(int deviceId, int deviceType, const QString &svrName)
{
    Q_ASSERT((deviceType == 0x00) || (deviceType == 0x01));

    auto labelSvr = new QLabel(scrollAreaWidgetContents);
    labelSvr->setObjectName(QString("labelSvr%1").arg(Utility::generateUniqueObjectId()));
    Utility::fillLabelWithImage(labelSvr, SvrLabelWidth, SvrLabelHeight, QString(":/images/svr.gif"));
    svrItemsLayout->insertWidget(svrItemsLayout->count() - 1, labelSvr);
    svrItemsLayout->setAlignment(labelSvr, Qt::AlignHCenter | Qt::AlignTop);

    auto svrInnerLayout = new QVBoxLayout(labelSvr);
    svrInnerLayout->setObjectName(QString("svrInnerLayout%1").arg(Utility::generateUniqueObjectId()));
    svrInnerLayout->setContentsMargins(9, 9, 9, 3);

    auto labelSvrName = new QLabel(QString("<h2>%1</h2>").arg(svrName), labelSvr);
    labelSvrName->setObjectName(QString("labelSvrName%1").arg(Utility::generateUniqueObjectId()));
    svrInnerLayout->addWidget(labelSvrName);
    svrInnerLayout->setAlignment(labelSvrName, Qt::AlignHCenter | Qt::AlignBottom);

    auto device = new Device;
    device->type = deviceType;
    device->deviceItem.svrItem = new SvrItem{ labelSvr, labelSvrName };
    deviceList.insert(deviceId, device);
}

void MSCMonitorForm::addNetworkBusItem()
{
    auto labelNetworkBus = new QLabel(scrollAreaWidgetContents);
    labelNetworkBus->setObjectName(QString("labelNetworkBus%1")
                                   .arg(Utility::generateUniqueObjectId()));
    Utility::fillLabelWithImage(labelNetworkBus, NetworkBusLabelWidth, NetworkBusLabelHeight,
                                QString(":/images/network_bus.gif"));
    networkBusItemsLayout->addWidget(labelNetworkBus);
    networkBusItemsLayout->setAlignment(labelNetworkBus, Qt::AlignHCenter | Qt::AlignTop);
}

void MSCMonitorForm::addLIUItem(int deviceId, int deviceType, const QString &liuItemName)
{
    Q_ASSERT((deviceType == 0x00) || (deviceType == 0x01));

    auto liuItem = new CustomLIULabel(liuItemName, scrollAreaWidgetContents);
    liuItemsLayout->insertWidget(liuItemsLayout->count() - 1, liuItem);
    liuItemsLayout->setAlignment(liuItem, Qt::AlignHCenter | Qt::AlignTop);

    if (!deviceList.contains(deviceId)) {
        auto device = new Device;
        device->type = deviceType;
        device->deviceItem.liuItem = liuItem;
        deviceList.insert(deviceId, device);

        liuItemCount++;
    } else {
        auto device = deviceList.value(deviceId);
        device->deviceItem.liuItem = liuItem;
    }
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

    qDebug() << "scrollAreaWidgetContents height:" << scrollAreaWidgetContents->height();
    if (totalChildWidgetsHeight > formHeight)
        scrollAreaWidgetContents->setMinimumHeight(totalChildWidgetsHeight);
}

void MSCMonitorForm::updateCardRunningState(int deviceId, int slotIndex, int state, int type)
{
    Q_ASSERT(slotIndex >= 00);
    Q_ASSERT(slotIndex <= CustomLIULabel::kCardNumberPerLIU);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    Q_ASSERT(type >= 0x00);
    Q_ASSERT(type <= 0x10);

    int key = (deviceId << 16) | slotIndex;
    if (!deviceStateList.contains(key)) {
        deviceStateList.insert(key, new DeviceState{ true, deviceId, slotIndex, state, type,
                                                     QMap<int, DeviceState::PortState *>() });
    } else {
        auto deviceState = deviceStateList.value(key);
        deviceState->runningState = state;
        deviceState->cardType = type;
    }
}

void MSCMonitorForm::updateMPUNetworkPortsState(int mpuIndex, int deviceId, int port1State,
                                                int port2State)
{
    Q_ASSERT((mpuIndex == 0x00) || (mpuIndex == 0x01));
    Q_ASSERT((port1State == 0x00) || (port1State == 0x01));
    Q_ASSERT((port2State == 0x00) || (port2State == 0x01));

    char slotIndex = 0x00;
    if (mpuIndex == 0x00)
        slotIndex = 0x08;
    else
        slotIndex = 0x09;

    int key = (deviceId << 16) | slotIndex;
    if (!deviceStateList.contains(key)) {
        LOG_ERROR("cannot find MPU card, mpuIndex:%d, deviceId:%d", mpuIndex, deviceId);
        return;
    }

    auto deviceState = deviceStateList.value(key);
    deviceState->isStateChanged = true;
    deviceState->portStateList.insert(0, new DeviceState::PortState{ 0x00, port1State });
    deviceState->portStateList.insert(1, new DeviceState::PortState{ 0x00, port2State });
}

void MSCMonitorForm::updateUserCardPortState(int portId, int deviceId, int slotIndex, int state,
                                             int type)
{
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId <= CustomCardLabel::kMaximumPortId);
    Q_ASSERT(slotIndex >= 00);
    Q_ASSERT(slotIndex <= CustomLIULabel::kCardNumberPerLIU);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    Q_ASSERT(type >= 0x00);
    Q_ASSERT(type <= 0x0a);

    int key = (deviceId << 16) | slotIndex;
    if (!deviceStateList.contains(key)) {
        LOG_ERROR("cannot find User card, portId:%d, deviceId:%d, slotIndex:%d, state:%d, type:%d",
                  portId, deviceId, slotIndex, state, type);
        return;
    }

    auto deviceState = deviceStateList.value(key);
    deviceState->isStateChanged = true;
    if (!deviceState->portStateList.contains(portId)) {
        deviceState->portStateList.insert(portId, new DeviceState::PortState{ type, state });
    } else {
        auto portState = deviceState->portStateList.value(portId);
        portState->type = type;
        portState->state = state;
    }
}

void MSCMonitorForm::onUpdateDeviceStateTimer()
{
    foreach (auto item, deviceStateList) {
        if (deviceList.contains(item->deviceId)) {
            auto device = deviceList.value(item->deviceId);
            if (device->type == 0x00) {
            } else if (device->type == 0x01) {
                auto liuItem = device->deviceItem.liuItem;
                if (item->isStateChanged) {
                    item->isStateChanged = false;
                    liuItem->updateCardStateInfo(item->slotIndex, item->runningState,
                                                    item->cardType);
                    /*
                    auto iter = item->portStateList.constBegin();
                    for (; iter != item->portStateList.constEnd(); iter++) {
                        liuItem->updateCardPortState(item->slotIndex, iter.key(),
                                                     iter.value()->state, iter.value()->type);
                    }
                    */
                }
            }
        }
    }
}
