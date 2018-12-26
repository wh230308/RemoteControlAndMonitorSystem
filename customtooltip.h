#ifndef CUSTOMTOOLTIP_H
#define CUSTOMTOOLTIP_H

#include <QDialog>

class QLabel;

struct CWPInfo
{
    CWPInfo()
    {
        seatId = 0xff;
        state = 0;
    }

    int seatId;
    int state;
    QString userName;
    QString leftPhone;
    QString rightPhone;
    QString userRole1;
    QString userRole2;
    QString userRole3;
};

class CustomToolTip : public QDialog
{
    Q_OBJECT
public:
    explicit CustomToolTip(const CWPInfo &cwpInfo, QWidget *parent = nullptr);

    static void popupTip(const CWPInfo &cwpInfo, const QPoint &pos);

signals:

public slots:

private:
    void initContentsLayout(const CWPInfo &cwpInfo);
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // CUSTOMTOOLTIP_H
