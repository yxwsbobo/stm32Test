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

W5500Manager::W5500Manager(const W5500DeviceInfo &w5500DeviceInfo, std::unique_ptr<W5500Config> config)
        :deviceInfo{w5500DeviceInfo}, config{std::move(config)}
{
}

void W5500Manager::reStart() {

    HAL_GPIO_WritePin(deviceInfo.Reset.Port, deviceInfo.Reset.Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(deviceInfo.Reset.Port, deviceInfo.Reset.Pin, GPIO_PIN_SET);
    HAL_Delay(2000);

    setMacAddress(config->loadMacAddress());

    if (config->isManual()) {
        setIpInfo(config->loadIpInfo());
    } else {
        setIpInfoAuto();
    }
}

void W5500Manager::setMacAddress(std::string macAddress) {
    uint8_t buff[16];
    sscanf( macAddress.c_str(), "%x-%x-%x-%x-%x-%x",buff, buff+1, buff+2, buff+3, buff+4, buff+5);
    WriteSPIRegister((uint16_t)ModeRegisterConfigAddress::SourceMac,buff,6);
}

void W5500Manager::setIpInfo(const IpInfo &ipInfo) {

    uint8_t newBuffMask[8]{};
    uint8_t newBuffIP[8]{};
    uint8_t newBuffGateway[8]{};
    uint8_t newBuffGateMac[16]{};


    uint8_t buff[8];
    sscanf( ipInfo.mask.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    WriteSPIRegister((uint16_t)ModeRegisterConfigAddress::SubnetMask,buff,4);

    sscanf(ipInfo.gateWay.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    WriteSPIRegister((uint16_t)ModeRegisterConfigAddress::Gatway, buff, 4);

    sscanf(ipInfo.Ip.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
    WriteSPIRegister((uint16_t)ModeRegisterConfigAddress::SourceIP, buff, 4);


//    ReadSPIRegister((uint16_t)ModeRegisterConfigAddress::SourceMac, newBuffGateMac, 6);
//
//    ReadSPIRegister((uint16_t)ModeRegisterConfigAddress::SubnetMask, newBuffMask, 4);
//
//    ReadSPIRegister((uint16_t)ModeRegisterConfigAddress::SourceIP, newBuffIP, 4);
//
//    ReadSPIRegister((uint16_t)ModeRegisterConfigAddress::Gatway, newBuffGateway, 4);


}

void W5500Manager::setIpInfoAuto() {
    //Todo get ipinfo use DHCP
}

void W5500Manager::WriteSPIRegister(uint16_t offset, uint8_t *buffer, int size, uint8_t reg, uint32_t timeout) {
    GPIOGuard guard{deviceInfo.SCS};
    auto* tValue = (uint8_t*)&offset;

    if(HAL_SPI_Transmit(deviceInfo.hspi, tValue + 1, 1, timeout) != HAL_OK){
        return;
    }
    if(HAL_SPI_Transmit(deviceInfo.hspi, tValue, 1, timeout) != HAL_OK){
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
    auto* tValue = (uint8_t*)&offset;

    if(HAL_SPI_Transmit(deviceInfo.hspi, tValue + 1, 1, timeout) != HAL_OK){
        return;
    }
    if(HAL_SPI_Transmit(deviceInfo.hspi, tValue, 1, timeout) != HAL_OK){
        return;
    }

    uint8_t ReadFlag = reg;
    if(HAL_SPI_Transmit(deviceInfo.hspi,&ReadFlag, 1, timeout) != HAL_OK){
        return;
    }

    HAL_SPI_Receive(deviceInfo.hspi, (uint8_t*)buff, size,timeout);
}

W5500Socket W5500Manager::newSocket(std::string destIp, uint16_t destPort, SocketType type) {
    SocketInfo info;
    info.destIp = std::move(destIp);
    info.destPort = destPort;
    info.type = type;

    //Todo Need check is socket empty
    info.index = 8;

    for (int i = 0; i < 8; ++i) {
        if(!socketsState[i]){
            info.index = i;
            socketsState[i] = true;
            break;
        }
    }

    auto temp = shared_from_this();

    return W5500Socket(shared_from_this(), info);
}

void W5500Manager::releaseSocket(int index) {
    socketsState[index] = false;
}


