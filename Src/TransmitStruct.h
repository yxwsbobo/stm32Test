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
    uint8_t CheckSum[4] ={0x00,0X4B, 0X69, 0X6E};
    MessageType Type{MessageType::Unknown};

    union {
        uint16_t Port;
        uint8_t Ip[4];
        uint8_t Mac[6]{};
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
