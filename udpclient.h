#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class QNetworkDatagram;
class QTimer;

class UdpClient : public QObject
{
    Q_OBJECT

enum
{
    HeartBeatTimerInterval = 5000 // 与服务器心跳保活定时器周期
};

public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();

public:
    bool initSock(const QString &svrIp, ushort port, const QString &localIp, ushort localport);
    void uinitSock();
    bool sendPacket(const char *data, int size);
    bool sendPacket(const QNetworkDatagram &datagram);

    void getSvrAddr(QString &svrIp, ushort &svrPort_) const;
    bool updateSvrAddr(const QString &svrIp, ushort svrPort_);

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
    void onSendHeartbeat();
    void readPendingDatagrams();

private:
    void processTheDatagram(const QNetworkDatagram &datagram);
    void processHeartbeatPkt(const QByteArray &byteArray);
    void processMainCardStatePkt(const QByteArray &byteArray);
    void processUserCardStatePkt(const QByteArray &byteArray);
    void processCWPIntoPkt(const QByteArray &byteArray);
    void processDeviceInfoPkt(const QByteArray &byteArray);
    void processEthPortsStatePkt(const QByteArray &byteArray);
    void processUserCardPortStatePkt(const QByteArray &byteArray);
    void replyPkt(const QNetworkDatagram &datagram);

private:
    QUdpSocket localSock_;
    QHostAddress svrIp_;
    ushort svrPort_ = 0;
    QTimer *heartbeatTimer_ = nullptr;
    int heartbeatPktCount_ = 0;
    bool isFirstHeartbeatPkt_ = true;
};

#endif // UDPCLIENT_H
