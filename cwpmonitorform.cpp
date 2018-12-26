#include "cwpmonitorform.h"

#include <QLabel>
#include <QEvent>
#include <QDebug>

#include "flowlayout.h"
#include "customcwplabel.h"
#include "log.hpp"


CWPMonitorForm::CWPMonitorForm(QWidget *parent) : QWidget(parent)
{
    auto layoutContents = new FlowLayout(this, kMargin, kCWPItemHSpace, kCWPItemVSpace);
    for (int i = 0; i < kCWPItemNumber; i++) {
        auto lblCWP = new CustomCWPLabel(this);
        layoutContents->addWidget(lblCWP);
    }
}

void CWPMonitorForm::onReportCWPState(char seatId, char seatState, const QByteArray &userName,
                                      const QByteArray &leftPhone, const QByteArray &rightPhone,
                                      const QByteArray &userRoles)
{
    qDebug() << tr("CWP Form pos:") << pos();
    qDebug() << tr("CWP Form global pos:") << mapToGlobal(pos());

    CustomCWPLabel *lblCWPItem = nullptr;
    foreach (auto item, vecCWPItems_) {
        if (item->getCWPId() == 0xff || item->getCWPId() == seatId) {
            lblCWPItem = item;
            break;
        }
    }

    if (lblCWPItem != nullptr) {
        QVector<QString> roles;
        QString role;
        for (int i = 0; i < userRoles.count(); i++) {
            if (userRoles.at(i) != '\0') {
                role.push_back(userRoles.at(i));
                continue;
            }

            if (!role.isEmpty()) {
                roles.push_back(role);
                role.clear();
            }
        }

        CWPInfo cwpInfo;
        cwpInfo.seatId = seatId;
        cwpInfo.state = seatState;
        cwpInfo.userName = userName;
        cwpInfo.leftPhone = leftPhone;
        cwpInfo.rightPhone = rightPhone;
        cwpInfo.userRole1 = roles.at(0);
        cwpInfo.userRole1 = roles.at(1);
        cwpInfo.userRole1 = roles.at(2);
        lblCWPItem->setCWPInfo(cwpInfo);
    }
}
