//
// Created by kin on 19-5-14.
//

#ifndef STM32CHEAP_CONFIGUSEFLASH_H
#define STM32CHEAP_CONFIGUSEFLASH_H


#include <cstdint>


struct ConfigUseFlash {
public:
    void SaveConfig();
    void LoadConfig();

public:
    uint8_t Mac[8] {};

    uint8_t Ip[4]{};
    uint32_t Port{};
    uint8_t Gateway[4]{};
    uint8_t Mask[4]{};

    uint8_t ServerIp[4]{};
    uint32_t ServerPort{};

private:
    static constexpr int32_t ConfigFlashAddress = 0x800F000;

    void WriteData();

    void InitAndSaveConfig();
};

extern ConfigUseFlash configFlash;

#endif //STM32CHEAP_CONFIGUSEFLASH_H
