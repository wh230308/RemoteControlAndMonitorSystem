#include "udpclientmanager.h"
#include <QtDebug>
#include "udpclient.h"

static const char *svrAddr[] = { "192.168.0.230", "192.168.0.250" };
#define SVR_PORT 3000

UdpClientManager::UdpClientManager(QObject *parent) : QObject(parent)
{
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
        if (client->initSock(svrAddr[i], SVR_PORT))
            udpClientList.push_back(client);
    }
}

UdpClientManager::~UdpClientManager()
{
    foreach (UdpClient *client, udpClientList) {
        delete client;
    }
    udpClientList.clear();
}
