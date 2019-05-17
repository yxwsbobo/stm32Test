//
// Created by kin on 19-5-7.
//

#include "W5500Config.h"
#include "ConfigUseFlash.h"
#include "string.h"

IpInfo W5500Config::loadIpInfo() {
    IpInfo result{};
    memcpy(result.Ip, configFlash.Ip,4);
    memcpy(result.gateWay, configFlash.Gateway, 4);
    memcpy(result.mask, configFlash.Mask,4);

    return result;
}

bool W5500Config::isManual() {
    return false;
}

void W5500Config::loadMacAddress(uint8_t *mac) {
    memcpy(mac, configFlash.Mac,6);
}

void W5500Config::saveIpInfo(IpInfo &ipInfo) {
    memcpy(configFlash.Ip, ipInfo.Ip,4);
    memcpy(configFlash.Gateway, ipInfo.gateWay, 4);
    memcpy(configFlash.Mask, ipInfo.mask,4);
}
