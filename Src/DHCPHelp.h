//
// Created by kin on 19-5-14.
//

#ifndef STM32CHEAP_DHCPHELP_H
#define STM32CHEAP_DHCPHELP_H

#include <memory>

typedef struct {
    uint8_t  op;            ///< @ref DHCP_BOOTREQUEST or @ref DHCP_BOOTREPLY
    uint8_t  htype;         ///< @ref DHCP_HTYPE10MB or @ref DHCP_HTYPE100MB
    uint8_t  hlen;          ///< @ref DHCP_HLENETHERNET
    uint8_t  hops;          ///< @ref DHCP_HOPS
    uint32_t xid;           ///< @ref DHCP_XID  This increase one every DHCP transaction.
    uint16_t secs;          ///< @ref DHCP_SECS
    uint16_t flags;         ///< @ref DHCP_FLAGSBROADCAST or @ref DHCP_FLAGSUNICAST
    uint8_t  ciaddr[4];     ///< @ref Request IP to DHCP sever
    uint8_t  yiaddr[4];     ///< @ref Offered IP from DHCP server
    uint8_t  siaddr[4];     ///< No use
    uint8_t  giaddr[4];     ///< No use
    uint8_t  chaddr[16];    ///< DHCP client 6bytes MAC address. Others is filled to zero
    uint8_t  sname[64];     ///< No use
    uint8_t  file[128];     ///< No use
    uint8_t  OPT[312]; ///< Option
} RIP_MSG;

class DHCPHelp {
public:
    enum dhcpState{
        Fail,
        GoOn,
        Success
    };

    void GetIp(uint8_t *buff);

    void GetMask(uint8_t *buff);

    void GetGateway(uint8_t *buff);

    void GetDestIp(uint8_t *destIp);

    void GetDestPort(uint16_t &Port);

    RIP_MSG BuildReRequest();

public:
    RIP_MSG BuildDiscover();

    void parseMsgAndBuild(const RIP_MSG &recvMsg, RIP_MSG &buildMsg, uint8_t *Ip, uint16_t Port);

    dhcpState getState();

private:
    uint8_t mac[6];
    std::string hName;

private:
    uint8_t recvIp[4];
    uint8_t recvMask[4];
    uint8_t recvGateway[4];
    uint8_t recvDns[4];
    uint8_t parseServerIp[4];
    uint32_t dhcp_lease_time = -1;
    uint8_t dIp[4]{255,255,255,255};
    uint16_t dPort{67};

public:
    DHCPHelp(uint8_t *mac, std::string hostName);
    void BuildBaseMsg(RIP_MSG& msg);

    DHCPHelp();

private:
    dhcpState dState = Fail;
    static constexpr int messageTypeIndex = 6;
    uint32_t nextOptPosition{};

    int parseMsg(const RIP_MSG &msg);

    void BuildRequest(RIP_MSG &msg);
};


#endif //STM32CHEAP_DHCPHELP_H
