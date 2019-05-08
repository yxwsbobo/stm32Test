//
// Created by kin on 19-4-30.
//

#ifndef STM32CHEAP_WOLMANAGER_H
#define STM32CHEAP_WOLMANAGER_H
#include "WOLInterface.h"
#include "W5500Manager.h"
#include <memory>


class WOLManager {
public:
    WOLManager(const W5500DeviceInfo& w5500DeviceInfo, const LEDDeviceInfo& ledDeviceInfo);

    int Run();

private:
    void Test(int speed);


private:
    int Running = 1;
    std::shared_ptr<W5500Manager> w5500Manager;
    LEDDeviceInfo ledDeviceInfo;
};


#endif //STM32CHEAP_WOLMANAGER_H
