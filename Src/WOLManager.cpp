//
// Created by kin on 19-4-30.
//

#include "WOLManager.h"

#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "ConfigUseFlash.h"
#include <string.h>


WOLManager::WOLManager(const W5500DeviceInfo &w5500DeviceInfo, const LEDDeviceInfo &ledDeviceInfo)
        : w5500Manager{std::make_shared<W5500Manager>(w5500DeviceInfo, std::unique_ptr<W5500Config>(new W5500Config))},
        ledDeviceInfo{ledDeviceInfo}
{
    configFlash.LoadConfig();
    w5500Manager->reStart();
}

void WOLManager::Test(int speed) {
    HAL_Delay(speed);
    HAL_GPIO_TogglePin(ledDeviceInfo.Led.Port, ledDeviceInfo.Led.Pin);
}

int WOLManager::Run() {

    uint8_t Buffer[64];
    auto testSocket = w5500Manager->newSocket(loadServerInfo());

    uint32_t i = 0;
    int j = 0;

    while(Running)
    {
        if(testSocket->Receive(Buffer, 64)){
            auto info = FillMessageInfo(Buffer,64);
            ProcessInfo(testSocket, info);
        }

        if((i++ %1000000) == 0){
            MessageHead mh{};
            mh.Type = MessageType::HeartBeat;
            w5500Manager->getMac(mh.Mac);
            testSocket->Transmit(&mh, sizeof(mh));
            if(j++ %20 == 0){
                w5500Manager->leasedDHCP();
            }
        }
        if((i %200000) == 0){
            HAL_GPIO_TogglePin(ledDeviceInfo.Led.Port, ledDeviceInfo.Led.Pin);
        }
    }
    return 0;
}

MessageInfo WOLManager::FillMessageInfo(uint8_t *buffer, int size) {
    MessageInfo info{};
    if(size < 8){
        info.head.Type = MessageType::Unknown;
        return info;
    }
    for (int i = 0; i < 4; ++i) {
        info.from.Ip[i] = buffer[i];
    }
    info.from.Port = ChangeLittleBigEnd(*(uint16_t *)&buffer[4]);
    info.from.Size = ChangeLittleBigEnd(*(uint16_t*)&buffer[6]);
    buffer += 8;

    auto sSize = std::min((size_t)info.from.Size, sizeof(info.head));

    memcpy(&info.head, buffer, sSize);

    return info;
}

void WOLManager::ProcessInfo(std::unique_ptr<W5500Socket>& socket, MessageInfo info) {
    if(info.head.CheckSum[0] != 0x0 | info.head.CheckSum[1] != 0X4B ||
    info.head.CheckSum[2] != 0X69 || info.head.CheckSum[3] != 0x6E){
        return;
    }

    if(info.head.Type == MessageType::ASK){
        auto sSize = std::min((size_t)info.from.Size, sizeof(info.head));
        socket->Transmit(&info.head, sSize, info.from);
    }
    else if(info.head.Type == MessageType::WakePc){
        WakePc(socket, info.head.Mac);
    }
    else if(info.head.Type == MessageType::SetServerIp){
        saveServerIp(info.head.Ip);
        socket->setDestIp(info.head.Ip);

    }else if(info.head.Type == MessageType::SetServerPort){
        saveServerPort(info.head.Port);
        socket->setDestPort(info.head.Port);
    }else if(info.head.Type == MessageType::SetMac){
        saveMacAddress(info.head.Mac);
        //Need Restart
//        w5500Manager->setMacAddress(info.head.Mac);
    }
}

void WOLManager::WakePc(std::unique_ptr<W5500Socket> &socket, const uint8_t *mac) {
    uint8_t sendBuffer[102];
    EndPoint broadPoint{};
    w5500Manager->getBroadAddress(broadPoint.Ip);
    broadPoint.Port = 0x9;

    for (int i = 0; i < 6; ++i) {
        sendBuffer[i] = 0xFF;
    }

    for (int j = 1; j < 17; ++j) {
        for (int i = 0; i < 6; ++i) {
            sendBuffer[j*6 +i] = mac[i];
        }
    }

    socket->Transmit(sendBuffer,102,broadPoint);

    for (int i = 0; i < 20; ++i) {
        Test(100);
    }
}

void WOLManager::saveServerIp(uint8_t *ip) {
    memcpy(configFlash.ServerIp, ip, 4);
    configFlash.SaveConfig();
}

void WOLManager::saveServerPort(uint16_t port) {
    configFlash.ServerPort = port;
    configFlash.SaveConfig();
}


EndPoint WOLManager::loadServerInfo() {
    EndPoint endPoint;
    memcpy(endPoint.Ip,configFlash.ServerIp,4);
    endPoint.Port = configFlash.ServerPort;

    return endPoint;
}

void WOLManager::saveMacAddress(uint8_t *mac) {
    memcpy(configFlash.Mac, mac, 6);
    configFlash.SaveConfig();
}

