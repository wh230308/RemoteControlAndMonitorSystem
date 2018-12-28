#ifndef CUSTOMLIULABEL_H
#define CUSTOMLIULABEL_H

#include <QWidget>
#include <QLabel>
#include <QVector>

class QGridLayout;
class CustomCardLabel;
class QTimer;

class CustomLIULabel : public QLabel
{
    Q_OBJECT

public:
    enum
    {
        kCardNumberPerLIU = 18,
        kLIUContentsFirstRowHeight = 22,
        kLIUContentsLastRowHeight = 188,

        kCardStateTimerInterval = 500 // 板卡状态更新定时器
    };

public:
    CustomLIULabel(int width, int height, QWidget *parent = nullptr);
    ~CustomLIULabel();

    void updateCardRunningState(int slotIndex, int cardType, int runningState);
    void updateCardPortState(int slotIndex, int portId, int type, int state);
    void updateCardEthPortsState(int mpuFlag, int port1State, int port2State);

private:
    void initContentsLayout(int width, int height);

private slots:
    void onUpdateCardStateTimer();

private:
    QGridLayout *layoutContents_;
    QVector<CustomCardLabel *> vecLIUCards_;

    QTimer *cardStateTimer_;
};

#endif // CUSTOMLIULABEL_H
