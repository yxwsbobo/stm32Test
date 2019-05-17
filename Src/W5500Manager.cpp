#include <utility>

//
// Created by kin on 19-4-30.
//

#include "W5500Manager.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "GPIOGuard.h"
#include "string.h"
#include "DHCPHelp.h"

W5500Manager::W5500Manager(const W5500DeviceInfo &w5500DeviceInfo, std::unique_ptr<W5500Config> config)
        :deviceInfo{w5500DeviceInfo}, config{std::move(config)}
{
}

void W5500Manager::reStart() {

    HAL_GPIO_WritePin(deviceInfo.Reset.Port, deviceInfo.Reset.Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(deviceInfo.Reset.Port, deviceInfo.Reset.Pin, GPIO_PIN_SET);
    HAL_Delay(2000);

    uint8_t mac[6];
    config->loadMacAddress(mac);
    setMacAddress(mac);

    if (config->isManual()) {
        setIpInfo(config->loadIpInfo());
    } else {
        setIpInfoAuto();
    }

    setInterrupt();
}

void W5500Manager::setInterrupt() {
    auto value = (uint8_t)InterruptMaskRegisterValue::IpConflict | (uint8_t)InterruptMaskRegisterValue::AddressUnavaliable;
    writeRegister(ModeRegisterConfigAddress::InterruptMaskRegister, &value, 1);

    value = 0xfE;
    writeRegister(ModeRegisterConfigAddress::SocketInterruptMaskRegister, &value, 1);
}


void W5500Manager::setIpInfo(IpInfo ipInfo) {
    setMask(ipInfo.mask);
    setGateWay(ipInfo.gateWay);
    setIp(ipInfo.Ip);
}

void W5500Manager::setIpInfoAuto() {

    IpInfo ipInfo{};
    for (int i = 0; i < 1000; ++i) {
        ipInfo = GetIpInfoFromDHCP();
        if(*(uint32_t*)ipInfo.Ip != 0){
            break;
        }
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        for (int j = 0; j < 100; ++j) {
            HAL_Delay(100);
        }
    }


//    config->saveIpInfo(ipInfo);

    return setIpInfo(ipInfo);
}

void W5500Manager::WriteSPIRegister(uint16_t offset, uint8_t *buffer, int size, uint8_t reg, uint32_t timeout) {
    GPIOGuard guard{deviceInfo.SCS};

    offset = ChangeLittleBigEnd(offset);
    if(HAL_SPI_Transmit(deviceInfo.hspi, (uint8_t*)&offset, 2, timeout) != HAL_OK){
        return;
    }

    uint8_t WriteFlag = 4;
    WriteFlag |= reg;
    if(HAL_SPI_Transmit(deviceInfo.hspi, &WriteFlag, 1, timeout) != HAL_OK){
        return;
    }

    if(HAL_SPI_Transmit(deviceInfo.hspi, buffer, size, timeout) != HAL_OK){
        return;
    }
}

void W5500Manager::ReadSPIRegister(uint16_t offset, uint8_t *buff, size_t size, uint8_t reg, uint32_t timeout) {
    GPIOGuard guard{deviceInfo.SCS};

    offset = ChangeLittleBigEnd(offset);
    if(HAL_SPI_Transmit(deviceInfo.hspi, (uint8_t*)&offset, 2, timeout) != HAL_OK){
        return;
    }

    uint8_t ReadFlag = reg;
    if(HAL_SPI_Transmit(deviceInfo.hspi,&ReadFlag, 1, timeout) != HAL_OK){
        return;
    }

    HAL_SPI_Receive(deviceInfo.hspi, (uint8_t*)buff, size,timeout);
}

//W5500Socket W5500Manager::newSocket(std::string destIp, uint16_t destPort,uint16_t port, SocketType type) {
//
//    EndPoint endPoint;
//    sscanf(destIp.c_str(), "%d.%d.%d.%d",&endPoint.Ip[0], &endPoint.Ip[1], &endPoint.Ip[2], &endPoint.Ip[3]);
//    endPoint.Port = destPort;
//    return newSocket(endPoint,port, type);
//}

std::unique_ptr<W5500Socket> W5500Manager::newSocket(EndPoint edPoint,uint16_t port, SocketType type) {
    SocketInfo info{};

    memcpy(info.destIp, edPoint.Ip,4);
    info.destPort = edPoint.Port;
    info.type = type;
    info.sourcePort = port;

    //Todo Need check is socket empty
    info.index = 8;

    for (int i = 0; i < 8; ++i) {
        if(!socketsState[i]){
            info.index = i;
            socketsState[i] = true;
            break;
        }
    }

    return std::unique_ptr<W5500Socket>(new W5500Socket(shared_from_this(), info));
}


uint8_t W5500Manager::GetSocketRegister(int index) {
    return uint8_t(index<<5) | (uint8_t)SocketRegister::Register;
}

uint8_t W5500Manager::GetSocketSendRegister(int index) {
    return uint8_t(index<<5) | (uint8_t)SocketRegister::SendBuffer;
}

uint8_t W5500Manager::GetSocketReceiveRegister(int index) {
    return uint8_t(index<<5) | (uint8_t)SocketRegister::ReceiveBuffer;
}

void W5500Manager::releaseSocket(int index) {
    writeRegister(index, SocketCommandRegisterValue::Close);
    writeRegister(index, SocketModeRegisterValue::Closed);
    setSocketPort(index, 0);

    socketsState[index] = false;
}

void W5500Manager::setMacAddress(std::string macAddress) {
    uint8_t buff[16];
    sscanf( macAddress.c_str(), "%x-%x-%x-%x-%x-%x",buff, buff+1, buff+2, buff+3, buff+4, buff+5);
    return setMacAddress(buff);
}

void W5500Manager::setMacAddress(uint8_t *macAddress) {
    writeRegister(ModeRegisterConfigAddress::SourceMac, macAddress, 6);
}

void W5500Manager::setIp(const std::string& Ip) {
    uint8_t buff[8];
    sscanf(Ip.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);

    setIp(buff);
}

void W5500Manager::setIp(uint8_t *Ip) {
    writeRegister(ModeRegisterConfigAddress::SourceIP, Ip, 4);
}



void W5500Manager::readIp(std::string &Ip) {
    uint8_t buff[8];

    ReadSPIRegister((uint16_t)ModeRegisterConfigAddress::SourceIP, buff, 4);

    char ip[20];
    sprintf(ip,"%d.%d.%d.%d", buff[0], buff[1], buff[2], buff[3]);
    Ip = ip;
}

void W5500Manager::setMask(const std::string& mask) {
    uint8_t buff[8];
    sscanf( mask.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    return setMask(buff);
}

void W5500Manager::setMask(uint8_t *mask) {
    writeRegister(ModeRegisterConfigAddress::SubnetMask, mask, 4);
}

void W5500Manager::setGateWay(const std::string& gateWay) {
    uint8_t buff[8];
    sscanf(gateWay.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    return setGateWay(buff);
}

void W5500Manager::setGateWay(uint8_t *gateWay) {
    writeRegister(ModeRegisterConfigAddress::Gatway, gateWay, 4);
}

void W5500Manager::setSocketPort(int index, uint16_t port) {
    port = ChangeLittleBigEnd(port);
    WriteSPIRegister((uint16_t) SocketRegisterAddress::Port, (uint8_t*)&port, 2, GetSocketRegister(index));
}

void W5500Manager::writeRegister(int index, SocketModeRegisterValue value) {
    WriteSPIRegister((uint16_t)SocketRegisterAddress::ModeRegister,(uint8_t*)&value, 1,GetSocketRegister(index));
}

void W5500Manager::writeRegister(int index, SocketCommandRegisterValue value) {
    WriteSPIRegister((uint16_t)SocketRegisterAddress::CommandRegister,(uint8_t*)&value, 1,GetSocketRegister(index));
}

void W5500Manager::writeRegister(int index, SocketInterruptRegisterValue value,bool isMask) {
    auto add = (uint16_t)SocketRegisterAddress::InterruptMaskRegister;
    if(!isMask){
        add = (uint16_t)SocketRegisterAddress::InterruptRegister;
    }
    WriteSPIRegister(add,(uint8_t*)&value, 1,GetSocketRegister(index));
}

uint8_t W5500Manager::readSocketInterruptRegister(int index,bool isMask) {
    uint8_t value = 0;
    auto add = (uint16_t)SocketRegisterAddress::InterruptMaskRegister;
    if(!isMask){
        add = (uint16_t)SocketRegisterAddress::InterruptRegister;
    }
    ReadSPIRegister(add,(uint8_t*)&value, 1,GetSocketRegister(index));
    WriteSPIRegister(add,(uint8_t*)&value, 1,GetSocketRegister(index));
    return value;
}

SocketStateRegisterValue W5500Manager::readSocketState(int index) {
    uint8_t state;
    ReadSPIRegister((uint8_t)SocketRegisterAddress::StateRegister, &state, 1, GetSocketRegister(index));
    return SocketStateRegisterValue(state);
}

void W5500Manager::setSocketDestIp(int index, const std::string &Ip) {
    uint8_t buff[8];
    sscanf(Ip.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    setSocketDestIp(index, buff);
}

void W5500Manager::setSocketDestIp(int index, uint8_t *Ip) {
    WriteSPIRegister((uint16_t)SocketRegisterAddress::DestIp, Ip, 4,GetSocketRegister(index));
}


void W5500Manager::setSocketDestPort(int index, uint16_t port) {
    port = ChangeLittleBigEnd(port);
    WriteSPIRegister((uint16_t)SocketRegisterAddress::DestPort, (uint8_t*)&port, 2,GetSocketRegister(index));
}

uint16_t W5500Manager::getTXWritePointer(int index) {
    uint16_t offset;
    ReadSPIRegister((uint8_t)SocketRegisterAddress::TXWritePointer, (uint8_t*)&offset, 2, GetSocketRegister(index));
    return ChangeLittleBigEnd(offset);
}

void W5500Manager::setTXWritePointer(int index, uint16_t offset) {
    offset = ChangeLittleBigEnd(offset);
    WriteSPIRegister((uint8_t)SocketRegisterAddress::TXWritePointer ,(uint8_t*)&offset, 2, GetSocketRegister(index));
}

void W5500Manager::writeTXWriteBuffer(int index, void *buffer, int size) {

    //Todo fix offset
    auto offset = getTXWritePointer(index);

    WriteSPIRegister(offset, (uint8_t*)buffer, size, GetSocketSendRegister(index));

    offset += size;

    setTXWritePointer(index, offset);
}

uint16_t W5500Manager::getRXReceivedSize(int index) {
    uint16_t receivedSize = 0;
    ReadSPIRegister((uint8_t)SocketRegisterAddress::RXRecievedSize,(uint8_t*)&receivedSize, 2,GetSocketRegister(index));
    return ChangeLittleBigEnd(receivedSize);
}

uint16_t W5500Manager::readRXReceivedBuffer(int index, void *buffer, int bufferSize, int readSize) {
    uint16_t realSize = readSize;
    if(readSize <= 0){
        realSize = getRXReceivedSize(index);
    }
    realSize = std::min((int)realSize, bufferSize);

    //Todo fix offset
    uint16_t offset = getRXReadPointer(index);

    ReadSPIRegister(offset, (uint8_t*)buffer, realSize, GetSocketReceiveRegister(index));

    offset += realSize;

    setRXReadPointer(index, offset);

    return realSize;
}

uint16_t W5500Manager::getRXReadPointer(int index) {
    uint16_t offset;
    ReadSPIRegister((uint8_t)SocketRegisterAddress::RXReadPointer, (uint8_t*)&offset, 2, GetSocketRegister(index));
    return ChangeLittleBigEnd(offset);
}

void W5500Manager::setRXReadPointer(int index, uint16_t offset) {
    offset = ChangeLittleBigEnd(offset);
    WriteSPIRegister((uint8_t)SocketRegisterAddress::RXReadPointer ,(uint8_t*)&offset, 2, GetSocketRegister(index));
}

void W5500Manager::writeRegister(ModeRegisterConfigAddress value, void *buffer, int size) {
    WriteSPIRegister((uint16_t)value,(uint8_t*)buffer,size);
}

void W5500Manager::readRegister(ModeRegisterConfigAddress value, void *buffer, int size) {
    ReadSPIRegister((uint16_t)value, (uint8_t*)buffer, size);
}

void W5500Manager::getBroadAddress(uint8_t *address) {
    auto ipInfo = config->loadIpInfo();
    for (int i = 0; i < 4; ++i) {
        address[i] = ~ipInfo.mask[i];
        address[i] |= ipInfo.Ip[i];
    }
}

void W5500Manager::getMac(uint8_t *mac) {
    config->loadMacAddress(mac);
}

#pragma pack(1)
struct DHCPRecvStruct{
    uint8_t Ip[4];
    uint16_t Port;
    uint16_t Size;
    RIP_MSG msg;
};
#pragma pack()

IpInfo W5500Manager::GetIpInfoFromDHCP() {
    IpInfo result{};
    setIpInfo(result);

    uint8_t mac[6];
    config->loadMacAddress(mac);
    dhcpHelp = DHCPHelp{mac, "SamingWake"};

    EndPoint endPoint{{255,255,255,255}, 67};
    dhcpSocket = newSocket(endPoint,68);

    auto fn =[&](RIP_MSG& tmpMsg){
        EndPoint destPoint{};
        dhcpHelp.GetDestIp(destPoint.Ip);
        dhcpHelp.GetDestPort(destPoint.Port);
        dhcpSocket->Transmit(&tmpMsg, sizeof(tmpMsg), destPoint);
        HAL_Delay(50);
        DHCPRecvStruct recvStruct{};
        auto recvFlag = false;
        for (int i = 0; i < 30; ++i) {
            GlobalFlag = 1;
            if(dhcpSocket->Receive(&recvStruct, sizeof(recvStruct))){
                dhcpHelp.parseMsgAndBuild(recvStruct.msg, tmpMsg, recvStruct.Ip, ChangeLittleBigEnd(recvStruct.Port));
                recvFlag = true;
            }else{
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
                HAL_Delay(100);
            }
        }
        if(recvFlag){
            return dhcpHelp.getState();
        }else{
            return DHCPHelp::Fail;
        }
    };

    auto msg = dhcpHelp.BuildDiscover();

    int tryTimes = 0;
    while(true){
        auto tState = fn(msg);
        if(tState == DHCPHelp::Fail){
            return result;
        }
        if(tState == DHCPHelp::Success){
            dhcpHelp.GetIp(result.Ip);
            dhcpHelp.GetMask(result.mask);
            dhcpHelp.GetGateway(result.gateWay);
            return result;
        }
        if(tryTimes++ >=20){
            return result;
        }
    }
}

void W5500Manager::leasedDHCP() {
    auto msg = dhcpHelp.BuildReRequest();
    EndPoint destPoint{};
    dhcpHelp.GetDestIp(destPoint.Ip);
    dhcpHelp.GetDestPort(destPoint.Port);
    dhcpSocket->Transmit(&msg, sizeof(msg), destPoint);
}




