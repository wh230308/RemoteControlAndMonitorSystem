#ifndef UTILITY_H
#define UTILITY_H

#include <QtGlobal>

class QLabel;
class QString;

class Utility
{
public:

    static uint generateUniqueObjectId();

    static void fillLabelWithImage(QLabel *label, int width, int height, const QString &imagePath);
    static void fillLabelWithImage(QLabel *label, const QString &imagePath);

    static bool loadSvrAddrConfig(QString &svr1Ip, ushort &svr1Port,
                                  QString &svr2Ip, ushort &svr2Port);
    static bool saveSvrAddrConfig(const QString &svr1Ip, ushort svr1Port,
                                  const QString &svr2Ip, ushort svr2Port);
};

#endif // UTILITY_H
