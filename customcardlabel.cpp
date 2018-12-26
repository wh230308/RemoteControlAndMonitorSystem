#include "customcardlabel.h"

#include <QGridLayout>
#include <QDebug>

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

static const QString cardImgPath(":/images/card.gif");
static const QString cardImgPathOffline(":/images/card_offline.jpg");
static const QString cardImgPathSingleEth(":/images/card_single_eth.jpg");
static const QString stateLampOffImgPath(":/images/lamp_off.gif");
static const QString stateLampRunningImgPath(":/images/lamp_running.gif");

CustomCardLabel::CustomCardLabel(QWidget *parent, bool isMPUCard)
    : QLabel(parent)
    , isMPUCard_(isMPUCard)
{
    initContentsLayout();
}

void CustomCardLabel::updateCardTypeName(const QString &typeName)
{
    lblTypeName_->setText(tr("%1").arg(typeName));
}

void CustomCardLabel::updateRunningState(int state)
{
    Q_ASSERT((state == 0x00) || (state == 0x01));

    if (state == 0x00) {
        isRunning_ = false;
        lblRunningStateLamp_->setPixmap(QPixmap(stateLampOffImgPath));
    } else {
        isRunning_ = true;
        lblRunningStateLamp_->setPixmap(QPixmap(stateLampRunningImgPath));
    }
}

void CustomCardLabel::updatePortState(int portId, int type, int state)
{
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId <= kMaximumPortId);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= portTyepDescList.count());

    if (!isRunning_)
        state = 0x00;

    QString imgPath;
    if (state == 0x00)
        imgPath = stateLampOffImgPath;
    else
        imgPath = stateLampRunningImgPath;

    if (!mapCardPorts_.contains(portId)) {
        // 新增用户板端口
        addPortStateLayout(portId, imgPath);
    } else {
        // 已存在的用户板端口
        mapCardPorts_.value(portId)->lblStateLamp->setPixmap(QPixmap(imgPath));
    }
}

void CustomCardLabel::updateEthPortsState(int port1State, int port2State)
{
    Q_ASSERT((port1State == 0x00) || (port1State == 0x01));
    Q_ASSERT((port2State == 0x00) || (port2State == 0x01));

    if (!isRunning_) {
        Utility::fillLabelWithImage(this, kCardLabelWidth, kCardLabelHeight, cardImgPathOffline);
    }

    if ((port1State == 0) || (port2State == 0))
        Utility::fillLabelWithImage(this, kCardLabelWidth, kCardLabelHeight, cardImgPathSingleEth);
}

void CustomCardLabel::flickerRunningStateLamp(int parity)
{
    if ((parity % 2) == 0x00)
        lblRunningStateLamp_->setPixmap(QPixmap(stateLampOffImgPath));
    else
        lblRunningStateLamp_->setPixmap(QPixmap(stateLampRunningImgPath));
}

void CustomCardLabel::initContentsLayout()
{
    if (objectName().isEmpty())
        setObjectName(QString("customCardLabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kCardLabelWidth, kCardLabelHeight, cardImgPath);
    setAlignment(Qt::AlignCenter);
    setVisible(false); // 板卡默认不显示，当服务器有上报该板卡状态时显示

    layoutContents_ = new QGridLayout(this);
    layoutContents_->setObjectName(QString("layoutContenst%1").arg(Utility::generateUniqueObjectId()));
    layoutContents_->setHorizontalSpacing(1);
    layoutContents_->setVerticalSpacing(0);

    // 板卡类型名称描述
    lblTypeName_ = new QLabel(this);
    lblTypeName_->setObjectName(QString("lblTypeName%1").arg(Utility::generateUniqueObjectId()));
    lblTypeName_->setAlignment(Qt::AlignCenter);
    lblTypeName_->setFont(QFont(QString("Calibri"), 9, QFont::Bold));
    layoutContents_->addWidget(lblTypeName_, 1, 1, 1, 2, Qt::AlignHCenter | Qt::AlignBottom);


    // 板卡运行状态灯及其描述
    lblRunningStateLamp_ = new QLabel(this);
    lblRunningStateLamp_->setObjectName(QString("lblRunningStateLamp%1")
                                         .arg(Utility::generateUniqueObjectId()));
    lblRunningStateLamp_->setAlignment(Qt::AlignCenter);
    Utility::fillLabelWithImage(lblRunningStateLamp_, kCardStateLampLabelWidth,
                                kCardStateLampLabelHeight, stateLampOffImgPath);
    layoutContents_->addWidget(lblRunningStateLamp_, 2, 1, Qt::AlignHCenter | Qt::AlignBottom);

    auto lblRunningStateDesc = new QLabel(tr("<b>Run</b>"), this);
    lblRunningStateDesc->setObjectName(QString("lblRunningStateDesc%1")
                                         .arg(Utility::generateUniqueObjectId()));
    lblRunningStateDesc->setAlignment(Qt::AlignCenter);
    lblRunningStateDesc->setFont(QFont(QString("Calibri"), 6, QFont::Bold));
    layoutContents_->addWidget(lblRunningStateDesc, 2, 2, Qt::AlignHCenter | Qt::AlignBottom);

    // GridLayout默认4行，第1行第4行留白，第2行展示板卡类型，第3行展示运行状态灯及其描述
    layoutContents_->setRowStretch(0, kCardContentRowMargin);
    layoutContents_->setRowStretch(1, kCardContentRowHeight);
    layoutContents_->setRowStretch(2, kCardContentRowHeight);
    layoutContents_->setRowStretch(3, kCardLabelHeight - kCardContentRowMargin
                                   - kCardContentRowHeight * 2);

    // GridLayout共4列，第1列第4列留白，第2列第3列布置状态灯及其描述
    layoutContents_->setColumnStretch(0, kCardContentColumnMargin);
    layoutContents_->setColumnStretch(1, kCardContentLeftColumnWidth);
    layoutContents_->setColumnStretch(2, kCardContentRightColumnWidth);
    layoutContents_->setColumnStretch(3, kCardContentColumnMargin);

    if (!isMPUCard_) {
        addPortStateLayout(0, stateLampOffImgPath);
        addPortStateLayout(1, stateLampOffImgPath);
    }
}

void CustomCardLabel::addPortStateLayout(int portId, const QString &imgPath)
{
    int lastRowIndex = layoutContents_->rowCount() - 1;
    int lastRowStretch = layoutContents_->rowStretch(lastRowIndex);

    // 端口状态描述
    auto lblTypeDesc = new QLabel(tr("<b>%1</b>").arg(portId + 1), this);
    lblTypeDesc->setObjectName(QString("lblTypeDesc%1")
                                 .arg(Utility::generateUniqueObjectId()));
    lblTypeDesc->setAlignment(Qt::AlignCenter);
    lblTypeDesc->setFont(QFont(QString("Microsoft YaHei"), 6));
    layoutContents_->addWidget(lblTypeDesc, lastRowIndex, 1, Qt::AlignHCenter | Qt::AlignBottom);

    // 端口状态灯
    auto lblStateLamp = new QLabel(this);
    lblStateLamp->setObjectName(QString("lblStateLamp%1")
                                  .arg(Utility::generateUniqueObjectId()));
    lblStateLamp->setAlignment(Qt::AlignCenter);
    Utility::fillLabelWithImage(lblStateLamp, kCardStateLampLabelWidth, kCardStateLampLabelHeight,
                                imgPath);
    layoutContents_->addWidget(lblStateLamp, lastRowIndex, 2, Qt::AlignHCenter | Qt::AlignBottom);

    layoutContents_->setRowStretch(lastRowIndex, kCardContentRowHeight);
    layoutContents_->setRowStretch(lastRowIndex + 1, lastRowStretch - kCardContentRowHeight);

    mapCardPorts_.insert(portId, new CardPort{ lblTypeDesc, lblStateLamp });
}
