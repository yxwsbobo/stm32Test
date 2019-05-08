//
// Created by kin on 19-4-30.
//

#include "WOLManager.h"

#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

WOLManager::WOLManager(const W5500DeviceInfo &w5500DeviceInfo, const LEDDeviceInfo &ledDeviceInfo)
        : w5500Manager{std::make_shared<W5500Manager>(w5500DeviceInfo, std::unique_ptr<W5500Config>(new W5500Config))},
        ledDeviceInfo{ledDeviceInfo}
{
    w5500Manager->reStart();
}

void WOLManager::Test(int speed) {
    HAL_Delay(speed);
    HAL_GPIO_TogglePin(ledDeviceInfo.Led.Port, ledDeviceInfo.Led.Pin);
}

int WOLManager::Run() {

    auto testSocket = w5500Manager->newSocket("192.30.1.244", 6000);

    while(Running)
    {
        if(testSocket.getState() == W5500Socket::CONNECTED)
        {
            Test(3000);
            testSocket.Send("Hello world");
        }
        else{
            Test(200);
        }
    }
    return 0;
}

