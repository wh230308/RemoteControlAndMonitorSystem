#include "utility.h"

#include <QLabel>
#include <QString>


Utility::Utility()
{

}

void Utility::fillLabelWithImage(QLabel *label, int width, int height, const QString &imagePath)
{
    label->setGeometry(0, 0, width, height);

    QImage image;
    image.load(imagePath);
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap pixmapFit = pixmap.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    label->setPixmap(pixmapFit);
}
