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
        kLIULabelWidth = 1126,
        kLIULabelHeigth = 220,
        kLIUItemsSpace = 25,

        kLIUContentFirstRowHeight = 22,
        kLIUContentLastRowHeight = 178,
        kLIUContentNameColumnWidth = 190,
        kLIUContentCardColumnWidth = 52,

        kCardStateTimerInterval = 500 // 板卡状态更新定时器
    };

public:
    CustomLIULabel(const QString &liuName, QWidget *parent = nullptr);

    void updateCardRunningState(int slotIndex, int cardType, int runningState);
    void updateCardPortState(int slotIndex, int portId, int type, int state);
    void updateCardEthPortsState(int mpuFlag, int port1State, int port2State);

private:
    void initContentsLayout(const QString &liuName);

private slots:
    void onUpdateCardStateTimer();

private:
    QGridLayout *layoutContents_;
    QLabel *lblLIUName_;
    QVector<CustomCardLabel *> vecLIUCards_;

    QTimer *cardStateTimer_;
};

#endif // CUSTOMLIULABEL_H
