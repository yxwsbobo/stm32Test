//
// Created by kin on 19-5-14.
//

#include <cstdint>
#include "DHCPHelp.h"
#include "WOLInterface.h"
#include "string.h"

/* DHCP state machine. */
#define STATE_DHCP_INIT          0        ///< Initialize
#define STATE_DHCP_DISCOVER      1        ///< send DISCOVER and wait OFFER
#define STATE_DHCP_REQUEST       2        ///< send REQEUST and wait ACK or NACK
#define STATE_DHCP_LEASED        3        ///< ReceiveD ACK and IP leased
#define STATE_DHCP_REREQUEST     4        ///< send REQUEST for maintaining leased IP
#define STATE_DHCP_RELEASE       5        ///< No use
#define STATE_DHCP_STOP          6        ///< Stop processing DHCP

#define DHCP_FLAGSBROADCAST      0x8000   ///< The broadcast value of flags in @ref RIP_MSG
#define DHCP_FLAGSUNICAST        0x0000   ///< The unicast   value of flags in @ref RIP_MSG

/* DHCP message OP code */
#define DHCP_BOOTREQUEST         1        ///< Request Message used in op of @ref RIP_MSG
#define DHCP_BOOTREPLY           2        ///< Reply Message used i op of @ref RIP_MSG

/* DHCP message type */
#define DHCP_DISCOVER            1        ///< DISCOVER message in OPT of @ref RIP_MSG
#define DHCP_OFFER               2        ///< OFFER message in OPT of @ref RIP_MSG
#define DHCP_REQUEST             3        ///< REQUEST message in OPT of @ref RIP_MSG
#define DHCP_DECLINE             4        ///< DECLINE message in OPT of @ref RIP_MSG
#define DHCP_ACK                 5        ///< ACK message in OPT of @ref RIP_MSG
#define DHCP_NAK                 6        ///< NACK message in OPT of @ref RIP_MSG
#define DHCP_RELEASE             7        ///< RELEASE message in OPT of @ref RIP_MSG. No use
#define DHCP_INFORM              8        ///< INFORM message in OPT of @ref RIP_MSG. No use

#define DHCP_HTYPE10MB           1        ///< Used in type of @ref RIP_MSG
#define DHCP_HTYPE100MB          2        ///< Used in type of @ref RIP_MSG

#define DHCP_HLENETHERNET        6        ///< Used in hlen of @ref RIP_MSG
#define DHCP_HOPS                0        ///< Used in hops of @ref RIP_MSG
#define DHCP_SECS                0        ///< Used in secs of @ref RIP_MSG

#define INFINITE_LEASETIME       0xffffffff	///< Infinite lease time

#define OPT_SIZE                 312               /// Max OPT size of @ref RIP_MSG
#define RIP_MSG_SIZE             (236+OPT_SIZE)    /// Max size of @ref RIP_MSG


/*
 * @brief DHCP option and value (cf. RFC1533)
 */
enum
{
    padOption               = 0,
    subnetMask              = 1,
    timerOffset             = 2,
    routersOnSubnet         = 3,
    timeServer              = 4,
    nameServer              = 5,
    dns                     = 6,
    logServer               = 7,
    cookieServer            = 8,
    lprServer               = 9,
    impressServer           = 10,
    resourceLocationServer	= 11,
    hostName                = 12,
    bootFileSize            = 13,
    meritDumpFile           = 14,
    domainName              = 15,
    swapServer              = 16,
    rootPath                = 17,
    extentionsPath          = 18,
    IPforwarding            = 19,
    nonLocalSourceRouting   = 20,
    policyFilter            = 21,
    maxDgramReasmSize       = 22,
    defaultIPTTL            = 23,
    pathMTUagingTimeout     = 24,
    pathMTUplateauTable     = 25,
    ifMTU                   = 26,
    allSubnetsLocal         = 27,
    broadcastAddr           = 28,
    performMaskDiscovery    = 29,
    maskSupplier            = 30,
    performRouterDiscovery  = 31,
    routerSolicitationAddr  = 32,
    staticRoute             = 33,
    trailerEncapsulation    = 34,
    arpCacheTimeout         = 35,
    ethernetEncapsulation   = 36,
    tcpDefaultTTL           = 37,
    tcpKeepaliveInterval    = 38,
    tcpKeepaliveGarbage     = 39,
    nisDomainName           = 40,
    nisServers              = 41,
    ntpServers              = 42,
    vendorSpecificInfo      = 43,
    netBIOSnameServer       = 44,
    netBIOSdgramDistServer	= 45,
    netBIOSnodeType         = 46,
    netBIOSscope            = 47,
    xFontServer             = 48,
    xDisplayManager         = 49,
    dhcpRequestedIPaddr     = 50,
    dhcpIPaddrLeaseTime     = 51,
    dhcpOptionOverload      = 52,
    dhcpMessageType         = 53,
    dhcpServerIdentifier    = 54,
    dhcpParamRequest        = 55,
    dhcpMsg                 = 56,
    dhcpMaxMsgSize          = 57,
    dhcpT1value             = 58,
    dhcpT2value             = 59,
    dhcpClassIdentifier     = 60,
    dhcpClientIdentifier    = 61,
    endOption               = 255
};



DHCPHelp::DHCPHelp(uint8_t *m, std::string hName) : hName(hName) {
    memcpy(mac, m, 6);
}

void DHCPHelp::BuildBaseMsg(RIP_MSG &msg) {
    memset(&msg, 0, sizeof(msg));

    msg.op = DHCP_BOOTREQUEST;
    msg.htype = DHCP_HTYPE10MB;
    msg.hlen = DHCP_HLENETHERNET;
    msg.hops = DHCP_HOPS;
    msg.xid = 0Xa600a611;
    msg.secs = ChangeLittleBigEnd(DHCP_SECS);
    msg.flags = ChangeLittleBigEnd(DHCP_FLAGSBROADCAST);

    memcpy(msg.chaddr, mac, 6);

    int index = 0;
    msg.OPT[index++] = (uint8_t)((0x63825363 & 0xFF000000) >> 24);
    msg.OPT[index++] = (uint8_t)((0x63825363 & 0x00FF0000) >> 16);
    msg.OPT[index++] = (uint8_t)((0x63825363 & 0x0000FF00) >>  8);
    msg.OPT[index++] = (uint8_t) (0x63825363 & 0x000000FF) >>  0;

    msg.OPT[index++] = dhcpMessageType;
    msg.OPT[index++] = 0x01;
    msg.OPT[index++] = DHCP_DISCOVER; //messageTypeIndex

    // Client identifier
    msg.OPT[index++] = dhcpClientIdentifier;
    msg.OPT[index++] = 0x07;
    msg.OPT[index++] = 0x01;
    for (int i = 0; i < 6; ++i) {
        msg.OPT[index++] = mac[i];
    }

    // host name
    msg.OPT[index++] = hostName;
    msg.OPT[index++] = hName.size();
    memcpy(&msg.OPT[index],hName.c_str(), hName.size());
    index+= hName.size();

    nextOptPosition = index;

}


RIP_MSG DHCPHelp::BuildDiscover() {
    RIP_MSG msg;
    BuildBaseMsg(msg);

    int index = nextOptPosition;
    msg.OPT[index++] = dhcpParamRequest;
    msg.OPT[index++] = 0x06;	// length of request
    msg.OPT[index++] = subnetMask;
    msg.OPT[index++] = routersOnSubnet;
    msg.OPT[index++] = dns;
    msg.OPT[index++] = domainName;
    msg.OPT[index++] = dhcpT1value;
    msg.OPT[index++] = dhcpT2value;
    msg.OPT[index++] = endOption;

    dState = GoOn;
    return msg;
}


void DHCPHelp::BuildRequest(RIP_MSG &msg) {
    BuildBaseMsg(msg);

    msg.flags = ChangeLittleBigEnd(DHCP_FLAGSBROADCAST);

    msg.OPT[messageTypeIndex] = DHCP_REQUEST;

    int index = nextOptPosition;

    msg.OPT[index++] = dhcpRequestedIPaddr;
    msg.OPT[index++] = 0x04;
    msg.OPT[index++] = recvIp[0];
    msg.OPT[index++] = recvIp[1];
    msg.OPT[index++] = recvIp[2];
    msg.OPT[index++] = recvIp[3];

    msg.OPT[index++] = dhcpServerIdentifier;
    msg.OPT[index++] = 0x04;
    msg.OPT[index++] = parseServerIp[0];
    msg.OPT[index++] = parseServerIp[1];
    msg.OPT[index++] = parseServerIp[2];
    msg.OPT[index++] = parseServerIp[3];

    msg.OPT[index++] = dhcpParamRequest;
    msg.OPT[index++] = 0x08;	// length of request
    msg.OPT[index++] = subnetMask;
    msg.OPT[index++] = routersOnSubnet;
    msg.OPT[index++] = dns;
    msg.OPT[index++] = domainName;
    msg.OPT[index++] = dhcpT1value;
    msg.OPT[index++] = dhcpT2value;
    msg.OPT[index++] = performRouterDiscovery;
    msg.OPT[index++] = staticRoute;
    msg.OPT[index++] = endOption;

    //Todo OnRequest
//    msg.flags = ChangeLittleBigEnd(DHCP_FLAGSUNICAST);
//    memcpy(msg.ciaddr, offerMsg->yiaddr, 4);

}

void DHCPHelp::parseMsgAndBuild(const RIP_MSG &recvMsg, RIP_MSG &buildMsg, uint8_t *Ip, uint16_t Port) {
    if(Port != 67){
        return;
    }
    if(memcmp(recvMsg.chaddr, mac,6) != 0){
        return;
    }
    auto type = parseMsg(recvMsg);

    if(type == DHCP_OFFER || type == DHCP_ACK){
        memcpy(recvIp, recvMsg.yiaddr,4);
    }

    if(type == DHCP_NAK){
        dState = Fail;
    }else if(type == DHCP_OFFER){
        memset(dIp,255,4);
        BuildRequest(buildMsg);
    }else if(type == DHCP_ACK){
        memcpy(dIp, parseServerIp, 4);
        dState = Success;
    }else{
        dState = GoOn;
    }
}

DHCPHelp::dhcpState DHCPHelp::getState() {
    return dState;
}

void DHCPHelp::GetIp(uint8_t *buff) {
    memcpy(buff, recvIp,4);
}

void DHCPHelp::GetMask(uint8_t *buff) {
    memcpy(buff, recvMask, 4);
}

void DHCPHelp::GetGateway(uint8_t *buff) {
    memcpy(buff, recvGateway, 4);
}

int DHCPHelp::parseMsg(const RIP_MSG &msg) {

    auto type = 0;
    auto p = &msg.OPT[4];
    auto e = &msg.OPT[312];
    auto opt_len = 0;

    while ( p < e ) {
        switch ( *p ) {
            case endOption :
                p = e;
                break;
            case padOption :
                p++;
                break;
            case dhcpMessageType :
                p++;
                p++;
                type = *p++;
                break;
            case subnetMask :
                p++;
                p++;
                recvMask[0] = *p++;
                recvMask[1] = *p++;
                recvMask[2] = *p++;
                recvMask[3] = *p++;
                break;
            case routersOnSubnet :
                p++;
                opt_len = *p++;
                recvGateway[0] = *p++;
                recvGateway[1] = *p++;
                recvGateway[2] = *p++;
                recvGateway[3] = *p++;
                p = p + (opt_len - 4);
                break;
            case dns :
                p++;
                opt_len = *p++;
                recvDns[0] = *p++;
                recvDns[1] = *p++;
                recvDns[2] = *p++;
                recvDns[3] = *p++;
                p = p + (opt_len - 4);
                break;
            case dhcpIPaddrLeaseTime :
                p++;
                opt_len = *p++;
                dhcp_lease_time  = *p++;
                dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
                dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
                dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
                break;
            case dhcpServerIdentifier :
                p++;
                opt_len = *p++;
                parseServerIp[0] = *p++;
                parseServerIp[1] = *p++;
                parseServerIp[2] = *p++;
                parseServerIp[3] = *p++;
                break;
            default :
                p++;
                opt_len = *p++;
                p += opt_len;
                break;
        }
    }

    return type;
}

void DHCPHelp::GetDestIp(uint8_t *destIp) {
    memcpy(destIp, dIp, 4);
}

void DHCPHelp::GetDestPort(uint16_t &Port) {
    Port = dPort;
}

DHCPHelp::DHCPHelp() {}

RIP_MSG DHCPHelp::BuildReRequest() {
    RIP_MSG msg;
    BuildBaseMsg(msg);

    msg.flags = ChangeLittleBigEnd(DHCP_FLAGSUNICAST);
    memcpy(msg.ciaddr, recvIp, 4);
    msg.OPT[messageTypeIndex] = DHCP_REQUEST;

    int index = nextOptPosition;

    msg.OPT[index++] = dhcpParamRequest;
    msg.OPT[index++] = 0x08;	// length of request
    msg.OPT[index++] = subnetMask;
    msg.OPT[index++] = routersOnSubnet;
    msg.OPT[index++] = dns;
    msg.OPT[index++] = domainName;
    msg.OPT[index++] = dhcpT1value;
    msg.OPT[index++] = dhcpT2value;
    msg.OPT[index++] = performRouterDiscovery;
    msg.OPT[index++] = staticRoute;
    msg.OPT[index++] = endOption;

    return msg;
}




