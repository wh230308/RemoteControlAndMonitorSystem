#ifndef CWPMONITORFORM_H
#define CWPMONITORFORM_H

#include <QWidget>
#include <QVector>
#include <QMap>


class CustomCWPLabel;


class CWPMonitorForm : public QWidget
{
    Q_OBJECT

enum
{
    kMargin = 30,
    kCWPItemHSpace = 30,
    kCWPItemVSpace = 30,

    kCWPItemNumber = 12
};

public:
    explicit CWPMonitorForm(QWidget *parent = nullptr);

signals:

public slots:
    void onReportCWPState(char seatId, char seatState, const QByteArray &userName,
                          const QByteArray &leftPhone, const QByteArray &rightPhone,
                          const QByteArray &userRoles);

private:
    QVector<CustomCWPLabel *> vecCWPItems_;
};

#endif // CWPMONITORFORM_H
