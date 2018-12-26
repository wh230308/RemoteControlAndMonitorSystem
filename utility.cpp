#include "utility.h"

#include <atomic>

#include <QLabel>
#include <QString>
#include <QCoreApplication>
#include <QSettings>
#include <QFile>

#include "log.hpp"


#define SVRS_ADDR_FILENAME  ("ServersAddress.ini")
#define SVR1_IP_KEY         ("Server1/Ip")
#define SVR1_PORT_KEY       ("Server1/Port")
#define SVR2_IP_KEY         ("Server2/Ip")
#define SVR2_PORT_KEY       ("Server2/Port")


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

bool Utility::loadSvrAddrConfig(QString &svr1Ip, ushort &svr1Port,
                                QString &svr2Ip, ushort &svr2Port)
{
    auto fileName = QCoreApplication::applicationDirPath() + QString(SVRS_ADDR_FILENAME);
    if (!QFile::exists(fileName)) {
        LOG_WARN("The server address config file does not exists, filename:%s", SVRS_ADDR_FILENAME);
        return false;
    }

    QSettings setting(fileName, QSettings::IniFormat);
    svr1Ip = setting.value(SVR1_IP_KEY).toString();
    svr1Port = setting.value(SVR1_PORT_KEY).toString().toUShort();
    svr2Ip = setting.value(SVR2_IP_KEY).toString();
    svr2Port = setting.value(SVR2_PORT_KEY).toString().toUShort();
    return true;
}

bool Utility::saveSvrAddrConfig(const QString &svr1Ip, ushort svr1Port,
                                const QString &svr2Ip, ushort svr2Port)
{
    auto fileName = QCoreApplication::applicationDirPath() + QString(SVRS_ADDR_FILENAME);
    QSettings setting(fileName, QSettings::IniFormat);
    setting.setValue(SVR1_IP_KEY, svr1Ip);
    setting.setValue(SVR1_PORT_KEY, svr1Port);
    setting.setValue(SVR2_IP_KEY, svr2Ip);
    setting.setValue(SVR2_PORT_KEY, svr2Port);
}
