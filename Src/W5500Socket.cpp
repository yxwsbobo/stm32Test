#include <utility>

#include <utility>

//
// Created by kin on 19-5-8.
//

#include "W5500Socket.h"
#include "W5500Manager.h"

W5500Socket::W5500Socket(const std::weak_ptr<W5500Manager>& manager, SocketInfo info)
:manager{manager}, info{std::move(info)}
{
    if(info.index>=8){
        Error_Handler();
    }
    auto socketRegister = uint8_t(info.index<<5) | (uint8_t)SocketRegister::Register;

    if(auto m = manager.lock()){
        uint8_t port[2] = {0x13, 0x88};
        m->WriteSPIRegister((uint16_t)SocketRegisterAddress::Port,port, 2,socketRegister);

        uint8_t buff[8];
        sscanf(info.destIp.c_str(), "%d.%d.%d.%d",buff, buff + 1, buff + 2, buff + 3);
        m->WriteSPIRegister((uint16_t)SocketRegisterAddress::DestIp, buff, 4,socketRegister);
        m->WriteSPIRegister((uint16_t)SocketRegisterAddress::Port,port, 2,socketRegister);

        if(info.type == SocketType::UDP){
            auto UDPValue = (uint8_t)SocketModeRegisterValue::UDP;
            m->WriteSPIRegister((uint16_t)SocketRegisterAddress::ModeRegister,&UDPValue, 1,socketRegister);
        }else if(info.type == SocketType::TCP){
            auto TCPValue = (uint8_t)SocketModeRegisterValue::TCP;
            m->WriteSPIRegister((uint16_t)SocketRegisterAddress::ModeRegister,&TCPValue, 1,socketRegister);
        }

        auto OpenValue = (uint8_t)SocketConfigRegisterValue::Open;
        m->WriteSPIRegister((uint16_t)SocketRegisterAddress::ConfigRegister,&OpenValue, 1,socketRegister);

        HAL_Delay(20);

        uint8_t state;
        m->ReadSPIRegister((uint8_t)SocketRegisterAddress::StateRegister, &state, 1, socketRegister);

        if(state != (uint8_t)SocketStateRegisterValue::UDP) {
            for (int i = 0; i < 20; ++i) {
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

void W5500Socket::Send(std::string message) {

}
