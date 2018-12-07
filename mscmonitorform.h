#ifndef MSCMONITORFORM_H
#define MSCMONITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>

class CustomLIULabel;
class QBoxLayout;
class QGridLayout;
class QScrollArea;
class QLabel;
class QTimer;

class MSCMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    FormDefaultWidth = 1440,
    FormDefaultHeight = 758,
    FormMargin = 20,

    SvrLabelWidth = 165,
    SvrLabelHeight = 165,
    SvrHStretch = 3,
    NetworkBusLabelWidth = 110,
    NetworkBusLabelHeight = FormDefaultHeight - FormMargin,
    NetworkBusHStretch = 2,
    LIULabelWidth = 1126,
    LIULabelHeigth = 220,
    LIUItemsHStretch = 20,
    LIUItemsSpace = 25,
    DefaultLIUItemsNumber = 3,

    DeviceStateTimerInterval = 500 // 设备状态更新定时器
};

struct SvrItem
{
    QLabel *labelSvr;
    QLabel *labelSvrName;
};

struct DeviceState
{
    bool isStateChanged;

    int deviceId; // 标识唯一的SVR或LIU
    int slotIndex; // LIU板卡槽位号，仅设备为LIU时有效
    int runningState; // SVR或LIU运行状态：0离线；1在线
    int cardType; // LIU板卡类型，仅设备为LIU且板卡为用户卡时有效

    struct PortState
    {
        int type; // 端口类型，仅设备为LIU且板卡为用户卡时有效
        int state; // 端口状态
    };

    // 端口状态列表，当设备为LIU-MPU板时，表示网口1和网口2状态；
    // 当设备为LIU用户板时表示各种类型端口
    QVector<PortState *> portStateList;
};

struct Device
{
    int type;
    union DeviceItem
    {
        SvrItem *svrItem;
        CustomLIULabel *liuItem;
    };

    DeviceItem deviceItem;
};

public:
    explicit MSCMonitorForm(QWidget *parent = nullptr);
    ~MSCMonitorForm();

signals:

public slots:
    void onHeartbeatTimeout();
    void onReportMainCardState(char deviceId, char slotIndex, char state);
    void onReportUserCardState(char deviceId, char slotIndex, char state, char type);
    void onReportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName);
    void onReportMPUNetworkPortsState(char mpuIndex, char deviceId, char port1State, char port2State);
    void onReportUserCardPortState(char portId, char deviceId, char slotIndex, char state, char type);

    void onUpdateDeviceStateTimer();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initFormContentsLayout();

    void addSvrItem(int deviceId, int deviceType, const QString &svrName);
    void addNetworkBusItem();
    void addLIUItem(int deviceId, int deviceType, const QString &liuItemName);

    void checkChildWidgetsSizeToScroll(int formWidth, int formHeight);

    void updateCardRunningState(int deviceId, int slotIndex, int state, int type);
    void updateMPUNetworkPortsState(int mpuIndex, int deviceId, int port1State, int port2State);
    void updateUserCardPortState(int portId, int deviceId, int slotIndex, int state, int type);

private:
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QBoxLayout *contentsLayout;
    QBoxLayout *svrItemsLayout;
    QBoxLayout *networkBusItemsLayout;
    QBoxLayout *liuItemsLayout;

    QTimer *deviceStateTimer;
    int formWidth = 0;
    int formHeight = 0;
    int liuItemCount = 0;
    QMap<int, Device *> deviceList;
    QMap<int, DeviceState *> deviceStateList;
};

#endif // MSCMONITORFORM_H
