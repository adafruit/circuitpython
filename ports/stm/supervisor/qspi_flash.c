// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/spi_flash_api.h"

#include <stdint.h>
#include <string.h>

#include "py/mpconfig.h" // for EXTERNAL_FLASH_QSPI_DUAL

#include "supervisor/shared/external_flash/common_commands.h"
#include "supervisor/shared/external_flash/qspi_flash.h"

OSPI_HandleTypeDef hospi1;

#define READ_STATUS_REG_CMD 0x05
#define WRITE_ENABLE_CMD 0x06
#define FAST_READ 0x0B
#define QUAD_IN_FAST_PROG_CMD 0x32
#define QUAD_IN_OUT_FAST_READ_CMD 0xEB

#define RESET_ENABLE_CMD 0x66
#define RESET_MEMORY_CMD 0x99

static HAL_StatusTypeDef OSPI_Init(void);

HAL_StatusTypeDef OSPI_Init() {
    OSPIM_CfgTypeDef sOspiManagerCfg = { 0 };
    HAL_StatusTypeDef ret;

    hospi1.Instance = OCTOSPI1;
    HAL_OSPI_DeInit(&hospi1);

    hospi1.Init.FifoThreshold = 1;
    hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
    hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
    hospi1.Init.DeviceSize = 23;
    hospi1.Init.ChipSelectHighTime = 6;
    hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
    hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
    hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
    hospi1.Init.ClockPrescaler = 1;
    hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
    hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
    hospi1.Init.ChipSelectBoundary = 0;
    hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
    hospi1.Init.MaxTran = 0;
    hospi1.Init.Refresh = 0;

    if ((ret = HAL_OSPI_Init(&hospi1)) != HAL_OK) {
        return ret;
    }

    sOspiManagerCfg.ClkPort = 1;
    sOspiManagerCfg.NCSPort = 1;
    sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_NONE;
    sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
    return HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
}

bool spi_flash_command(uint8_t command) {

    OSPI_RegularCmdTypeDef sCommand = { 0 };

    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction = command;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

    return HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

bool spi_flash_read_command(uint8_t command, uint8_t *response, uint32_t length) {

    OSPI_RegularCmdTypeDef ospi_cmd = { 0 };

    ospi_cmd.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    ospi_cmd.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    ospi_cmd.Instruction = command;
    ospi_cmd.AddressMode = HAL_OSPI_ADDRESS_NONE;
    ospi_cmd.AddressSize = 0x00;
    ospi_cmd.Address = 0x00;
    ospi_cmd.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    ospi_cmd.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
    ospi_cmd.AlternateBytes = 0xFF;
    ospi_cmd.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE;
    ospi_cmd.DataMode = HAL_OSPI_DATA_1_LINE;
    ospi_cmd.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    ospi_cmd.DummyCycles = 0;
    ospi_cmd.NbData = length;
    ospi_cmd.DQSMode = HAL_OSPI_DQS_DISABLE;
    ospi_cmd.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

    if (HAL_OSPI_Command(&hospi1, &ospi_cmd, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)
        != HAL_OK) {
        return false;
    }

    return HAL_OSPI_Receive(&hospi1, response, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

bool spi_flash_write_command(uint8_t command, uint8_t *data, uint32_t length) {
    return false;
}

bool spi_flash_sector_command(uint8_t command, uint32_t address) {
    if (command != CMD_SECTOR_ERASE) {
        return false;
    }

    OSPI_RegularCmdTypeDef sCommand = { 0 };

    /* Erasing Sequence --------------------------------- */
    sCommand.Instruction = command;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.Address = address;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DataMode = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles = 0;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

    return HAL_OSPI_Command(&hospi1, &sCommand, HAL_MAX_DELAY) == HAL_OK;
}

bool spi_flash_write_data(uint32_t address, uint8_t *data, uint32_t length) {

    OSPI_RegularCmdTypeDef sCommand = { 0 };

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;
    sCommand.NbData = length;
    sCommand.Address = address;
    sCommand.DummyCycles = 0;

    if (HAL_OSPI_Command(&hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return false;
    }

    return HAL_OSPI_Transmit(&hospi1, data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

bool spi_flash_read_data(uint32_t address, uint8_t *data, uint32_t length) {

    OSPI_RegularCmdTypeDef s_command = { 0 };

    s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
    s_command.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
    s_command.Instruction = QUAD_IN_OUT_FAST_READ_CMD;
    s_command.AddressMode = HAL_OSPI_ADDRESS_4_LINES;
    s_command.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
    s_command.Address = address;
    s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
    s_command.AlternateBytesSize = HAL_OSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes = 0xFF;
    s_command.DataMode = HAL_OSPI_DATA_4_LINES;
    s_command.DummyCycles = 4;
    s_command.NbData = length;
    s_command.DQSMode = HAL_OSPI_DQS_DISABLE;
    s_command.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

    if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return false;
    }

    return HAL_OSPI_Receive(&hospi1, data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK;
}

void spi_flash_init(void) {
    // Init QSPI flash
    OSPI_Init();
}

void spi_flash_init_device(const external_flash_device *device) {
}
