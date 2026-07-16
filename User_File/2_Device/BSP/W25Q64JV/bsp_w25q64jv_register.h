/**
 * @file bsp_w25q64jv_register.h
 * @author WangFonzhuo
 * @brief W25Q64JV当前使用到的指令
 * @version 1.0
 * @date 2026-07-14 27赛季
 */

#ifndef BSP_W25Q64JV_REGISTER_H
#define BSP_W25Q64JV_REGISTER_H

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief W25Q64JV指令
 */
enum Enum_W25Q64JV_Command : uint8_t
{
    W25Q64JV_Command_READ_STATUS_REGISTER_1  = 0x05,
    W25Q64JV_Command_READ_STATUS_REGISTER_2  = 0x35,
    W25Q64JV_Command_WRITE_STATUS_REGISTER_2 = 0x31,
    W25Q64JV_Command_FAST_READ_QUAD_IO       = 0xEB,
    W25Q64JV_Command_QUAD_INPUT_PAGE_PROGRAM = 0x32,
    W25Q64JV_Command_SECTOR_ERASE            = 0x20,
    W25Q64JV_Command_WRITE_ENABLE            = 0x06,
    W25Q64JV_Command_JEDEC_ID                = 0x9F,
};

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

#endif /* BSP_W25Q64JV_REGISTER_H */

/*----------------------------------------------------------------------------*/