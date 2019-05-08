//
// Created by kin on 19-5-7.
//

#ifndef STM32CHEAP_W5500CONFIG_H
#define STM32CHEAP_W5500CONFIG_H


#include <string>
#include "CommonStruct.h"

class W5500Config {

public:
    std::string loadMacAddress();
    IpInfo loadIpInfo();
    bool isManual();

};


#endif //STM32CHEAP_W5500CONFIG_H
