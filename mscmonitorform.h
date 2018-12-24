#ifndef MSCMONITORFORM_H
#define MSCMONITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>

class CustomSvrLabel;
class CustomLIULabel;
class QBoxLayout;
class QGridLayout;
class QScrollArea;
class QLabel;
class QTimer;
struct Device;
struct DeviceState;

class MSCMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    kFormDefaultWidth = 1440,
    kFormDefaultHeight = 758,
    kFormMargin = 20,

    kSvrLabelWidth = 165,
    kSvrLabelHeight = 165,
    kSvrHStretch = 3,
    kNetworkBusLabelWidth = 110,
    kNetworkBusLabelHeight = kFormDefaultHeight - kFormMargin,
    kNetworkBusHStretch = 2,
    kLIULabelWidth = 1126,
    kLIULabelHeigth = 220,
    kLIUItemsHStretch = 20,
    kLIUItemsSpace = 25,
    kDefaultLIUItemsNumber = 3,

    kDeviceStateTimerInterval = 1000 // 设备状态更新定时器
};

public:
    explicit MSCMonitorForm(QWidget *parent = nullptr);
    ~MSCMonitorForm();

signals:

public slots:
    void onHeartbeatTimeout();
    void onReportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName);
    void onReportMainCardState(char deviceId, char mpuFlag, char state);
    void onReportUserCardState(char deviceId, char slotIndex, char type, char state);
    void onReportEthPortsState(char deviceId, char mpuFlag, char port1State, char port2State);
    void onReportUserCardPortState(char deviceId, char slotIndex, char portId, char type, char state);

    void onUpdateDeviceStateTimer();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initFormContentsLayout();

    void addSvrItem(int deviceId, int deviceType, const QString &svrItemName);
    void addNetworkBusItem();
    void addLIUItem(int deviceId, int deviceType, const QString &liuItemName);

    void checkChildWidgetsSizeToScroll(int formWidth, int formHeight);

    void updateDeviceState(int deviceId, int slotIndex, int type, int state);
    void updateEthPortsState(int deviceId, int mpuFlag, int port1State, int port2State);
    void updateUserCardPortState(int deviceId, int slotIndex, int portId, int type, int state);

private:
    QScrollArea *scrollArea_;
    QWidget *widgetScrollAreaContent_;
    QBoxLayout *layoutcontents_;
    QBoxLayout *layoutSvrItems_;
    QBoxLayout *layoutNetworkBusItems_;
    QBoxLayout *layoutLIUItems_;

    QTimer *devStateTimer_;
    int formWidth_ = 0;
    int formHeight_ = 0;
    int liuItemCount_ = 0;
    // key表示设备号, value表示SVR或LIU设备
    QMap<int, Device *> mapDevs_;
    // key高字为设备号，低字为槽位号，value表示SVR或LIU板卡状态信息
    QMap<int, DeviceState *> mapDevStateInfos_;
};

#endif // MSCMONITORFORM_H
