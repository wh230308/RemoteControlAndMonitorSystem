#ifndef MSCMONITORFORM_H
#define MSCMONITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>

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
    DefaultSvrItemsNumber = 2,
    DefaultLIUItemsNumber = 3,
    CardNumberPerLIUItem = 18,

    FormDefaultWidth = 1440,
    FormDefaultHeight = 900,

    SvrLabelWidth = 165,
    SvrLabelHeight = 165,
    LIULabelWidth = 1126,
    LIULabelHeigth = 220,
    NetworkBusLabelWidth = 110,
    NetworkBusLabelHeight = LIULabelHeigth * 3 + LIULabelHeigth / 2,
    FormMargin = 20,
    LIUItemsSpace = 25,

    LIUItemLabelNameHStretch = 190,
    CardLabelHStretch = 52,
    CardLabelWidth = 51,
    CardLabelHeight = 178,

    CardStateLampLabelWidth = 8,
    CardStateLampLabelHeight = 8,

    DeviceStateTimerInterval = 500 // 设备状态更新定时器
};

struct SvrItem
{
    QLabel *labelSvr;
    QLabel *labelSvrName;
};

struct LIUCardState
{
    char deviceId; // 板卡在哪个LIU上
    char slotIndex; // 板卡在LIU的哪个槽位上
    char state; // 板卡状态：0离线；1在线
    char type; // 板卡类型：0x00 ASL; 0x01 ALT;...具体参见协议文档，MPU板无类型用0x7f标识
};

struct LIUCard
{
    QLabel *labelLIUCard;
    QLabel *labelTypeName;
    QLabel *labelRunningStateLamp;
    QLabel *labelCardPort1Lamp;
    QLabel *labelCardPort2Lamp;
};

struct LIUItem
{
    QLabel *labelLIU;
    QLabel *labelLIUName;
    QVector<LIUCard *> liuCardList;
};

struct Device
{
    int type;
    union DeviceItem
    {
        SvrItem *svrItem;
        LIUItem *liuItem;
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

    void onUpdateCardStateTimer();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initSvrItemsLayout();
    void initNetworkBusLayout();
    void initLIUItemsLayout();

    void addSvrItem(char deviceId, char deviceType, const QString &svrName);
    void addNetworkBusItem();
    void addLIUItem(char deviceId, char deviceType, const QString &liuItemName);
    void addCardsToLIUItem(QLabel *labelLIUItem, QLabel *labelLIUItemName,
                           QVector<LIUCard *> &liuCardList);
    void initCardLayout(int index, QWidget *parent, QGridLayout *parentLayout,
                        QVector<LIUCard *> &liuCardList);
    void checkChildWidgetsSizeToScroll(int formWidth, int formHeight);

    void updateCardState(char deviceId, char slotIndex, char state, char type);

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
    QVector<LIUCardState *> liuCardStateList;
};

#endif // MSCMONITORFORM_H
