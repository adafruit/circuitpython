from adafruit_bus_device import i2c_device

import time

REG_LEFT_INPUT_VOLUME    = 0x00
REG_RIGHT_INPUT_VOLUME   = 0x01
REG_LOUT1_VOLUME         = 0x02
REG_ROUT1_VOLUME         = 0x03
REG_CLOCKING_1           = 0x04
REG_ADC_DAC_CTRL_1       = 0x05
REG_ADC_DAC_CTRL_2       = 0x06
REG_AUDIO_INTERFACE_1    = 0x07
REG_CLOCKING_2           = 0x08
REG_AUDIO_INTERFACE_2    = 0x09
REG_LEFT_DAC_VOLUME      = 0x0A
REG_RIGHT_DAC_VOLUME     = 0x0B
REG_RESET                = 0x0F
REG_3D_CONTROL           = 0x10
REG_ALC1                 = 0x11
REG_ALC2                 = 0x12
REG_ALC3                 = 0x13
REG_NOISE_GATE           = 0x14
REG_LEFT_ADC_VOLUME      = 0x15
REG_RIGHT_ADC_VOLUME     = 0x16
REG_ADDITIONAL_CONTROL_1 = 0x17
REG_ADDITIONAL_CONTROL_2 = 0x18
REG_PWR_MGMT_1           = 0x19
REG_PWR_MGMT_2           = 0x1A
REG_ADDITIONAL_CONTROL_3 = 0x1B
REG_ANTI_POP_1           = 0x1C
REG_ANTI_POP_2           = 0x1D
REG_ADCL_SIGNAL_PATH     = 0x20
REG_ADCR_SIGNAL_PATH     = 0x21
REG_LEFT_OUT_MIX_1       = 0x22
REG_RIGHT_OUT_MIX_2      = 0x25
REG_MONO_OUT_MIX_1       = 0x26
REG_MONO_OUT_MIX_2       = 0x27
REG_LOUT2_VOLUME         = 0x28
REG_ROUT2_VOLUME         = 0x29
REG_MONO_OUT_VOLUME      = 0x2A
REG_INPUT_BOOST_MIXER_1  = 0x2B
REG_INPUT_BOOST_MIXER_2  = 0x2C
REG_BYPASS_1             = 0x2D
REG_BYPASS_2             = 0x2E
REG_PWR_MGMT_3           = 0x2F
REG_ADDITIONAL_CONTROL_4 = 0x30
REG_CLASS_D_CONTROL_1    = 0x31
REG_CLASS_D_CONTROL_3    = 0x33
REG_PLL_N                = 0x34
REG_PLL_K_1              = 0x35
REG_PLL_K_2              = 0x36
REG_PLL_K_3              = 0x37



class WM8960:
    def __init__(self, i2c_bus, address=0x1a):
        self.i2c_device = i2c_device.I2CDevice(i2c_bus, address)
        self._buf = bytearray(2)

	# The WM8960 is read-only, it's not possible to read the registers from
	# the I2C interface. We therefore keep a local copy of the registers that
	# we update when writing to the device.
        self._registers_copy = [0x097, # R0 (0x00)
                                0x097, # R1 (0x01)
                                0x000, # R2 (0x02)
                                0x000, # R3 (0x03)
                                0x000, # R4 (0x04)
                                0x008, # F5 (0x05)
                                0x000, # R6 (0x06)
                                0x00A, # R7 (0x07)
                                0x1C0, # R8 (0x08)
                                0x000, # R9 (0x09)
                                0x0FF, # R10 (0x0a)
                                0x0FF, # R11 (0x0b)
                                0x000, # R12 (0x0C) RESERVED
                                0x000, # R13 (0x0D) RESERVED
                                0x000, # R14 (0x0E) RESERVED
                                0x000, # R15 (0x0F) RESERVED
                                0x000, # R16 (0x10)
                                0x07B, # R17 (0x11)
                                0x100, # R18 (0x12)
                                0x032, # R19 (0x13)
                                0x000, # R20 (0x14)
                                0x0C3, # R21 (0x15)
                                0x0C3, # R22 (0x16)
                                0x1C0, # R23 (0x17)
                                0x000, # R24 (0x18)
                                0x000, # R25 (0x19)
                                0x000, # R26 (0x1A)
                                0x000, # R27 (0x1B)
                                0x000, # R28 (0x1C)
                                0x000, # R29 (0x1D)
                                0x000, # R30 (0x1E) RESERVED
                                0x000, # R31 (0x1F) RESERVED
                                0x100, # R32 (0x20)
                                0x100, # R33 (0x21)
                                0x050, # R34 (0x22)
                                0x000, # R35 (0x23) RESERVED
                                0x000, # R36 (0x24) RESERVED
                                0x050, # R37 (0x25)
                                0x000, # R38 (0x26)
                                0x000, # R39 (0x27)
                                0x000, # R40 (0x28)
                                0x000, # R41 (0x29)
                                0x040, # R42 (0x2A)
                                0x000, # R43 (0x2B)
                                0x000, # R44 (0x2C)
                                0x050, # R45 (0x2D)
                                0x050, # R46 (0x2E)
                                0x000, # R47 (0x2F)
                                0x002, # R48 (0x30)
                                0x037, # R49 (0x31)
                                0x000, # R50 (0x32) RESERVED
                                0x080, # R51 (0x33)
                                0x008, # R52 (0x34)
                                0x031, # R53 (0x35)
                                0x026, # R54 (0x36)
                                0x0e9  # R55 (0x37)
                                ]


    def _write_register(self, reg, value):
        self._buf[0] = reg << 1 | (value & 0x100) >> 8
        self._buf[1] = value & 0xff
        with self.i2c_device as i2c:
            i2c.write(self._buf)
        self._registers_copy[reg] = value

    def _write_register_bit(self, reg, pos, value):
        current = self._registers_copy[reg]
        mask    = 1 << pos
        if value != 0:
            new = current & (~ mask)
        else:
            new = current | mask

        self._write_register(reg, new)

    def _write_register_mult(self, reg, msb, lsb, value):
        new = self._registers_copy[reg]

	# Clear bits for the range we case about
        for x in range(lsb, msb + 1):
            new = new & (~ (1 << x))

	new = current | value << lsb

        self._write_register(reg, new)

    def start_i2s_out(self):

    	self._write_register(REG_RESET, 0)
    	
    	# Enable Vref
        self._write_register_bit(REG_PWR_MGMT_1, 6, 1)

        # Enable VMID
        self._write_register_bit(REG_PWR_MGMT_1, 7, 1)
        self._write_register_bit(REG_PWR_MGMT_1, 8, 1)

        # Enable Left DAC
        self._write_register_bit(REG_PWR_MGMT_2, 8, 1)

        # Enable Right DAC
        self._write_register_bit(REG_PWR_MGMT_2, 7, 1)

        # Enable Left Output buffer
        self._write_register_bit(REG_PWR_MGMT_2, 6, 1)

        # Enable Right Output buffer
        self._write_register_bit(REG_PWR_MGMT_2, 5, 1)

        # Disable DAC Mute
        self._write_register_bit(REG_ADC_DAC_CTRL_1, 3, 0)

        # Enable Left DAC to Out mix
        self._write_register_bit(REG_LEFT_OUT_MIX_1, 8, 1)

        # Enable Righ DAC to Out mix
        self._write_register_bit(REG_RIGHT_OUT_MIX_2, 8, 1)

        # Enable Left Out mix
        self._write_register_bit(REG_PWR_MGMT_3, 3, 1)
        # Enable Righ Out mix
        self._write_register_bit(REG_PWR_MGMT_3, 2, 1)

        #  ADC CLK = SYSCLK / 256, DAC CLK = SYSCLK / 256, SYSCLK = MCLK
        self._write_register(REG_CLOCKING_1, 0x000000000);

        # 16-bit audio format
        self._write_register_bit(REG_AUDIO_INTERFACE_1, 3, 0)

        # I2C Mode
        self._write_register_bit(REG_AUDIO_INTERFACE_1, 1, 1)

        # DAC volume
        #self._write_register(REG_LEFT_DAC_VOLUME, 0x111111111)
        #self._write_register(REG_RIGHT_DAC_VOLUME, 0x111111111)

        self._write_register(REG_LEFT_DAC_VOLUME, 0x11111111)
        self._write_register(REG_RIGHT_DAC_VOLUME, 0x11111111)
        self._write_register_bit(REG_LEFT_DAC_VOLUME, 8, 1)
        self._write_register_bit(REG_RIGHT_DAC_VOLUME, 8, 1)

        # Headphone volume
        #self._write_register(REG_LOUT1_VOLUME, 0x11111111)
        #self._write_register(REG_ROUT1_VOLUME, 0x11111111)

        self._write_register(REG_LOUT1_VOLUME, 0x1011111)
        self._write_register(REG_ROUT1_VOLUME, 0x1011111)
        self._write_register_bit(REG_LOUT1_VOLUME, 8, 1)
        self._write_register_bit(REG_ROUT1_VOLUME, 8, 1)

    def start_i2s_out_org(self):
        # playback only
        self._write_register(0xf, 0) # Reset
        self._write_register(0x7, 0x2) # 16-bit I2S format
        self._write_register(0x4, 0x0) # SYSCLK = MCLK 11.2 mhz

        self._write_register(0xa, 0x1ff) # Left DAC volume
        self._write_register(0xb, 0x1ff) # Right DAC volume

        self._write_register(0x2, 0x079) # Left headphone volume
        self._write_register(0x3, 0x179) # Right headphone volume

        self._write_register(0x19, 0xc0) # 2 x 50k divider enabled and enable vref
        time.sleep(0.1)

        self._write_register(0x1a, 0x180) # DAC L + R

        self._write_register(0x22, 0x100) # Left DAC to Left Mixer
        self._write_register(0x25, 0x100) # Right DAC to Right Mixer

        self._write_register(0x2f, 0xc) # L + R Mixer output

        self._write_register(0x1a, 0x1e0) # DACL, DACR, LOUT1, ROUT1

        # Unmute DAC
        self._write_register(0x5, 0)
