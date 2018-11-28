#include "radiomonitorform.h"

#include <QLabel>

#include "flowlayout.h"
#include "utility.h"


RadioMonitorForm::RadioMonitorForm(QWidget *parent) : QWidget(parent)
{
    auto flowlayout = new FlowLayout(this, 30, 15, 15);
    for (int i = 0; i < RadioNumber; i++) {
        auto labelRadio = new QLabel(this);
        labelRadio->setObjectName(QString("labelRadio%1").arg(i));
        Utility::fillLabelWithImage(labelRadio, 96, 96, ":/images/radio.gif");
        flowlayout->addWidget(labelRadio);

        auto radioInnerLayout = new QVBoxLayout(labelRadio);
        radioInnerLayout->setObjectName(QStringLiteral("radioInnerLayout"));
        radioInnerLayout->setContentsMargins(9, 9, 9, 1);
        radioInnerLayout->setSpacing(0);

        auto labelRadioId = new QLabel(tr("<h2>RADIO%1</h2>").arg(i + 1), labelRadio);
        labelRadioId->setObjectName(QString("labelRadioId%1").arg(i));
        radioInnerLayout->addWidget(labelRadioId);
        radioInnerLayout->setAlignment(labelRadioId, Qt::AlignHCenter | Qt::AlignBottom);
    }
}
