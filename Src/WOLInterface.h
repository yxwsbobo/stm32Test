//
// Created by kin on 19-4-30.
//

#ifndef STM32CHEAP_WOLINTERFACE_H
#define STM32CHEAP_WOLINTERFACE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "gpio.h"

    extern int GlobalFlag;

    int RunCode();

    uint16_t ChangeLittleBigEnd(uint16_t value);
    uint32_t ChangeLittleBigEnd32(uint32_t value);


typedef struct GPIOInfo_{
        GPIO_TypeDef* Port;
        uint16_t Pin;
    }GPIOInfo;

    struct W5500DeviceInfo{
        GPIOInfo Reset;

        GPIOInfo SCS;

        SPI_HandleTypeDef* hspi;

    };

    struct LEDDeviceInfo{
        GPIOInfo Led;
    };

#ifdef __cplusplus
}
#endif
#endif //STM32CHEAP_WOLINTERFACE_H
