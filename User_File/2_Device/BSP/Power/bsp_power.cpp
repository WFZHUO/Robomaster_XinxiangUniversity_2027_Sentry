/**
 * @file bsp_power.cpp
 * @author WangFangzhuo
 * @brief 板载可控电源开关
 * @version 1.2
 * @date 2026-04-24
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp_power.h"

/* Variables -----------------------------------------------------------------*/

Class_Power BSP_Power;

/* Function definitions ------------------------------------------------------*/

void Class_Power::Init(bool vcc_out_1, bool vcc_out_2, bool dc5)
{
    Set_VCC_OUT_1(vcc_out_1);
    Set_VCC_OUT_2(vcc_out_2);
    Set_DC5(dc5);
}

void Class_Power::Set_VCC_OUT_1(bool enable)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
    vcc_out_1_status_ = enable;
}

void Class_Power::Set_VCC_OUT_2(bool enable)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
    vcc_out_2_status_ = enable;
}

void Class_Power::Set_DC5(bool enable)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
    dc5_status_ = enable;
}

void Class_Power::Disable_All_Output()
{
    Set_VCC_OUT_1(false);
    Set_VCC_OUT_2(false);
    Set_DC5(false);
}
