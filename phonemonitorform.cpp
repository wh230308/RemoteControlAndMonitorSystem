#include "phonemonitorform.h"

#include <QLabel>

#include "flowlayout.h"
#include "utility.h"


PhoneMonitorForm::PhoneMonitorForm(QWidget *parent) : QWidget(parent)
{
    auto flowlayout = new FlowLayout(this, 30, 15, 15);
    for (int i = 0; i < PhoneNumber; i++) {
        auto labelPhone = new QLabel(this);
        labelPhone->setObjectName(QString("labelPhone%1").arg(i));
        Utility::fillLabelWithImage(labelPhone, 96, 96, ":/images/phone.gif");
        flowlayout->addWidget(labelPhone);

        auto phoneInnerLayout = new QVBoxLayout(labelPhone);
        phoneInnerLayout->setObjectName(QStringLiteral("phoneInnerLayout"));
        phoneInnerLayout->setContentsMargins(9, 9, 9, 1);
        phoneInnerLayout->setSpacing(0);

        auto labelPhoneId = new QLabel(tr("<h2>PHONE%1</h2>").arg(i + 1), labelPhone);
        labelPhoneId->setObjectName(QString("labelPhoneId%1").arg(i));
        phoneInnerLayout->addWidget(labelPhoneId);
        phoneInnerLayout->setAlignment(labelPhoneId, Qt::AlignHCenter | Qt::AlignBottom);
    }
}
