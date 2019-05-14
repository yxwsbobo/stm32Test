//
// Created by kin on 19-5-7.
//

#ifndef STM32CHEAP_W5500CONFIG_H
#define STM32CHEAP_W5500CONFIG_H


#include <string>
#include "CommonStruct.h"

class W5500Config {

public:
    void loadMacAddress(uint8_t* mac);
    IpInfo loadIpInfo();
    void saveIpInfo(IpInfo& ipInfo);
    bool isManual();


};


#endif //STM32CHEAP_W5500CONFIG_H
