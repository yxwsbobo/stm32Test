//
// Created by kin on 19-4-30.
//

#include <spi.h>
#include "WOLInterface.h"
#include "WOLManager.h"
#ifdef __cplusplus
extern "C" {
#endif

int RunCode(){
    W5500DeviceInfo w5500DeviceInfo{};

    w5500DeviceInfo.Reset.Port = WNet_RST_GPIO_Port;
    w5500DeviceInfo.Reset.Pin = WNet_RST_Pin;
    w5500DeviceInfo.SCS.Port = WNet_SCS_GPIO_Port;
    w5500DeviceInfo.SCS.Pin = WNet_SCS_Pin;
    w5500DeviceInfo.hspi = &hspi1;


    LEDDeviceInfo ledDeviceInfo{};
    ledDeviceInfo.Led.Port = LED_GPIO_Port;
    ledDeviceInfo.Led.Pin = LED_Pin;

    WOLManager wolManager{w5500DeviceInfo, ledDeviceInfo};

    return wolManager.Run();
}

#ifdef __cplusplus
}

uint16_t ChangeLittleBigEnd(uint16_t value) {
    return ((value & 0x00ff)<<8) | ((value & 0xff00)>>8);
}


#endif