//
// Created by kin on 19-5-14.
//

#include "ConfigUseFlash.h"
#include "main.h"
#include "string.h"

ConfigUseFlash configFlash;

void ConfigUseFlash::SaveConfig() {
    auto state = HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = ConfigFlashAddress & (~0xFFF);
    f.NbPages = 1;
    f.Banks = 1;
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&f, &PageError);

    if(PageError == -1) {
        WriteData();
    }

    HAL_FLASH_Lock();
}

void ConfigUseFlash::LoadConfig() {
    auto add = (uint8_t*)ConfigFlashAddress;
    if(*(uint32_t*)add != 0X4B696E){
        InitAndSaveConfig();
    }else{
        add+= sizeof(uint32_t);
        for (int i = 0; i < 6; ++i) {
            Mac[i] = add[i];
        }
        add+= sizeof(uint64_t);
        memcpy(Ip, add, sizeof(uint32_t));
        add+= sizeof(uint32_t);
        memcpy(ServerIp, add, sizeof(uint32_t));
        add+= sizeof(uint32_t);
        memcpy(Gateway, add, sizeof(uint32_t));
        add+= sizeof(uint32_t);
        memcpy(Mask, add, sizeof(uint32_t));
        add+= sizeof(uint32_t);
        memcpy(&Port, add, sizeof(Port));
        add+= sizeof(Port);
        memcpy(&ServerPort, add, sizeof(ServerPort));
    }
}

void ConfigUseFlash::WriteData() {
    auto add = ConfigFlashAddress;

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,0X4B696E);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add,*(uint32_t *)Mac);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add,*(uint32_t *)(Mac+4));
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,*(uint32_t*)Ip);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,*(uint32_t*)ServerIp);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,*(uint32_t*)Gateway);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,*(uint32_t*)Mask);
    add+= sizeof(uint32_t);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,Port);
    add+= sizeof(Port);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,add,ServerPort);

}

void ConfigUseFlash::InitAndSaveConfig() {
    Mac[0] = 0x50;
    Mac[1] = 0x9A;
    Mac[2] = 0x2C;
    Mac[3] = 0x34;
    Mac[4] = 0x38;
    Mac[5] = 0xCF;

    Ip[0] = 192;
    Ip[1] = 30;
    Ip[2] = 1;
    Ip[3] = 72;

    Gateway[0] = 192;
    Gateway[1] = 30;
    Gateway[2] = 1;
    Gateway[3] = 1;

    Port = 5000;
    ServerPort = 5000;
    ServerIp[0] = 192;
    ServerIp[1] = 30;
    ServerIp[2] = 1;
    ServerIp[3] = 54;

    Mask[0] = Mask[1] = Mask[2] = 255;
    Mask[3] = 0;

    SaveConfig();
}
