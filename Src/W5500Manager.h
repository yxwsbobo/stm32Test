//
// Created by kin on 19-4-30.
//

#ifndef STM32CHEAP_W5500MANAGER_H
#define STM32CHEAP_W5500MANAGER_H

#include <string>
#include <memory>
#include <bitset>
#include "W5500Config.h"
#include "CommonStruct.h"
#include "WOLInterface.h"
#include "W5500Socket.h"

class W5500Manager: public std::enable_shared_from_this<W5500Manager> {
public:

    W5500Socket newSocket(std::string destIp, uint16_t destPort, SocketType type = SocketType::UDP);

public:

    W5500Manager(const W5500DeviceInfo& w5500DeviceInfo, std::unique_ptr<W5500Config> config);

    void reStart();

    void setIpInfo(const IpInfo& ipInfo);
    void setIpInfoAuto();

    void setMacAddress(std::string macAddress);

private:
    void WriteSPIRegister(uint16_t offset, uint8_t *buffer, int size, uint8_t reg = 0, uint32_t timeout = 5000);
    void ReadSPIRegister(uint16_t offset, uint8_t *buff, size_t size, uint8_t reg = 0, uint32_t timeout = 5000);

    friend W5500Socket;
    void releaseSocket(int index);

private:

    std::unique_ptr<W5500Config> config;
    W5500DeviceInfo deviceInfo;
    std::bitset<8> socketsState{};


};


#endif //STM32CHEAP_W5500MANAGER_H
