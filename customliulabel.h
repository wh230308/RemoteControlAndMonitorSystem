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
        CardNumberPerLIU = 18,
        LIULabelWidth = 1126,
        LIULabelHeigth = 220,
        LIUItemsSpace = 25,
    };

public:
    CustomLIULabel(uint objectId, const QString &liuName, QWidget *parent = nullptr);

public slots:
    void onUpdateCardStateTimer();
    void updateCardRunningState(int slotIndex, int state, int type);
    void updateCardPortState(int slotIndex, int portId, int state, int type);

private:
    QGridLayout *contenstLayout;
    QLabel *labelLIUName;
    QVector<CustomCardLabel *> liuCardList;

    QTimer *cardStateTimer;
};

#endif // CUSTOMLIULABEL_H
