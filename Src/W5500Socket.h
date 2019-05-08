//
// Created by kin on 19-5-8.
//

#ifndef STM32CHEAP_W5500SOCKET_H
#define STM32CHEAP_W5500SOCKET_H

#include "CommonStruct.h"
#include <memory>


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

    virtual ~W5500Socket();

    State getState();

    void Send(std::string message);

private:
    std::weak_ptr<W5500Manager> manager;
    SocketInfo info;
};


#endif //STM32CHEAP_W5500SOCKET_H
