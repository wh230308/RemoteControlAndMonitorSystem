#ifndef MSCMONITORFORM_H
#define MSCMONITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>

class QBoxLayout;
class QGridLayout;
class QScrollArea;
class QLabel;

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
    CardStateLampLabelHeight = 8
};

struct SvrItem
{
    QLabel *labelSvr;
    QLabel *labelSvrName;
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


public:
    explicit MSCMonitorForm(QWidget *parent = nullptr);
    ~MSCMonitorForm();

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void initSvrItemsLayout();
    void initNetworkBusLayout();
    void initLIUItemsLayout();

    void addSvrItem(int deviceId, int deviceType, const QString &svrName);
    void addNetworkBusItem();
    void addLIUItem(int deviceId, int deviceType, const QString &liuItemName);
    void addCardsToLIUItem(QLabel *labelLIUItem, QLabel *labelLIUItemName,
                           QVector<LIUCard *> &liuCardList);
    void initCardLayout(int index, QWidget *parent, QGridLayout *parentLayout,
                        QVector<LIUCard *> &liuCardList);

    void checkChildWidgetsSizeToScroll(int formWidth, int formHeight);

private:
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QBoxLayout *contentsLayout;
    QBoxLayout *svrItemsLayout;
    QBoxLayout *networkBusItemsLayout;
    QBoxLayout *liuItemsLayout;

    int formWidth = 0;
    int formHeight = 0;
    int liuItemCount = 0;
    QMap<int, void *> deviceList;
};

#endif // MSCMONITORFORM_H
