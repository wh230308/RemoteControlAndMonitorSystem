#ifndef CUSTOMCARDLABEL_H
#define CUSTOMCARDLABEL_H

#include <QWidget>
#include <QLabel>
#include <QVector>

class QGridLayout;

class CustomCardLabel : public QLabel
{
    Q_OBJECT

public:
    enum
    {
        CardLabelWidth = 51,
        CardLabelHeight = 178,
        CardStateLampLabelWidth = 8,
        CardStateLampLabelHeight = 8,

        MaximumPortId = 255
    };

    struct CardPort
    {
        QLabel *labelTypeDesc; // 端口类型描述, 0x00 ASL; 0x01 ALT;...具体参见协议文档
        QLabel *labelStateLamp; // 端口状态灯
    };

public:
    CustomCardLabel(uint objectId, QWidget *parent = nullptr);

    void updateCardTypeName(const QString &typeName);
    void updateRunningState(int state);
    void updatePortState(int portId, int state, int type, bool isMPUCard);

    bool isCardRunning() const { return runningState == 0x01; }
    void flickerRunningStateLamp(int parity);

private:

    QGridLayout *contenstLayout;
    QLabel *labelTypeName; // 板卡类型名：0x00 ASL; 0x01 ALT;...具体参见协议文档，MPU板无类型用0x7f标识
    QLabel *labelRunningStateLamp; // 运行状态灯
    QLabel *labelRunningStateDesc; // 运行状态描述（默认run）

    // 板卡面板端口列表，当板卡为MPU板时，只有2个端口，表示MPU板网口1和2
    // 当板卡为用户板时，最多有256个端口，分别表示各类型端口，如"ASL"、"ALT"、"EM"...
    QVector<CardPort *> cardPortList;

    int runningState; // 板卡运行状态：0离线；1在线
};

#endif // CUSTOMCARDLABEL_H
