from adafruit_bus_device import i2c_device

IO_EXP_SPK_Enable_L_Mask = 0b00000001
IO_EXP_SPK_Enable_R_Mask = 0b00000010
IO_EXP_SPK_Gain_0_Mask = 0b00000100
IO_EXP_SPK_Gain_1_Mask = 0b00001000
IO_EXP_DAC_Not_Reset_Mask = 0b00010000
IO_EXP_Jack_Detect_Mask = 0b00100000

IO_EXP_INPUT_REG = 0
IO_EXP_OUTPUT_REG = 1
IO_EXP_CONFIG_REG = 3

IO_EXP_CONFIG_REG_INIT = IO_EXP_Jack_Detect_Mask
IO_EXP_OUTPUT_REG_INIT = 0b00000000


class TCA6408:
    def __init__(self, i2c_bus, address=0x20):
        self._i2c_device = i2c_device.I2CDevice(i2c_bus, address)
        self._buf = bytearray(2)
        self._reg_state = IO_EXP_OUTPUT_REG_INIT

        self._write_register(IO_EXP_OUTPUT_REG, self._reg_state)
        self._write_register(IO_EXP_CONFIG_REG, IO_EXP_CONFIG_REG_INIT)

    def _write_register(self, reg, value):
        self._buf[0] = reg & 0xFF
        self._buf[1] = value & 0xFF
        with self._i2c_device as i2c:
            i2c.write(self._buf)

    def _set_out(self, new_state):
        self._write_register(IO_EXP_OUTPUT_REG, new_state)
        self._reg_state = new_state

    def enable_codec(self):
        self._set_out(self._reg_state | IO_EXP_DAC_Not_Reset_Mask)

    def enable_speakers(self, left, right):
        new_state = self._reg_state
        if left:
            new_state = new_state | IO_EXP_SPK_Enable_L_Mask
        else:
            new_state = new_state & ~IO_EXP_SPK_Enable_L_Mask

        if right:
            new_state = new_state | IO_EXP_SPK_Enable_R_Mask
        else:
            new_state = new_state & ~IO_EXP_SPK_Enable_R_Mask

        self._set_out(new_state)

    def set_speakers_gain(self, g0, g1):
        new_state = self._reg_state

        if g0:
            new_state = new_state | IO_EXP_SPK_Gain_0_Mask
        else:
            new_state = new_state & ~IO_EXP_SPK_Gain_0_Mask

        if g1:
            new_state = new_state | IO_EXP_SPK_Gain_1_Mask
        else:
            new_state = new_state & ~IO_EXP_SPK_Gain_1_Mask
