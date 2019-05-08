//
// Created by kin on 19-5-7.
//

#ifndef STM32CHEAP_GPIOGUARD_H
#define STM32CHEAP_GPIOGUARD_H

#include <main.h>
#include "WOLInterface.h"

class GPIOGuard {
public:

    explicit GPIOGuard(const GPIOInfo &info);

    virtual ~GPIOGuard();

private:
    GPIOInfo info;
};


#endif //STM32CHEAP_GPIOGUARD_H
