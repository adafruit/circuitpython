#define MICROPY_HW_BOARD_NAME "Framework Laptop 16 Keyboard (CircuitPython)"
#define MICROPY_HW_MCU_NAME "rp2040"

//#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

// External flash W25Q80DV
#define EXTERNAL_FLASH_QSPI_DUAL
#if QSPI_FLASH_FILESYSTEM
//#define MICROPY_QSPI_DATA0                NRF_GPIO_PIN_MAP(1, 00)
//#define MICROPY_QSPI_DATA1                NRF_GPIO_PIN_MAP(0, 21)
//#define MICROPY_QSPI_DATA2                NRF_GPIO_PIN_MAP(0, 22)
//#define MICROPY_QSPI_DATA3                NRF_GPIO_PIN_MAP(0, 23)
//#define MICROPY_QSPI_SCK                  NRF_GPIO_PIN_MAP(0, 19)
//#define MICROPY_QSPI_CS                   NRF_GPIO_PIN_MAP(0, 20)
#endif

#define CIRCUITPY_DRIVE_LABEL "CIRCUITPY"
#define CIRCUITPY_BOOT_COUNTER 1
#define FLASH_SIZE                  (0x100000)
#define FLASH_PAGE_SIZE             (0x4000)

#define BOARD_HAS_CRYSTAL 1

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO27, .sda = &pin_GPIO26}}
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO27)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO26)
// pin_GPIO0 sleep
// pin_GPIO28 INTB
// pin_GPIO29 SDB
