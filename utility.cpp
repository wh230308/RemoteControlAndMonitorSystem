#include "utility.h"

#include <QLabel>
#include <QString>


Utility::Utility()
{

}

void Utility::fillLabelWithImage(QLabel *label, int width, int height, const QString &imagePath)
{
    label->resize(width, height);
    label->setMinimumSize(width, height);

    QImage image;
    image.load(imagePath);
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap pixmapFit = pixmap.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    label->setPixmap(pixmapFit);
}

void Utility::fillLabelWithImage(QLabel *label, const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    pixmap.scaled(label->size(), Qt::KeepAspectRatio);
    label->setScaledContents(true);
    label->setPixmap(pixmap);
}
