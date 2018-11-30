#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>

class QUdpSocket;
class QHostAddress;
class QNetworkDatagram;
class QTimer;

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();

public:
    bool initSock(const char *peerIp, ushort port);
    void uinitSock();
    bool sendPacket(const char *data, int size);
    bool sendPacket(const QNetworkDatagram &datagram);

signals:
    void heartbeatTimeout();
    void reportMainCardState(char deviceId, char slotIndex, char state);
    void reportUserCardState(char deviceId, char slotIndex, char state, char type);
    void reportCWPState(char seatId, char seatState, const QByteArray &userName,
                        const QByteArray &leftPhone, const QByteArray &rightPhone,
                        const QByteArray &userRoles);
    void reportDeviceInfo(char deviceId, char deviceType, const QByteArray &deviceName);


public slots:
    void onSendHeartbeat();
    void readPendingDatagrams();

private:
    void processTheDatagram(const QNetworkDatagram& datagram);
    void processHeartbeatPkt(const QByteArray& byteArray);
    void processMainCardStatePkt(const QByteArray& byteArray);
    void processUserCardStatePkt(const QByteArray& byteArray);
    void processCWPIntoPkt(const QByteArray& byteArray);
    void processDeviceInfoPkt(const QByteArray& byteArray);
    void replyPkt(const QNetworkDatagram& datagram);

private:
    QUdpSocket *udpSock = nullptr;
    QHostAddress *peerHostAddr = nullptr;
    ushort peerPort = 0;
    QTimer *heartbeatTimer = nullptr;
    int heartbeatPktCount = 0;
    bool isFirstHeartbeatPkt = true;
};

#endif // UDPCLIENT_H
