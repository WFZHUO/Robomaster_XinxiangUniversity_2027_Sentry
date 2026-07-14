/**
 * @file bsp_w25q64jv.cpp
 * @author WangFonzhuo
 * @brief W25Q64JV串行Flash的配置与操作
 * @version 1.0
 * @date 2026-07-14 27赛季
 * @note 当前版本仅完成人工上板功能测试并通过, 尚未进行人工逐行代码审查.
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp_w25q64jv.h"

#include <string.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 全局W25Q64JV对象
Class_W25Q64JV BSP_W25Q64JV;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 异步初始化W25Q64JV, 自动读取JEDEC ID并检查或开启QE
 *
 * @note 返回HAL_OK仅表示初始化流程成功启动, 不表示初始化已经完成.
 * @param __hospi 绑定的OSPI
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Init(OSPI_HandleTypeDef *__hospi)
{
    if (__hospi == nullptr)
    {
        return HAL_ERROR;
    }

    if (Get_Busy_Flag())
    {
        return HAL_BUSY;
    }

    hospi = __hospi;
    Init_Finished_Flag = false;
    JEDEC_ID = 0U;
    Status_Register_2 = 0U;
    Rx_Buffer = nullptr;
    Rx_Length = 0U;

    Operation = Operation_INIT;

    uint8_t status = Start_Read_ID();

    if (status != HAL_OK)
    {
        Fail_Operation();
    }

    return status;
}

/**
 * @brief 异步四线读取数据
 *
 * @note 使用0xEB Fast Read Quad I/O, Mode Byte单独配置为8bit, 后续Dummy Cycle为4个时钟周期.
 * @note 返回HAL_OK仅表示读取成功启动, 完成后通过Get_Busy_Flag()和接收Getter获取数据.
 * @param __Address 读取起始地址
 * @param __Length 读取长度, Byte
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Get_Buffer(uint32_t __Address,
                                    uint32_t __Length)
{
    if ((hospi == nullptr) || (!Init_Finished_Flag))
    {
        return HAL_ERROR;
    }

    if (Get_Busy_Flag())
    {
        return HAL_BUSY;
    }

    if ((__Length == 0U) ||
        (__Length > OSPI_BUFFER_SIZE) ||
        (__Address >= W25Q64JV_SIZE) ||
        (__Length > (W25Q64JV_SIZE - __Address)))
    {
        return HAL_ERROR;
    }

    Operation = Operation_READ;
    Current_Instruction = W25Q64JV_Command_FAST_READ_QUAD_IO;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.Address = __Address;
    command.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
    command.AlternateBytes = 0x00U;
    command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
    command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
    command.DataMode = HAL_OSPI_DATA_4_LINES;
    command.DummyCycles = 4U;

    uint8_t status = OSPI_Command_Receive_Data(hospi,
                                               &command,
                                               __Length);

    if (status != HAL_OK)
    {
        Fail_Operation();
    }

    return status;
}

/**
 * @brief 异步四线页编程
 *
 * @note 内部自动执行写使能、WEL轮询、页编程和BUSY轮询.
 * @note 单次写入不能超过256B且不能跨页.
 * @param __Buffer 待写入数据
 * @param __Address 写入起始地址
 * @param __Length 写入长度, Byte
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Set_Buffer(const uint8_t *__Buffer,
                                    uint32_t __Address,
                                    uint32_t __Length)
{
    if ((hospi == nullptr) || (!Init_Finished_Flag))
    {
        return HAL_ERROR;
    }

    if (Get_Busy_Flag())
    {
        return HAL_BUSY;
    }

    if ((__Buffer == nullptr) ||
        (__Length == 0U) ||
        (__Length > W25Q64JV_PAGE_SIZE) ||
        (__Address >= W25Q64JV_SIZE) ||
        (__Length > (W25Q64JV_SIZE - __Address)) ||
        (((__Address % W25Q64JV_PAGE_SIZE) + __Length) > W25Q64JV_PAGE_SIZE))
    {
        return HAL_ERROR;
    }

    // 先保存用户数据, 防止异步流程执行期间原缓冲区被修改
    memcpy(Tx_Buffer, __Buffer, __Length);

    Pending_Address = __Address;
    Pending_Length = __Length;
    Operation = Operation_PAGE_PROGRAM;

    uint8_t status = Start_Write_Enable();

    if (status != HAL_OK)
    {
        Fail_Operation();
    }

    return status;
}

/**
 * @brief 异步擦除4KB扇区
 *
 * @note 内部自动执行写使能、WEL轮询、扇区擦除和BUSY轮询.
 * @param __Address 扇区起始地址, 必须按照4KB对齐
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Set_Sector_Erased(uint32_t __Address)
{
    if ((hospi == nullptr) || (!Init_Finished_Flag))
    {
        return HAL_ERROR;
    }

    if (Get_Busy_Flag())
    {
        return HAL_BUSY;
    }

    if ((__Address >= W25Q64JV_SIZE) ||
        ((__Address % W25Q64JV_SECTOR_SIZE) != 0U))
    {
        return HAL_ERROR;
    }

    Pending_Address = __Address;
    Operation = Operation_SECTOR_ERASE;

    uint8_t status = Start_Write_Enable();

    if (status != HAL_OK)
    {
        Fail_Operation();
    }

    return status;
}

/**
 * @brief OSPI底层完成或错误回调, 直接推进W25Q64JV异步操作
 *
 * @note drv_ospi在调用本回调前已清除本次OSPI传输状态, 因此可以直接启动下一步异步操作.
 * @param __Operation OSPI完成或发生错误的操作类型
 * @param __Buffer OSPI接收或发送缓冲区
 * @param __Length 有效数据长度
 * @param __Error_Code HAL OSPI错误码
 */
void Class_W25Q64JV::OSPI_Callback(enum Enum_OSPI_Operation __Operation,
                                    uint8_t *__Buffer,
                                    uint32_t __Length,
                                    uint32_t __Error_Code)
{
    if (Operation == Operation_NONE)
    {
        return;
    }

    if (__Error_Code != HAL_OSPI_ERROR_NONE)
    {
        Fail_Operation();
        return;
    }

    uint8_t status = HAL_ERROR;

    if (__Operation == OSPI_OPERATION_RECEIVE)
    {
        Rx_Buffer = __Buffer;
        Rx_Length = __Length;

        // 初始化第一步, 读取并检查JEDEC ID
        if ((Operation == Operation_INIT) &&
            (Current_Instruction == W25Q64JV_Command_JEDEC_ID))
        {
            if ((__Buffer == nullptr) || (__Length < 3U))
            {
                Fail_Operation();
                return;
            }

            JEDEC_ID = (static_cast<uint32_t>(__Buffer[0]) << 16U) |
                       (static_cast<uint32_t>(__Buffer[1]) << 8U) |
                       static_cast<uint32_t>(__Buffer[2]);

            if (JEDEC_ID != W25Q64JV_JEDEC_ID)
            {
                Fail_Operation();
                return;
            }

            status = Start_Read_Status_Register_2();
        }
        // 初始化第二步, 检查QE是否已经开启
        else if ((Operation == Operation_INIT) &&
                 (Current_Instruction == W25Q64JV_Command_READ_STATUS_REGISTER_2))
        {
            if ((__Buffer == nullptr) || (__Length == 0U))
            {
                Fail_Operation();
                return;
            }

            Status_Register_2 = __Buffer[0];

            if ((Status_Register_2 & W25Q64JV_STATUS_2_QE) != 0U)
            {
                Init_Finished_Flag = true;
                Finish_Operation();
                return;
            }

            status = Start_Write_Enable();
        }
        // 普通四线读取完成
        else if ((Operation == Operation_READ) &&
                 (Current_Instruction == W25Q64JV_Command_FAST_READ_QUAD_IO))
        {
            Finish_Operation();
            return;
        }
    }
    else if (__Operation == OSPI_OPERATION_COMMAND)
    {
        // 写使能指令完成后轮询WEL, 确认写使能锁存器已经置位
        if (Current_Instruction == W25Q64JV_Command_WRITE_ENABLE)
        {
            status = Start_Auto_Polling(W25Q64JV_STATUS_1_WEL,
                                        W25Q64JV_STATUS_1_WEL);
        }
        // 扇区擦除指令发送完成后轮询BUSY, 等待Flash内部擦除完成
        else if (Current_Instruction == W25Q64JV_Command_SECTOR_ERASE)
        {
            status = Start_Auto_Polling(0U,
                                        W25Q64JV_STATUS_1_BUSY);
        }
    }
    else if (__Operation == OSPI_OPERATION_TRANSMIT)
    {
        // 数据发送结束只表示数据已经送入Flash, 仍需轮询BUSY等待内部编程完成
        if ((Current_Instruction == W25Q64JV_Command_WRITE_STATUS_REGISTER_2) ||
            (Current_Instruction == W25Q64JV_Command_QUAD_INPUT_PAGE_PROGRAM))
        {
            status = Start_Auto_Polling(0U,
                                        W25Q64JV_STATUS_1_BUSY);
        }
    }
    else if (__Operation == OSPI_OPERATION_AUTO_POLLING)
    {
        // WEL置位后根据完整操作启动真正的写入或擦除指令
        if (Current_Instruction == W25Q64JV_Command_WRITE_ENABLE)
        {
            if (Operation == Operation_INIT)
            {
                status = Start_Write_Status_Register_2();
            }
            else if (Operation == Operation_PAGE_PROGRAM)
            {
                status = Start_Page_Program();
            }
            else if (Operation == Operation_SECTOR_ERASE)
            {
                status = Start_Sector_Erase();
            }
        }
        // QE写入完成
        else if (Current_Instruction == W25Q64JV_Command_WRITE_STATUS_REGISTER_2)
        {
            Init_Finished_Flag = true;
            Finish_Operation();
            return;
        }
        // 页编程或扇区擦除的BUSY已经清零
        else if ((Current_Instruction == W25Q64JV_Command_QUAD_INPUT_PAGE_PROGRAM) ||
                 (Current_Instruction == W25Q64JV_Command_SECTOR_ERASE))
        {
            Finish_Operation();
            return;
        }
    }

    if (status != HAL_OK)
    {
        Fail_Operation();
    }
}

/**
 * @brief 构造基础OSPI指令配置
 *
 * @param __Instruction W25Q64JV指令
 * @return OSPI_RegularCmdTypeDef 基础OSPI指令配置
 */
OSPI_RegularCmdTypeDef Class_W25Q64JV::Build_Command(enum Enum_W25Q64JV_Command __Instruction) const
{
    OSPI_RegularCmdTypeDef command = {};

    command.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    command.FlashId = HAL_OSPI_FLASH_ID_1;
    command.Instruction = static_cast<uint32_t>(__Instruction);
    command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    command.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
    command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    command.AddressMode = HAL_OSPI_ADDRESS_NONE;
    command.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
    command.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
    command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    command.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
    command.DataMode = HAL_OSPI_DATA_NONE;
    command.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    command.DummyCycles = 0U;
    command.DQSMode = HAL_OSPI_DQS_DISABLE;
    command.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

    return command;
}

/**
 * @brief 启动JEDEC ID异步读取
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Read_ID()
{
    Current_Instruction = W25Q64JV_Command_JEDEC_ID;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.DataMode = HAL_OSPI_DATA_1_LINE;

    return OSPI_Command_Receive_Data(hospi, &command, 3U);
}

/**
 * @brief 启动状态寄存器2异步读取
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Read_Status_Register_2()
{
    Current_Instruction = W25Q64JV_Command_READ_STATUS_REGISTER_2;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.DataMode = HAL_OSPI_DATA_1_LINE;

    return OSPI_Command_Receive_Data(hospi, &command, 1U);
}

/**
 * @brief 启动写使能指令
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Write_Enable()
{
    Current_Instruction = W25Q64JV_Command_WRITE_ENABLE;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);

    return OSPI_Command(hospi, &command);
}

/**
 * @brief 启动状态寄存器2异步写入, 开启QE四线模式
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Write_Status_Register_2()
{
    Current_Instruction = W25Q64JV_Command_WRITE_STATUS_REGISTER_2;
    Status_Register_2 |= W25Q64JV_STATUS_2_QE;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.DataMode = HAL_OSPI_DATA_1_LINE;

    return OSPI_Command_Transmit_Data(hospi,
                                      &command,
                                      &Status_Register_2,
                                      1U);
}

/**
 * @brief 启动四线页编程
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Page_Program()
{
    Current_Instruction = W25Q64JV_Command_QUAD_INPUT_PAGE_PROGRAM;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.Address = Pending_Address;
    command.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    command.DataMode = HAL_OSPI_DATA_4_LINES;

    return OSPI_Command_Transmit_Data(hospi,
                                      &command,
                                      Tx_Buffer,
                                      Pending_Length);
}

/**
 * @brief 启动4KB扇区擦除
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Sector_Erase()
{
    Current_Instruction = W25Q64JV_Command_SECTOR_ERASE;

    OSPI_RegularCmdTypeDef command = Build_Command(Current_Instruction);
    command.Address = Pending_Address;
    command.AddressMode = HAL_OSPI_ADDRESS_1_LINE;

    return OSPI_Command(hospi, &command);
}

/**
 * @brief 启动状态寄存器1硬件自动轮询
 *
 * @note Current_Instruction保留上一条Flash指令, 用于轮询完成后判断下一步操作.
 * @note 当前未配置软件超时, 状态始终不匹配时OSPI和本库可能长期保持Busy.
 * @param __Match 状态匹配值
 * @param __Mask 状态匹配掩码
 * @return uint8_t HAL执行状态
 */
uint8_t Class_W25Q64JV::Start_Auto_Polling(uint32_t __Match,
                                            uint32_t __Mask)
{
    OSPI_RegularCmdTypeDef command = Build_Command(W25Q64JV_Command_READ_STATUS_REGISTER_1);
    command.DataMode = HAL_OSPI_DATA_1_LINE;
    command.NbData = 1U;

    OSPI_AutoPollingTypeDef config = {};
    config.Match = __Match;
    config.Mask = __Mask;
    config.MatchMode = HAL_OSPI_MATCH_MODE_AND;
    config.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;
    config.Interval = 5U;

    return OSPI_Auto_Polling(hospi, &command, &config);
}

/**
 * @brief 完成当前W25Q64JV操作并恢复空闲状态
 */
void Class_W25Q64JV::Finish_Operation()
{
    Operation = Operation_NONE;
    Current_Instruction = W25Q64JV_Command_JEDEC_ID;
    Pending_Address = 0U;
    Pending_Length = 0U;
}

/**
 * @brief 终止当前W25Q64JV操作并恢复空闲状态
 */
void Class_W25Q64JV::Fail_Operation()
{
    if (Operation == Operation_INIT)
    {
        Init_Finished_Flag = false;
    }

    Finish_Operation();
}

/*----------------------------------------------------------------------------*/
