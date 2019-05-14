//
// Created by kin on 19-5-8.
//

#ifndef STM32CHEAP_W5500SOCKET_H
#define STM32CHEAP_W5500SOCKET_H

#include "CommonStruct.h"
#include <memory>
#include <functional>

class W5500Manager;

class W5500Socket {
public:
    enum State{
        CLOSED,
        OPEN,
        CONNECTED
    };

public:
    friend class W5500Manager;
    W5500Socket(const std::weak_ptr<W5500Manager>& manager, SocketInfo info);
    void setDestIp(uint8_t* Ip);
    void setDestPort(uint16_t Port);

    virtual ~W5500Socket();

public:
    std::function<void(std::string)> OnReceive;

    State getState();

    void Transmit(std::string message);
    void Transmit(void* buffer, int bufferSize, EndPoint endPoint);
    void Transmit(void* buffer, int bufferSize);
    bool Receive(std::string& message);
    int Receive(void* buffer, int bufferSize);

private:
    std::weak_ptr<W5500Manager> manager;
    SocketInfo info;
};


#endif //STM32CHEAP_W5500SOCKET_H
