#include "customcwplabel.h"

#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>

#include "utility.h"

CustomCWPLabel::CustomCWPLabel(QWidget *parent) : QLabel(parent)
{
    initContentsLayout();
    installEventFilter(this);
}

void CustomCWPLabel::updateRunningState()
{

}

void CustomCWPLabel::initContentsLayout()
{
    if (objectName().isEmpty())
        setObjectName(QString("customCardLabel%1").arg(Utility::generateUniqueObjectId()));

    Utility::fillLabelWithImage(this, kCWPItemWidth, kCWPItemHeight, ":/images/cwp.gif");
    setAlignment(Qt::AlignCenter);

    auto layoutCWPContents = new QVBoxLayout(this);
    layoutCWPContents->setObjectName(QString("layoutCWPContents%1").arg(Utility::generateUniqueObjectId()));
    layoutCWPContents->setContentsMargins(9, 9, 9, 1);
    layoutCWPContents->setSpacing(0);

    static int index = 0;
    auto lblCWPId = new QLabel(tr("<h2>CWP%1</h2>").arg(++index), this);
    lblCWPId->setObjectName(QString("lblCWPId%1").arg(Utility::generateUniqueObjectId()));
    lblCWPId->setAlignment(Qt::AlignCenter);
    layoutCWPContents->addWidget(lblCWPId);
    layoutCWPContents->setAlignment(lblCWPId, Qt::AlignHCenter | Qt::AlignBottom);
}

bool CustomCWPLabel::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        if (event->type() == QEvent::Enter) {
            QPoint cwpPos = mapToGlobal(QPoint(0, 0));
            cwpPos += QPoint(width() / 2, height() / 2);
            CustomToolTip::popupTip(cwpInfo_, cwpPos);
            return true;
        }
    }

    return QLabel::eventFilter(watched, event);
}
