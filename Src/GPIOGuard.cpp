//
// Created by kin on 19-5-7.
//

#include "GPIOGuard.h"

GPIOGuard::GPIOGuard(const GPIOInfo &info) : info(info) {
    HAL_GPIO_WritePin(info.Port, info.Pin, GPIO_PIN_RESET);

}

GPIOGuard::~GPIOGuard() {
    HAL_GPIO_WritePin(info.Port, info.Pin, GPIO_PIN_SET);
}