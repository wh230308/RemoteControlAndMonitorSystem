#include "utility.h"

#include <QLabel>
#include <QString>
#include <QCoreApplication>
#include <QSettings>

#include <atomic>

#define SVRS_ADDR_FILENAME ("ServersAddress.ini")
#define SVR1_IP_KEY ("Server1/Ip")
#define SVR1_PORT_KEY ("Server1/Port")
#define SVR2_IP_KEY ("Server2/Ip")
#define SVR2_PORT_KEY ("Server2/Port")


uint Utility::generateUniqueObjectId()
{
    static std::atomic<uint> uniqueObjectId{ 0 };
    uniqueObjectId.fetch_add(1);

    return uniqueObjectId;
}

void Utility::fillLabelWithImage(QLabel *label, int width, int height, const QString &imagePath)
{
    label->resize(width, height);

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

void Utility::loadServerAddrConfig(QString &svr1Ip, ushort &svr1Port,
                                   QString &svr2Ip, ushort &svr2Port)
{
    QSettings setting(QString("%1\\%2").arg(QCoreApplication::applicationDirPath())
                       .arg(SVRS_ADDR_FILENAME), QSettings::IniFormat);
    svr1Ip = setting.value(SVR1_IP_KEY).toString();
    svr1Port = setting.value(SVR1_PORT_KEY).toString().toUShort();
    svr2Ip = setting.value(SVR2_IP_KEY).toString();
    svr2Port = setting.value(SVR2_PORT_KEY).toString().toUShort();
}
