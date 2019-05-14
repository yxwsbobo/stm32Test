//
// Created by kin on 19-4-30.
//

#ifndef STM32CHEAP_WOLMANAGER_H
#define STM32CHEAP_WOLMANAGER_H
#include "WOLInterface.h"
#include "W5500Manager.h"
#include <memory>


class WOLManager {
public:
    WOLManager(const W5500DeviceInfo& w5500DeviceInfo, const LEDDeviceInfo& ledDeviceInfo);

    int Run();

private:
    void Test(int speed);


private:
    int Running = 1;
    std::shared_ptr<W5500Manager> w5500Manager;
    LEDDeviceInfo ledDeviceInfo;

    MessageInfo FillMessageInfo(uint8_t *buffer, int size);

    void ProcessInfo(W5500Socket& socket, MessageInfo info);

    void WakePc(W5500Socket &socket, const uint8_t* mac);

    void saveServerIp(uint8_t *ip);

    void saveServerPort(uint16_t port);

    EndPoint loadServerInfo();

    void saveMacAddress(uint8_t *mac);
};


#endif //STM32CHEAP_WOLMANAGER_H
