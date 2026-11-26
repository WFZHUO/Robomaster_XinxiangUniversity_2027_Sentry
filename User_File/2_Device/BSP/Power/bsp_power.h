/**
 * @file bsp_power.h
 * @author WangFangzhuo
 * @brief 板载可控电源开关
 * @version 1.2
 * @date 2026-04-24
 */

#ifndef BSP_POWER_H
#define BSP_POWER_H

/* Includes ------------------------------------------------------------------*/

#include "stm32h7xx_hal.h"

/* Exported types ------------------------------------------------------------*/

class Class_Power
{
public:
    void Init(bool vcc_out_1 = false, bool vcc_out_2 = false, bool dc5 = false);

    void Set_VCC_OUT_1(bool enable);
    void Set_VCC_OUT_2(bool enable);
    void Set_DC5(bool enable);
    void Disable_All_Output();

    bool Get_VCC_OUT_1_Status() const { return vcc_out_1_status_; }
    bool Get_VCC_OUT_2_Status() const { return vcc_out_2_status_; }
    bool Get_DC5_Status() const { return dc5_status_; }

private:
    bool vcc_out_1_status_ = false;
    bool vcc_out_2_status_ = false;
    bool dc5_status_ = false;
};

/* Exported variables --------------------------------------------------------*/

extern Class_Power BSP_Power;

#endif // BSP_POWER_H
