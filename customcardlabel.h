#ifndef CUSTOMCARDLABEL_H
#define CUSTOMCARDLABEL_H

#include <QWidget>
#include <QLabel>
#include <QMap>

class QGridLayout;

class CustomCardLabel : public QLabel
{
    Q_OBJECT

public:
    enum
    {
        kCardLabelWidth = 51,
        kCardLabelHeight = 178,
        kCardStateLampLabelWidth = 8,
        kCardStateLampLabelHeight = 8,

        kCardContentRowMargin = 36,
        kCardContentRowHeight = 18,
        kCardContentColumnMargin = 8,
        kCardContentLeftColumnWidth = 17,
        kCardContentRightColumnWidth = 17,

        kMaximumPortId = 255,
        kDefaultPortNum = 2
    };

    struct CardPort
    {
        QLabel *lblTypeDesc; // 端口类型描述, 0x00 ASL; 0x01 ALT;...具体参见协议文档
        QLabel *lblStateLamp; // 端口状态灯
    };

public:
    CustomCardLabel(QWidget *parent = nullptr, bool isMPUCard = false);

    void updateCardTypeName(const QString &typeName);
    void updateRunningState(int state);
    void updatePortState(int portId, int type, int state); // 更新端口状态，仅用户板
    void updateEthPortsState(int port1State, int port2State); // 更新网口状态，仅主控板

    bool isCardRunning() const { return runningState_ == 0x01; }
    void flickerRunningStateLamp(int parity);

private:
    void initContentsLayout();
    void addPortStateLayout(int portId, const QString &imgPath);

private:
    bool isMPUCard_;
    QGridLayout *layoutContents_;
    QLabel *lblTypeName_; // 板卡类型名：0x00 ASL; 0x01 ALT;...具体参见协议文档，MPU板无类型用0x7f标识
    QLabel *lblRunningStateLamp_; // 运行状态灯

    // 用户板端口，最多有256个端口，默认显示2个
    QMap<int, CardPort *> mapCardPorts_;
    int runningState_; // 板卡运行状态：0离线；1在线
};

#endif // CUSTOMCARDLABEL_H
