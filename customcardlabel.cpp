#include "customcardlabel.h"

#include <QGridLayout>

#include "utility.h"

static const QStringList portTyepDescList = {
    QObject::tr("ASL"),
    QObject::tr("ALT"),
    QObject::tr("EM"),
    QObject::tr("MTK"),
    QObject::tr("ISDN"),
    QObject::tr("DTK"),
    QObject::tr("DSL"),
    QObject::tr("DIU"),
    QObject::tr("RIU"),
    QObject::tr("YCK"),
    QObject::tr("EM4")
};

CustomCardLabel::CustomCardLabel(uint objectId, QWidget *parent) : QLabel(parent)
{
    if (objectName().isEmpty())
        setObjectName(QString("customCardLabel%1").arg(objectId));

    Utility::fillLabelWithImage(this, CardLabelWidth, CardLabelHeight, QString(":/images/card.gif"));
    setAlignment(Qt::AlignCenter);
    setVisible(false); // 板卡默认不显示，当服务器有上报该板卡状态时显示

    contenstLayout = new QGridLayout(this);
    contenstLayout->setObjectName(QString("contenstLayout%1").arg(objectId));
    contenstLayout->setHorizontalSpacing(1);
    contenstLayout->setVerticalSpacing(0);

    // 板卡类型名称描述
    labelTypeName = new QLabel(this);
    labelTypeName->setObjectName(QString("labelTypeName%1").arg(objectId));
    labelTypeName->setAlignment(Qt::AlignCenter);
    contenstLayout->addWidget(labelTypeName, 1, 1, 1, 2, Qt::AlignHCenter | Qt::AlignBottom);


    // 板卡运行状态灯及其描述
    labelRunningStateLamp = new QLabel(this);
    labelRunningStateLamp->setObjectName(QString("labelRunningStateLamp%1").arg(objectId));
    labelRunningStateLamp->setAlignment(Qt::AlignCenter);
    Utility::fillLabelWithImage(labelRunningStateLamp, CardStateLampLabelWidth,
                                CardStateLampLabelHeight, QString(":/images/lamp_off.gif"));
    contenstLayout->addWidget(labelRunningStateLamp, 2, 1, Qt::AlignHCenter | Qt::AlignVCenter);

    labelRunningStateDesc = new QLabel(tr("<b>Run</b>"), this);
    labelRunningStateDesc->setObjectName(QString("cardRunningStateDesc%1").arg(objectId));
    labelRunningStateDesc->setAlignment(Qt::AlignCenter);
    labelRunningStateDesc->setFont(QFont(QString("Microsoft YaHei"), 6, 1));
    contenstLayout->addWidget(labelRunningStateDesc, 2, 2, Qt::AlignHCenter | Qt::AlignVCenter);

    // 布局行数动态增加
    contenstLayout->setRowStretch(0, 36);
    contenstLayout->setRowStretch(1, 20);
    contenstLayout->setRowStretch(2, 20);
    contenstLayout->setRowStretch(3, 102);

    // 布局为4列，第1列第4列留白，第2列第3列布置状态灯及其描述
    contenstLayout->setColumnStretch(0, 8);
    contenstLayout->setColumnStretch(1, 17);
    contenstLayout->setColumnStretch(2, 18);
    contenstLayout->setColumnStretch(3, 8);
}

void CustomCardLabel::updateCardTypeName(const QString &typeName)
{
    labelTypeName->setText(tr("<b>%1</b>").arg(typeName));
}

void CustomCardLabel::updateRunningState(int state)
{
    Q_ASSERT((state == 0x00) || (state == 0x01));

    runningState = state;
    if (state == 0x00)
        labelRunningStateLamp->setPixmap(QPixmap(":/images/lamp_off.gif"));
    else
        labelRunningStateLamp->setPixmap(QPixmap(":/images/lamp_running.gif"));
}

void CustomCardLabel::updatePortState(int portId, int state, int type, bool isMPUCard)
{
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId <= MaximumPortId);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= cardPortList.size());

    QString imgPath;
    if (state == 0x00)
        imgPath = ":/images/lamp_off.gif";
    else
        imgPath = ":/images/lamp_running.gif";

    QString portTypeDesc;
    if (isMPUCard)
        portTypeDesc = tr("<b>%1</b>").arg(portId);
    else
        portTypeDesc = tr("<b>%1</b>").arg(portTyepDescList.at(type));

    if (portId < cardPortList.size()) {
        // 已存在的板卡端口
        cardPortList.at(portId)->labelStateLamp->setPixmap(QPixmap(imgPath));
        cardPortList.at(portId)->labelTypeDesc->setText(portTypeDesc);
    }
    else {
        // 新增板卡端口
        int rowIndex = contenstLayout->rowCount() - 1; // 最后一行是空白行，因此添加新行时要使用该行

        auto labelTypeDesc = new QLabel(portTypeDesc, this);
        labelTypeDesc->setObjectName(QString("labelTypeDesc%1").arg(1));
        labelTypeDesc->setAlignment(Qt::AlignCenter);
        labelTypeDesc->setFont(QFont(QString("Microsoft YaHei"), 6));
        contenstLayout->addWidget(labelTypeDesc, rowIndex, 1, Qt::AlignHCenter | Qt::AlignVCenter);

        auto labelStateLamp = new QLabel(this);
        labelStateLamp->setObjectName(QString("labelStateLamp%1").arg(1));
        labelStateLamp->setAlignment(Qt::AlignCenter);
        Utility::fillLabelWithImage(labelStateLamp, CardStateLampLabelWidth,
                                    CardStateLampLabelHeight, imgPath);
        contenstLayout->addWidget(labelStateLamp, rowIndex, 2, Qt::AlignHCenter | Qt::AlignVCenter);

        contenstLayout->setRowStretch(rowIndex, 16);
        int totalRowStretch = 0;
        for (int i = 0; i < contenstLayout->rowCount(); i++)
            totalRowStretch += contenstLayout->rowStretch(i);
        contenstLayout->setRowStretch(rowIndex + 1, CardLabelHeight - totalRowStretch);

        cardPortList.push_back(new CardPort{ labelTypeDesc, labelStateLamp });
    }
}

void CustomCardLabel::flickerRunningStateLamp(int parity)
{
    if ((parity % 2) == 0x00)
        labelRunningStateLamp->setPixmap(QPixmap(":/images/lamp_off.gif"));
    else
        labelRunningStateLamp->setPixmap(QPixmap(":/images/lamp_running.gif"));
}
