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
#include "DHCPHelp.h"

class W5500Manager: public std::enable_shared_from_this<W5500Manager> {
public:

//    W5500Socket newSocket(std::string destIp, uint16_t destPort,uint16_t port, SocketType type = SocketType::UDP);
    std::unique_ptr<W5500Socket> newSocket(EndPoint info, uint16_t port = 5000, SocketType type = SocketType::UDP);

public:

    W5500Manager(const W5500DeviceInfo& w5500DeviceInfo, std::unique_ptr<W5500Config> config);

    void reStart();

    void setIpInfo(IpInfo ipInfo);
    void setIpInfoAuto();
    void getBroadAddress(uint8_t *address);
    void getMac(uint8_t* mac);
    void setMacAddress(uint8_t *macAddress);

    void leasedDHCP();

    void setIp(uint8_t* Ip);
    void setMask(uint8_t* mask);
    void setGateWay(uint8_t* gateWay);
private:
    void WriteSPIRegister(uint16_t offset, uint8_t *buffer, int size, uint8_t reg = 0, uint32_t timeout = 5000);
    void ReadSPIRegister(uint16_t offset, uint8_t *buff, size_t size, uint8_t reg = 0, uint32_t timeout = 5000);

    void setMacAddress(std::string macAddress);
    void setIp(const std::string& Ip);
    void readIp(std::string& Ip);
    void setMask(const std::string& mask);
    void setGateWay(const std::string& gateWay);


    friend W5500Socket;
    uint8_t GetSocketRegister(int index);
    uint8_t GetSocketSendRegister(int index);
    uint8_t GetSocketReceiveRegister(int index);
    void releaseSocket(int index);
    void setSocketPort(int index, uint16_t port);
    void writeRegister(int index, SocketModeRegisterValue value);
    void writeRegister(int index, SocketCommandRegisterValue value);
    void writeRegister(int index, SocketInterruptRegisterValue value, bool isMask = false);
    uint8_t readSocketInterruptRegister(int index, bool isMask = false);
    void writeRegister(ModeRegisterConfigAddress value, void* buffer, int size);
    void readRegister(ModeRegisterConfigAddress value, void* buffer, int size);


    SocketStateRegisterValue readSocketState(int index);

    void setSocketDestIp(int index, const std::string& Ip);
    void setSocketDestIp(int index, uint8_t* Ip);
    void setSocketDestPort(int index, uint16_t port);
    uint16_t getTXWritePointer(int index);
    void setTXWritePointer(int index, uint16_t offset);
    void writeTXWriteBuffer(int index, void* buffer, int size);
    uint16_t getRXReceivedSize(int index);
    uint16_t getRXReadPointer(int index);
    void setRXReadPointer(int index, uint16_t offset);

    /**
     *
     * @param index socket index
     * @param buffer
     * @param bufferSize
     * @param readSize -1 is read all
     * @return real read size
     */
    uint16_t readRXReceivedBuffer(int index, void* buffer, int bufferSize, int readSize = -1);


private:
    std::unique_ptr<W5500Socket> dhcpSocket;
    DHCPHelp dhcpHelp;
    std::unique_ptr<W5500Config> config;
    W5500DeviceInfo deviceInfo;
    std::bitset<8> socketsState{};

    void setInterrupt();

    IpInfo GetIpInfoFromDHCP();
};


#endif //STM32CHEAP_W5500MANAGER_H
