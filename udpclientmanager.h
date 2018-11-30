#ifndef UDPCLIENTMANAGER_H
#define UDPCLIENTMANAGER_H

#include <QObject>
#include <QVector>

class UdpClient;

class UdpClientManager : public QObject
{
    Q_OBJECT
public:
    explicit UdpClientManager(QObject *parent = nullptr);
    ~UdpClientManager();

signals:
    void heartbeatTimeout();
    void reportMainCardState(char deviceId, char slotIndex, char state);
    void reportUserCardState(char deviceId, char slotIndex, char state, char type);
    void reportCWPState(char seatId, char seatState, const QByteArray &userName,
                        const QByteArray &leftPhone, const QByteArray &rightPhone,
                        const QByteArray &userRoles);
    void reportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName);

private:
    QVector<UdpClient *> udpClientList;
};

#endif // UDPCLIENTMANAGER_H
