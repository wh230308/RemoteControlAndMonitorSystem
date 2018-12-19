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

static const QString cardImagePath(":/images/card.gif");
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
    labelTypeName_->setText(tr("<b>%1</b>").arg(typeName));
}

void CustomCardLabel::updateRunningState(int state)
{
    Q_ASSERT((state == 0x00) || (state == 0x01));

    runningState_ = state;
    if (state == 0x00)
        labelRunningStateLamp_->setPixmap(QPixmap(stateLampOffImgPath));
    else
        labelRunningStateLamp_->setPixmap(QPixmap(stateLampRunningImgPath));
}

void CustomCardLabel::updatePortState(int portId, int state, int type)
{
    Q_ASSERT(portId >= 0);
    Q_ASSERT(portId <= kMaximumPortId);
    Q_ASSERT((state == 0x00) || (state == 0x01));
    Q_ASSERT(type >= 0);
    Q_ASSERT(type <= cardPortList_.size());

    QString imgPath;
    if (state == 0x00)
        imgPath = stateLampOffImgPath;
    else
        imgPath = stateLampRunningImgPath;

    if (!cardPortList_.contains(portId)) {
        // 新增板卡端口
        addPortStateLayout(portId, imgPath);
    } else {
        // 已存在的板卡端口
        cardPortList_.value(portId)->labelStateLamp->setPixmap(QPixmap(imgPath));
        cardPortList_.value(portId)->labelTypeDesc->setText(tr("<b>%1</b>").arg(portId));
    }
}

void CustomCardLabel::flickerRunningStateLamp(int parity)
{
    if ((parity % 2) == 0x00)
        labelRunningStateLamp_->setPixmap(QPixmap(stateLampOffImgPath));
    else
        labelRunningStateLamp_->setPixmap(QPixmap(stateLampRunningImgPath));
}

void CustomCardLabel::initContentsLayout()
{
    if (objectName().isEmpty())
        setObjectName(QString("customCardLabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kCardLabelWidth, kCardLabelHeight, cardImagePath);
    setAlignment(Qt::AlignCenter);
    setVisible(false); // 板卡默认不显示，当服务器有上报该板卡状态时显示

    contenstLayout_ = new QGridLayout(this);
    contenstLayout_->setObjectName(QString("contenstLayout%1").arg(Utility::generateUniqueObjectId()));
    contenstLayout_->setHorizontalSpacing(1);
    contenstLayout_->setVerticalSpacing(0);

    // 板卡类型名称描述
    labelTypeName_ = new QLabel(this);
    labelTypeName_->setObjectName(QString("labelTypeName%1").arg(Utility::generateUniqueObjectId()));
    labelTypeName_->setAlignment(Qt::AlignCenter);
    contenstLayout_->addWidget(labelTypeName_, 1, 1, 1, 2, Qt::AlignHCenter | Qt::AlignBottom);


    // 板卡运行状态灯及其描述
    labelRunningStateLamp_ = new QLabel(this);
    labelRunningStateLamp_->setObjectName(QString("labelRunningStateLamp%1")
                                         .arg(Utility::generateUniqueObjectId()));
    labelRunningStateLamp_->setAlignment(Qt::AlignCenter);
    Utility::fillLabelWithImage(labelRunningStateLamp_, kCardStateLampLabelWidth,
                                kCardStateLampLabelHeight, stateLampOffImgPath);
    contenstLayout_->addWidget(labelRunningStateLamp_, 2, 1, Qt::AlignHCenter | Qt::AlignBottom);

    auto labelRunningStateDesc = new QLabel(tr("<b>Run</b>"), this);
    labelRunningStateDesc->setObjectName(QString("cardRunningStateDesc%1")
                                         .arg(Utility::generateUniqueObjectId()));
    labelRunningStateDesc->setAlignment(Qt::AlignCenter);
    labelRunningStateDesc->setFont(QFont(QString("Microsoft YaHei"), 6, 1));
    contenstLayout_->addWidget(labelRunningStateDesc, 2, 2, Qt::AlignHCenter | Qt::AlignBottom);

    // GridLayout默认4行，第1行第4行留白，第2行展示板卡类型，第3行展示运行状态灯及其描述
    contenstLayout_->setRowStretch(0, kCardContentRowMargin);
    contenstLayout_->setRowStretch(1, kCardContentRowHeight);
    contenstLayout_->setRowStretch(2, kCardContentRowHeight);
    contenstLayout_->setRowStretch(3, kCardLabelHeight - kCardContentRowMargin
                                   - kCardContentRowHeight * 2);

    // GridLayout共4列，第1列第4列留白，第2列第3列布置状态灯及其描述
    contenstLayout_->setColumnStretch(0, kCardContentColumnMargin);
    contenstLayout_->setColumnStretch(1, kCardContentLeftColumnWidth);
    contenstLayout_->setColumnStretch(2, kCardContentRightColumnWidth);
    contenstLayout_->setColumnStretch(3, kCardContentColumnMargin);

    if (!isMPUCard_) {
        addPortStateLayout(0, stateLampOffImgPath);
        addPortStateLayout(1, stateLampOffImgPath);
    }
}

void CustomCardLabel::addPortStateLayout(int portId, const QString &imgPath)
{
    int lastRowIndex = contenstLayout_->rowCount() - 1;
    int lastRowStretch = contenstLayout_->rowStretch(lastRowIndex);

    // 端口状态描述
    auto labelTypeDesc = new QLabel(tr("<b>%1</b>").arg(portId + 1), this);
    labelTypeDesc->setObjectName(QString("labelTypeDesc%1")
                                 .arg(Utility::generateUniqueObjectId()));
    labelTypeDesc->setAlignment(Qt::AlignCenter);
    labelTypeDesc->setFont(QFont(QString("Microsoft YaHei"), 6));
    contenstLayout_->addWidget(labelTypeDesc, lastRowIndex, 1, Qt::AlignHCenter | Qt::AlignBottom);

    // 端口状态灯
    auto labelStateLamp = new QLabel(this);
    labelStateLamp->setObjectName(QString("labelStateLamp%1")
                                  .arg(Utility::generateUniqueObjectId()));
    labelStateLamp->setAlignment(Qt::AlignCenter);
    Utility::fillLabelWithImage(labelStateLamp, kCardStateLampLabelWidth, kCardStateLampLabelHeight,
                                imgPath);
    contenstLayout_->addWidget(labelStateLamp, lastRowIndex, 2, Qt::AlignHCenter | Qt::AlignBottom);

    contenstLayout_->setRowStretch(lastRowIndex, kCardContentRowHeight);
    contenstLayout_->setRowStretch(lastRowIndex + 1, lastRowStretch - kCardContentRowHeight);

    cardPortList_.insert(portId, new CardPort{ labelTypeDesc, labelStateLamp });
}
