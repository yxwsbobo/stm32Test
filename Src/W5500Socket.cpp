#include <utility>

#include <utility>

//
// Created by kin on 19-5-8.
//

#include "W5500Socket.h"
#include "W5500Manager.h"
#include <string.h>

W5500Socket::W5500Socket(const std::weak_ptr<W5500Manager>& manager, SocketInfo info)
:manager{manager}, info{std::move(info)}
{
    if(info.index>=8){
        Error_Handler();
    }

    if(auto m = manager.lock()){
        //Todo SelectLocalPort
        m->setSocketPort(info.index, info.sourcePort);

        if(info.type == SocketType::UDP){
            m->writeRegister(info.index, SocketModeRegisterValue::UDP);
        }else if(info.type == SocketType::TCP){
            m->writeRegister(info.index, SocketModeRegisterValue::TCP);
        }

        m->writeRegister(info.index, SocketCommandRegisterValue::Open);

        HAL_Delay(20);

        auto state = m->readSocketState(info.index);

        //Todo need to fix when TCP
        if(state != SocketStateRegisterValue::UDP) {
            for (int i = 0; i < 40; ++i) {
                HAL_Delay(100);
                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            }
        }
    }
}

W5500Socket::~W5500Socket() {
    if(auto m = manager.lock()){
        m->releaseSocket(info.index);
    }
}

W5500Socket::State W5500Socket::getState() {
    static int i = 0;
    if(i++ >= 20){
        return CONNECTED;
    }
    return CLOSED;
}

void W5500Socket::Transmit(std::string message) {

    if(auto m = manager.lock()){

        m->setSocketDestIp(info.index, info.destIp);
        m->setSocketDestPort(info.index, info.destPort);

        uint16_t offset = m->getTXWritePointer(info.index);
        message += ": " + std::to_string(offset) + "\r\n";

        m->writeTXWriteBuffer(info.index, (void*)message.c_str(),message.size());

        m->writeRegister(info.index, SocketCommandRegisterValue::Send);
    }
}

void W5500Socket::Transmit(void *buffer, int bufferSize) {
    EndPoint endPoint;
    memcpy(endPoint.Ip, info.destIp,4);
    endPoint.Port = info.destPort;
    endPoint.Size = bufferSize;

    return Transmit(buffer, bufferSize, endPoint);
}

void W5500Socket::Transmit(void *buffer, int bufferSize, EndPoint endPoint) {
    if(auto m = manager.lock()){

        m->setSocketDestIp(info.index, endPoint.Ip);
        m->setSocketDestPort(info.index, endPoint.Port);

        uint16_t offset = m->getTXWritePointer(info.index);

        m->writeTXWriteBuffer(info.index, buffer, bufferSize);

        m->writeRegister(info.index, SocketCommandRegisterValue::Send);
    }
}

bool W5500Socket::Receive(std::string &message) {

    char buff[128]{};
    if(Receive(buff,128) != 0){
        message = std::string(buff+8);
        return true;
    }
    return false;
}

int W5500Socket::Receive(void *buffer, int size) {
    int result = 0;
    if(auto m = manager.lock()){

        if(!GlobalFlag)
            return result;
        GlobalFlag = 0;

        uint8_t iValue;
        m->readRegister(ModeRegisterConfigAddress::InterruptRegister, &iValue, 1);

        iValue &= 0xf0;
        m->writeRegister(ModeRegisterConfigAddress::InterruptRegister, &iValue, 1);

        if(iValue & (uint8_t)InterruptMaskRegisterValue::AddressUnavaliable){
        }

        uint8_t socketInterruptRegister = 0;
        m->readRegister(ModeRegisterConfigAddress::SocketInterruptRegister, &socketInterruptRegister, 1);
        m->writeRegister(ModeRegisterConfigAddress::SocketInterruptRegister, &socketInterruptRegister, 1);
        if((socketInterruptRegister & info.index) == info.index){
            auto iType = m->readSocketInterruptRegister(info.index);
            if(iType & (uint8_t)SocketInterruptRegisterValue::Receive)
            {
                auto receivedSize = m->getRXReceivedSize(info.index);
                if(receivedSize == 0){
                    return result;
                }

                result = m->readRXReceivedBuffer(info.index, buffer, size);

                m->writeRegister(info.index, SocketCommandRegisterValue::Receive);
            }
        }
    }

    return result;
}

void W5500Socket::setDestIp(uint8_t *Ip) {
    if(auto m = manager.lock()) {
        m->setSocketDestIp(info.index, Ip);
    }
}

void W5500Socket::setDestPort(uint16_t Port) {
    if(auto m = manager.lock()) {
        m->setSocketDestPort(info.index, Port);
    }
}

W5500Socket::W5500Socket() {}

