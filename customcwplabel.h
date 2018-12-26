#ifndef CUSTOMCWPLABEL_H
#define CUSTOMCWPLABEL_H

#include <QLabel>

#include "customtooltip.h"


class CustomCWPLabel : public QLabel
{
    Q_OBJECT

enum
{
    kCWPItemWidth = 120,
    kCWPItemHeight = 100
};

public:
    explicit CustomCWPLabel(QWidget *parent = nullptr);

    void updateRunningState();
    void setCWPInfo(const CWPInfo &cwpInfo)
    {
        cwpInfo_ = std::move(cwpInfo);
    }

    int getCWPId() const
    {
        return cwpInfo_.seatId;
    }

signals:

public slots:

private:
    void initContentsLayout();
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    CWPInfo cwpInfo_;
    CustomToolTip *toolTip_ = nullptr;
};

#endif // CUSTOMCWPLABEL_H
