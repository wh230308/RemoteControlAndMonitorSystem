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
    void reportMainCardState(char deviceId, char mpuFlag, char state);
    void reportUserCardState(char deviceId, char slotIndex, char type, char state);
    void reportCWPState(char seatId, char seatState, const QByteArray &userName,
                        const QByteArray &leftPhone, const QByteArray &rightPhone,
                        const QByteArray &userRoles);
    void reportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName);
    void reportEthPortsState(char deviceId, char mpuFlag, char port1State, char port2State);
    void reportUserCardPortState(char deviceId, char slotIndex, char portId, char type, char state);

public slots:
    void onSvrAddrChanged(const QString &svr1Ip, ushort svr1Port,
                          const QString &svr2Ip, ushort svr2Port);

private:
    QVector<UdpClient *> vecUdpClients_;
};

#endif // UDPCLIENTMANAGER_H
