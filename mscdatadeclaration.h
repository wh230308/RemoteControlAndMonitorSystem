#ifndef MSCDATADECLARATION_H
#define MSCDATADECLARATION_H

#include <QVector>

class CustomSvrLabel;
class CustomLIULabel;

// SVR或LIU主控板网口状态
struct EthPortsState
{
    int mpuFlag; // 左右MPU板标记，0表示左侧MPU板，1表示右侧MPU板，仅设备为LIU时有效
    int port1State; // 网口1状态，0表示端口，1表示连接
    int port2State; // 同上
};

// LIU用户板端口状态
struct CardPortState
{
    int portId;
    int type;
    int state; // 端口状态，0表示未激活使用，1表示激活使用
};

struct DeviceState
{
    DeviceState(bool isChanged, int deviceId, int slotIndex, int cardType, int runningState)
    {
        this->isChanged = isChanged;
        this->deviceId = deviceId;
        this->slotIndex = slotIndex;
        this->cardType = cardType;
        this->runningState = runningState;
        ethPortsState = nullptr;
        for (int i = 0; i < 2; i++)
            cardPortStates[i] = nullptr;
    }

    bool isChanged;
    int deviceId;
    int slotIndex; // LIU板卡槽位号，仅设备为LIU时有效
    int cardType; // LIU板卡类型，仅设备为LIU且板卡为用户卡时有效
    int runningState; // SVR或LIU运行状态：0表示故障；1表示正常
    EthPortsState *ethPortsState;
    CardPortState *cardPortStates[2];
};

struct Device
{
    Device(int type, CustomSvrLabel *lblSvrItem)
    {
        this->type = type;
        this->lblSvrItem = lblSvrItem;
    }

    Device(int type, CustomLIULabel *lblLIUItem)
    {
        this->type = type;
        this->lblLIUItem = lblLIUItem;
    }

    int type;
    union
    {
        CustomSvrLabel *lblSvrItem;
        CustomLIULabel *lblLIUItem;
    };
};

#endif // MSCDATADECLARATION_H
