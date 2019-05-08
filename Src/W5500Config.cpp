//
// Created by kin on 19-5-7.
//

#include "W5500Config.h"

std::string W5500Config::loadMacAddress() {
    return "50-9A-2C-34-38-CF";
}

IpInfo W5500Config::loadIpInfo() {
    IpInfo result;
    result.gateWay = "192.30.1.1";
    result.Ip = "192.30.1.72";
    result.mask = "255.255.255.0";

    return result;
}

bool W5500Config::isManual() {
    return true;
}

