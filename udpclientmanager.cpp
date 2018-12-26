#include "udpclientmanager.h"
#include <QtDebug>
#include "udpclient.h"
#include "utility.h"


UdpClientManager::UdpClientManager(QObject *parent) : QObject(parent)
{
    QString svrIpList[2];
    ushort svrPortList[2];
    Utility::loadSvrAddrConfig(svrIpList[0], svrPortList[0], svrIpList[1], svrPortList[1]);

    for (int i = 0; i < 2; i++) {
        auto client = new UdpClient;
        connect(client, SIGNAL(heartbeatTimeout()), this, SIGNAL(heartbeatTimeout()));
        connect(client, SIGNAL(reportMainCardState(char,char,char)),
                this, SIGNAL(reportMainCardState(char,char,char)));
        connect(client, SIGNAL(reportUserCardState(char,char,char,char)),
                this, SIGNAL(reportUserCardState(char,char,char,char)));
        connect(client, SIGNAL(reportCWPState(char,char,QByteArray,QByteArray,QByteArray,QByteArray)),
                this, SIGNAL(reportCWPState(char,char,QByteArray,QByteArray,QByteArray,QByteArray)));
        connect(client, SIGNAL(reportDeviceInfo(char,char,QByteArray)),
                this, SIGNAL(reportDeviceInfo(char,char,QByteArray)));
        connect(client, SIGNAL(reportEthPortsState(char,char,char,char)),
                this, SIGNAL(reportEthPortsState(char,char,char,char)));
        connect(client, SIGNAL(reportUserCardPortState(char,char,char,char,char)),
                this, SIGNAL(reportUserCardPortState(char,char,char,char,char)));
        if (client->initSock(svrIpList[i], svrPortList[i], QString("0.0.0.0"), 0))
            vecUdpClients_.push_back(client);
    }
}

UdpClientManager::~UdpClientManager()
{
    foreach (UdpClient *client, vecUdpClients_) {
        delete client;
    }
    vecUdpClients_.clear();
}

void UdpClientManager::onSvrAddrChanged(const QString &svr1Ip, ushort svr1Port,
                                        const QString &svr2Ip, ushort svr2Port)
{
    QString svrIp;
    ushort svrPort = 0;

    vecUdpClients_.at(0)->getSvrAddr(svrIp, svrPort);
    if ((svrIp != svr1Ip) || (svrPort != svr1Port)) {
        vecUdpClients_.at(0)->updateSvrAddr(svr1Ip, svr1Port);
    }

    vecUdpClients_.at(1)->getSvrAddr(svrIp, svrPort);
    if ((svrIp != svr2Ip) || (svrPort != svr2Port)) {
        vecUdpClients_.at(1)->updateSvrAddr(svr2Ip, svr2Port);
    }
}
