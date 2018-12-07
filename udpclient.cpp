#include "udpclient.h"

#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QTimer>

#include "log.hpp"


UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
}

UdpClient::~UdpClient()
{
    if (heartbeatTimer != nullptr)
        heartbeatTimer->stop();

    uinitSock();
    delete svrHostAddr;
}

bool UdpClient::initSock(const QString &svrIp, ushort svrPort)
{
    udpSock = new QUdpSocket(this);
    if (!udpSock->bind()) {
        LOG_ERROR("bind failed...");
        return false;
    }

    connect(udpSock, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    svrHostAddr = new QHostAddress(svrIp);
    this->svrPort = svrPort;

    heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(onSendHeartbeat()));
    heartbeatTimer->start(HeartBeatTimerInterval);

    return true;
}

void UdpClient::uinitSock()
{
   udpSock->close();
}

bool UdpClient::sendPacket(const char *data, int size)
{
    if (-1 == udpSock->writeDatagram(data, static_cast<qint64>(size), *svrHostAddr, svrPort)) {
        LOG_ERROR("send packet failed, error:%d", udpSock->error());
        return false;
    }

    return true;
}

bool UdpClient::sendPacket(const QNetworkDatagram &datagram)
{
    if (-1 == udpSock->writeDatagram(datagram)) {
        LOG_ERROR("send packet failed, error:%d", udpSock->error());
        return false;
    }

    return true;
}

void UdpClient::getSvrAddr(QString &svrIp, ushort &svrPort) const
{
    svrIp = svrHostAddr->toString();
    svrPort = this->svrPort;
}

bool UdpClient::updateSvrAddr(const QString &svrIp, ushort svrPort)
{
    LOG_TRACE("update server address, svrIp:%s, svrPort:%u", svrIp.toStdString().c_str(), svrPort);

    if (!svrHostAddr->setAddress(svrIp)) {
        LOG_ERROR("set server ip address failed, svrIp:%s", svrIp.toStdString().c_str());
        return false;
    }

    this->svrPort = svrPort;

    return true;
}

void UdpClient::onSendHeartbeat()
{
    heartbeatPktCount++;
    const uchar heartbeatPacket[] = { 0x57, 0x5a, 0xaa, 0x5a, 0xaa };
    sendPacket((const char *)heartbeatPacket, sizeof(heartbeatPacket));

    //qDebug() << "onSendHeartbeat count:" << heartbeatPktCount;
    if (heartbeatPktCount >= 3) {
        LOG_ERROR("heartbeat keep alived timeout with server %s",
                  svrHostAddr->toString().toStdString().c_str());
        heartbeatPktCount = 0;

        emit heartbeatTimeout();
    }
}

void UdpClient::readPendingDatagrams()
{
    while (udpSock->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSock->receiveDatagram();
        processTheDatagram(datagram);
    }
}

void UdpClient::processTheDatagram(const QNetworkDatagram &datagram)
{
    if (!datagram.isValid()) {
        LOG_ERROR("the datagarm is invalid");
        return;
    }

    if (datagram.data().isEmpty()) {
        LOG_WARN("the datagarm is empty");
        return;
    }

    if (datagram.data().size() > 1) {
        if ((0x57 == datagram.data().at(0)) && (datagram.data().size() >= 5)) {
            //qDebug() << tr("processHeartbeatPkt...");
            //LOG_TRACE("process heartbeat...");
            processHeartbeatPkt(datagram.data());
            return;
        }

        if ((0xf0 == static_cast<uchar>(datagram.data().at(0)))
                 && (datagram.data().size() >= 6)) {
            //qDebug() << tr("onReportMainCardState...");
            processMainCardStatePkt(datagram.data());
        }
        else if ((0x00 == datagram.data().at(0)) && (datagram.data().size()) >= 9) {
            //qDebug() << tr("onReportUserCardState...");
            processUserCardStatePkt(datagram.data());
        }
        else if ((0x20 == datagram.data().at(0)) && (datagram.data().size()) >= 198) {
            //qDebug() << tr("onReportCWPInfo...");
            processCWPIntoPkt(datagram.data());
        }
        else if ((0x10 == datagram.data().at(0)) && (datagram.data().size()) >= 8) {
            //qDebug() << tr("onReportPortState...");
            processUserCardPortStatePkt(datagram.data());
        }
        else if ((0x80 == static_cast<uchar>(datagram.data().at(0))
                  && (datagram.data().size()) >= 8)) {
            //qDebug() << tr("onReportNetworkInterfaceState...");
            processMPUNetworkPortsStatePkt(datagram.data());
        }
        else if ((0x71 == datagram.data().at(0)) && (datagram.data().size()) >= 21) {
            //qDebug() << tr("onReportDeviceInfo...");
            processDeviceInfoPkt(datagram.data());
        }

        replyPkt(datagram);
    }
}

void UdpClient::processHeartbeatPkt(const QByteArray &byteArray)
{
    if ((0x5a == static_cast<uchar>(byteArray.at(1)))
            && (0xaa == static_cast<uchar>(byteArray.at(2)))
            && (0x5a == static_cast<uchar>(byteArray.at(3)))
            && (0xaa == static_cast<uchar>(byteArray.at(4)))) {
        heartbeatPktCount--;

        if (isFirstHeartbeatPkt) {
            isFirstHeartbeatPkt = false;
            const char queryStates[] = { 0x5e, 0x01, 0x02, 0x03, 0x04 };
            sendPacket(static_cast<const char*>(queryStates), sizeof(queryStates));
        }
    }
}

void UdpClient::processMainCardStatePkt(const QByteArray &byteArray)
{
    char deviceId = byteArray.at(4);
    char slotIndex = byteArray.at(3);
    char state = byteArray.at(5);

    emit reportMainCardState(deviceId, slotIndex, state);
}

void UdpClient::processUserCardStatePkt(const QByteArray &byteArray)
{
    char deviceId = byteArray.at(3);
    char slotIndex = byteArray.at(4);
    char state = byteArray.at(5);
    char type = byteArray.at(6);

    emit reportUserCardState(deviceId, slotIndex, state, type);
}

void UdpClient::processCWPIntoPkt(const QByteArray &byteArray)
{
    char seatId = byteArray.at(4);
    char seatState = byteArray.at(5);
    QByteArray userName = byteArray.mid(6, 32);
    QByteArray leftPhone = byteArray.mid(38, 32);
    QByteArray rightPhone = byteArray.mid(70, 32);
    QByteArray userRoles = byteArray.mid(102);

    emit reportCWPState(seatId, seatState, userName, leftPhone, rightPhone, userRoles);
}

void UdpClient::processDeviceInfoPkt(const QByteArray &byteArray)
{
    char deviceId = byteArray.at(3);
    char deviceType = byteArray.at(4);
    QByteArray deviceName = byteArray.right(16);

    emit reportDeviceInfo(deviceId, deviceType, deviceName);
}

void UdpClient::processMPUNetworkPortsStatePkt(const QByteArray &byteArray)
{
    char mpuIndex = byteArray.at(3);
    char deviceId = byteArray.at(4);
    char port1State = byteArray.at(5);
    char port2State = byteArray.at(6);

    emit reportMPUNetworkPortsState(mpuIndex, deviceId, port1State, port2State);
}

void UdpClient::processUserCardPortStatePkt(const QByteArray &byteArray)
{
    char portId = byteArray.at(3);
    char deviceId = byteArray.at(4);
    char slotIndex = byteArray.at(5);
    char state = byteArray.at(6);
    char type = byteArray.at(7);

    emit reportUserCardPortState(portId, deviceId, slotIndex, state, type);
}

void UdpClient::replyPkt(const QNetworkDatagram &datagram)
{
    QByteArray byteArray;
    byteArray.push_back(static_cast<char>(0xaa));
    byteArray.push_back(static_cast<char>(0xbb));
    byteArray.append(datagram.data());

    sendPacket(datagram.makeReply(byteArray));
}
