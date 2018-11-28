#include "cwpmonitorform.h"

#include <QLabel>

#include "flowlayout.h"
#include "utility.h"


CWPMonitorForm::CWPMonitorForm(QWidget *parent) : QWidget(parent)
{
    auto flowlayout = new FlowLayout(this, 30, 15, 15);
    for (int i = 0; i < CWPNumber; i++) {
        auto labelCWP = new QLabel(this);
        labelCWP->setObjectName(QString("labelCWP%1").arg(i));
        Utility::fillLabelWithImage(labelCWP, 96, 96, ":/images/cwp.gif");
        flowlayout->addWidget(labelCWP);

        auto cwpInnerLayout = new QVBoxLayout(labelCWP);
        cwpInnerLayout->setObjectName(QStringLiteral("cwpInnerLayout"));
        cwpInnerLayout->setContentsMargins(9, 9, 9, 1);
        cwpInnerLayout->setSpacing(0);

        auto labelCWPId = new QLabel(tr("<h2>CWP%1</h2>").arg(i + 1), labelCWP);
        labelCWPId->setObjectName(QString("labelCWPId%1").arg(i));
        cwpInnerLayout->addWidget(labelCWPId);
        cwpInnerLayout->setAlignment(labelCWPId, Qt::AlignHCenter | Qt::AlignBottom);
    }
}
