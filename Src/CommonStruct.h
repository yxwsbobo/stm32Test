//
// Created by kin on 19-5-7.
//

#ifndef STM32CHEAP_COMMONSTRUCT_H
#define STM32CHEAP_COMMONSTRUCT_H

#include <string>
#include "TransmitStruct.h"

enum class SocketType{
    TCP,
    UDP
};

enum class ModeRegister{
    MR = 0,
    ForceARP = 0X2,
    PPPOE = 0X8,
    PingBlock = 0X10,
    WakeOnLan = 0X20,
    Reset = 0X80
};

enum class ModeRegisterConfigAddress{
    Gatway = 0X1,
    SubnetMask = 0X5,
    SourceMac = 0X9,
    SourceIP = 0XF,
    InterruptLevel = 0x13,
    InterruptRegister = 0x15,
    InterruptMaskRegister = 0x16,
    SocketInterruptRegister = 0x17,
    SocketInterruptMaskRegister = 0x18
};

enum class SocketRegister {
    ModeRegister = 0x0,
    Register = 0x8,
    SendBuffer = 0x10,
    ReceiveBuffer = 0x18

};

enum class SocketModeRegisterValue{
    Closed = 0x0,
    TCP = 0x1,
    UDP = 0X2,
    MACRAW = 0X4, //only Socket 0
    Multicast = 0x80
};

enum class SocketCommandRegisterValue{
    Open = 0x1,
    Listen = 0x2,
    Connect = 0x4,
    DisConnect = 0x8,
    Close = 0x10,
    Send = 0x20,
    SendMac = 0x21,
    SendKeep = 0x22,
    Receive = 0x40
};

enum class SocketStateRegisterValue{
    Closed = 0x0,
    MACRAW = 0x2,
    Init = 0x13,
    Listen = 0x14,
    Established = 0x17,
    CloseWait = 0x1c,
    UDP = 0x22,

    //temp state
    SYNSENT = 0x15,
    SYNRECV = 0X16,
    FIN_WAIT = 0X18,
    CLOSING = 0X1A,
    TIME_WAIT = 0X1B,
    LAST_ACK = 0X1D
};

enum class SocketInterruptRegisterValue{
    Connect = 0x1,
    DisConnect = 0x2,
    Receive = 0x4,
    TimeOut = 0x8,
    SendOk = 0x10
};

enum class SocketRegisterAddress{
    ModeRegister = 0x0,
    CommandRegister = 0x1,
    InterruptRegister = 0x2,
    StateRegister = 0x3,
    Port = 0X4,
    DestMac = 0x6,
    DestIp = 0xC,
    DestPort = 0x10,
    MaxSegSize = 0x12,
    ReceiveBufferSize = 0x1E,
    TransmitBuffersize = 0X1F,
    TXReadPointer = 0x22,
    TXWritePointer = 0x24,
    RXRecievedSize = 0x26,
    RXReadPointer = 0x28,
    RXWritePointer = 0x2A,
    InterruptMaskRegister = 0x2c,
    KeepaliveTimer = 0x2F
};

enum class InterruptMaskRegisterValue{
    MagicPackage = 0x10,
    PPPOE = 0x20,
    AddressUnavaliable = 0x40,
    IpConflict = 0x80
};

enum class RegisterAccessControl{
    Read = 0x0,
    Write = 0x4
};

struct IpInfo{
    uint8_t gateWay[4];
    uint8_t mask[4];
    uint8_t Ip[4];
};

struct SocketInfo{

    uint16_t sourcePort;
    uint8_t destIp[4];
    uint16_t destPort;
    SocketType type;
    uint32_t index;
};



#endif //STM32CHEAP_COMMONSTRUCT_H
