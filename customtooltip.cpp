#include "customtooltip.h"

#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QEvent>
#include <QDebug>

CustomToolTip::CustomToolTip(const CWPInfo &cwpInfo, QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    resize(162, 202);

    initContentsLayout(cwpInfo);
    installEventFilter(this);
}

void CustomToolTip::popupTip(const CWPInfo &cwpInfo, const QPoint &pos)
{
    auto toolTip = new CustomToolTip(cwpInfo);
    toolTip->show();
    toolTip->move(pos);
}

void CustomToolTip::initContentsLayout(const CWPInfo &cwpInfo)
{
    auto topLevelLayout = new QHBoxLayout(this);
    topLevelLayout->setObjectName(QStringLiteral("topLevelLayout"));
    topLevelLayout->setContentsMargins(0, 0, 0, 0);
    topLevelLayout->setSpacing(0);

    auto tableWidget = new QTableWidget(8, 2, this);
    tableWidget->setWindowFlags(Qt::FramelessWindowHint);
    tableWidget->horizontalHeader()->setVisible(false);
    tableWidget->verticalHeader()->setVisible(false);
    topLevelLayout->addWidget(tableWidget);

    auto seatIdItem = new QTableWidgetItem(tr("席位号"));
    tableWidget->setItem(0, 0, seatIdItem);
    auto seatIdItemValue = new QTableWidgetItem(QString().setNum(cwpInfo.seatId));
    tableWidget->setItem(0, 1, seatIdItemValue);
    tableWidget->setRowHeight(0, 18);

    auto seatStateItem = new QTableWidgetItem(tr("状态"));
    tableWidget->setItem(1, 0, seatStateItem);
    auto seatStateValue = new QTableWidgetItem(QString().setNum(cwpInfo.state));
    tableWidget->setItem(1, 1, seatStateValue);
    tableWidget->setRowHeight(1, 18);

    auto userNameItem = new QTableWidgetItem(tr("用户名"));
    tableWidget->setItem(2, 0, userNameItem);
    auto userNameValue = new QTableWidgetItem(cwpInfo.userName);
    tableWidget->setItem(2, 1, userNameValue);
    tableWidget->setRowHeight(2, 18);

    auto leftPhoneItem = new QTableWidgetItem(tr("左话机"));
    tableWidget->setItem(3, 0, leftPhoneItem);
    auto leftPhoneValue = new QTableWidgetItem(cwpInfo.leftPhone);
    tableWidget->setItem(3, 1, leftPhoneValue);
    tableWidget->setRowHeight(3, 18);

    auto rightPhoneItem = new QTableWidgetItem(tr("右话机"));
    tableWidget->setItem(4, 0, rightPhoneItem);
    auto rightPhoneValue = new QTableWidgetItem(cwpInfo.rightPhone);
    tableWidget->setItem(4, 1, rightPhoneValue);
    tableWidget->setRowHeight(4, 18);

    auto userRoleItem1 = new QTableWidgetItem(tr("用户角色1"));
    tableWidget->setItem(5, 0, userRoleItem1);
    auto userRolesValue1 = new QTableWidgetItem(cwpInfo.userRole1);
    tableWidget->setItem(5, 1, userRolesValue1);
    tableWidget->setRowHeight(5, 18);

    auto userRolesItem2 = new QTableWidgetItem(tr("用户角色2"));
    tableWidget->setItem(6, 0, userRolesItem2);
    auto userRolesValue2 = new QTableWidgetItem(cwpInfo.userRole2);
    tableWidget->setItem(6, 1, userRolesValue2);
    tableWidget->setRowHeight(6, 18);

    auto userRolesItem3 = new QTableWidgetItem(tr("用户角色3"));
    tableWidget->setItem(7, 0, userRolesItem3);
    auto userRolesValue3 = new QTableWidgetItem(cwpInfo.userRole3);
    tableWidget->setItem(7, 1, userRolesValue3);
    tableWidget->setRowHeight(7, 18);

    tableWidget->setColumnWidth(0, 70);
    tableWidget->setColumnWidth(1, 90);
}

bool CustomToolTip::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == this) && (event->type() == QEvent::WindowDeactivate)) {
        close();
        event->accept();

        return true;
    }

    return QDialog::eventFilter(obj, event);
}
