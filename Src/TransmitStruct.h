//
// Created by kin on 19-5-13.
//

#ifndef STM32CHEAP_TRANSMITSTRUCT_H
#define STM32CHEAP_TRANSMITSTRUCT_H

#pragma pack(1)

enum class MessageType : uint8_t {
    Unknown,
    HeartBeat,
    SetMac,
    SetServerIp,
    SetServerPort,
    WakePc,
    ASK = 0x21
};

struct MessageHead{
    MessageType Type;

    union {
        uint16_t Port;
        uint8_t Ip[4];
        uint8_t Mac[6];
    };

};

struct EndPoint{
    uint8_t Ip[4];
    uint16_t Port;
    uint16_t Size;
};

struct MessageInfo{
    EndPoint from;
    MessageHead head;
};



#pragma pack()

#endif //STM32CHEAP_TRANSMITSTRUCT_H
