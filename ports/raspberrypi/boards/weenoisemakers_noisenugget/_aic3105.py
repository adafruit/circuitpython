from adafruit_bus_device import i2c_device

import time

# Page select register
AIC3X_PAGE_SELECT = 0
# Software reset register
AIC3X_RESET = 1
# Codec Sample rate select register
AIC3X_SAMPLE_RATE_SEL_REG = 2
# PLL progrramming register A
AIC3X_PLL_PROGA_REG = 3
# PLL progrramming register B
AIC3X_PLL_PROGB_REG = 4
# PLL progrramming register C
AIC3X_PLL_PROGC_REG = 5
# PLL progrramming register D
AIC3X_PLL_PROGD_REG = 6
# Codec datapath setup register
AIC3X_CODEC_DATAPATH_REG = 7
# Audio serial data interface control register A
AIC3X_ASD_INTF_CTRLA = 8
# Audio serial data interface control register B
AIC3X_ASD_INTF_CTRLB = 9
# Audio serial data interface control register C
AIC3X_ASD_INTF_CTRLC = 10
# Audio overflow status and PLL R value programming register
AIC3X_OVRF_STATUS_AND_PLLR_REG = 11
# Audio codec digital filter control register
AIC3X_CODEC_DFILT_CTRL = 12
# Headset/button press detection register
AIC3X_HEADSET_DETECT_CTRL_A = 13
AIC3X_HEADSET_DETECT_CTRL_B = 14
# ADC PGA Gain control registers
LADC_VOL = 15
RADC_VOL = 16
# MIC3 control registers
MIC3LR_2_LADC_CTRL = 17
MIC3LR_2_RADC_CTRL = 18
# Line1 Input control registers
LINE1L_2_LADC_CTRL = 19
LINE1R_2_LADC_CTRL = 21
LINE1R_2_RADC_CTRL = 22
LINE1L_2_RADC_CTRL = 24
# Line2 Input control registers
LINE2L_2_LADC_CTRL = 20
LINE2R_2_RADC_CTRL = 23
# MICBIAS Control Register
MICBIAS_CTRL = 25

# AGC Control Registers A, B, C
LAGC_CTRL_A = 26
LAGC_CTRL_B = 27
LAGC_CTRL_C = 28
RAGC_CTRL_A = 29
RAGC_CTRL_B = 30
RAGC_CTRL_C = 31

# DAC Power and Left High Power Output control registers
DAC_PWR = 37
HPLCOM_CFG = 37
# Right High Power Output control registers
HPRCOM_CFG = 38
# High Power Output Stage Control Register
HPOUT_SC = 40
# DAC Output Switching control registers
DAC_LINE_MUX = 41
# High Power Output Driver Pop Reduction registers
HPOUT_POP_REDUCTION = 42
# DAC Digital control registers
LDAC_VOL = 43
RDAC_VOL = 44
# Left High Power Output control registers
LINE2L_2_HPLOUT_VOL = 45
PGAL_2_HPLOUT_VOL = 46
DACL1_2_HPLOUT_VOL = 47
LINE2R_2_HPLOUT_VOL = 48
PGAR_2_HPLOUT_VOL = 49
DACR1_2_HPLOUT_VOL = 50
HPLOUT_CTRL = 51
# Left High Power COM control registers
LINE2L_2_HPLCOM_VOL = 52
PGAL_2_HPLCOM_VOL = 53
DACL1_2_HPLCOM_VOL = 54
LINE2R_2_HPLCOM_VOL = 55
PGAR_2_HPLCOM_VOL = 56
DACR1_2_HPLCOM_VOL = 57
HPLCOM_CTRL = 58
# Right High Power Output control registers
LINE2L_2_HPROUT_VOL = 59
PGAL_2_HPROUT_VOL = 60
DACL1_2_HPROUT_VOL = 61
LINE2R_2_HPROUT_VOL = 62
PGAR_2_HPROUT_VOL = 63
DACR1_2_HPROUT_VOL = 64
HPROUT_CTRL = 65
# Right High Power COM control registers
LINE2L_2_HPRCOM_VOL = 66
PGAL_2_HPRCOM_VOL = 67
DACL1_2_HPRCOM_VOL = 68
LINE2R_2_HPRCOM_VOL = 69
PGAR_2_HPRCOM_VOL = 70
DACR1_2_HPRCOM_VOL = 71
HPRCOM_CTRL = 72
# Mono Line Output Plus/Minus control registers
LINE2L_2_MONOLOPM_VOL = 73
PGAL_2_MONOLOPM_VOL = 74
DACL1_2_MONOLOPM_VOL = 75
LINE2R_2_MONOLOPM_VOL = 76
PGAR_2_MONOLOPM_VOL = 77
DACR1_2_MONOLOPM_VOL = 78
MONOLOPM_CTRL = 79
# Left Line Output Plus/Minus control registers
LINE2L_2_LLOPM_VOL = 80
PGAL_2_LLOPM_VOL = 81
DACL1_2_LLOPM_VOL = 82
LINE2R_2_LLOPM_VOL = 83
PGAR_2_LLOPM_VOL = 84
DACR1_2_LLOPM_VOL = 85
LLOPM_CTRL = 86
# Right Line Output Plus/Minus control registers
LINE2L_2_RLOPM_VOL = 87
PGAL_2_RLOPM_VOL = 88
DACL1_2_RLOPM_VOL = 89
LINE2R_2_RLOPM_VOL = 90
PGAR_2_RLOPM_VOL = 91
DACR1_2_RLOPM_VOL = 92
RLOPM_CTRL = 93

MODULE_POWER_STATUS = 94

# GPIO/IRQ registers
AIC3X_STICKY_IRQ_FLAGS_REG = 96
AIC3X_RT_IRQ_FLAGS_REG = 97

AIC3X_CLOCK_REG = 101
# Clock generation control register
AIC3X_CLKGEN_CTRL_REG = 102
# New AGC registers
LAGCN_ATTACK = 103
LAGCN_DECAY = 104
RAGCN_ATTACK = 105
RAGCN_DECAY = 106
# New Programmable ADC Digital Path and I2C Bus Condition Register
NEW_ADC_DIGITALPATH = 107
# Passive Analog Signal Bypass Selection During Powerdown Register
PASSIVE_BYPASS = 108
# DAC Quiescent Current Adjustment Register
DAC_ICC_ADJ = 109


# My kingdom for enums, and strong typing...
LINE2_L = 0
PGA_L = 1
DAC_L1 = 2
LINE2_R = 3
PGA_R = 4
DAC_R1 = 5

# Don't use the same values for the sink and source "enums", this way a runtime
# error will tell us when we mix the two.
HP_L_OUT = 10
HP_L_COM = 11
HP_R_OUT = 12
HP_R_COM = 13
LINE_OUT_L = 14
LINE_OUT_R = 15


class AIC3105:
    def __init__(self, i2c_bus, sample_rate, address=0x18):
        self.i2c_device = i2c_device.I2CDevice(i2c_bus, address)
        self._buf = bytearray(2)
        self._sample_rate = sample_rate

        self._registers_copy = [
            0b00000000,  # 0
            0b00000000,  # 1
            0b00000000,  # 2
            0b00010000,  # 3
            0b00000010,  # 4
            0b00000000,  # 5
            0b00000000,  # 6
            0b00000000,  # 7
            0b00000000,  # 8
            0b00000000,  # 9
            0b00000000,  # 10
            0b00000001,  # 11
            0b00000000,  # 12
            0b00000000,  # 13
            0b00000000,  # 14
            0b10000000,  # 15
            0b10000000,  # 16
            0b11111111,  # 17
            0b11111111,  # 18
            0b01111000,  # 19
            0b01111000,  # 20
            0b01111000,  # 21
            0b01111000,  # 22
            0b01111000,  # 23
            0b01111000,  # 24
            0b00000000,  # 25
            0b00000000,  # 26
            0b11111110,  # 27
            0b00000000,  # 28
            0b00000000,  # 29
            0b11111110,  # 30
            0b00000000,  # 31
            0b00000000,  # 32
            0b00000000,  # 33
            0b00000000,  # 34
            0b00000000,  # 35
            0b00000000,  # 36
            0b00000000,  # 37
            0b00000000,  # 38
            0b00000000,  # 39
            0b00000000,  # 40
            0b00000000,  # 41
            0b00000000,  # 42
            0b10000000,  # 43
            0b10000000,  # 44
            0b00000000,  # 45
            0b00000000,  # 46
            0b00000000,  # 47
            0b00000000,  # 48
            0b00000000,  # 49
            0b00000000,  # 50
            0b00000110,  # 51
            0b00000000,  # 52
            0b00000000,  # 53
            0b00000000,  # 54
            0b00000000,  # 55
            0b00000000,  # 56
            0b00000000,  # 57
            0b00000110,  # 58
            0b00000000,  # 59
            0b00000000,  # 60
            0b00000000,  # 61
            0b00000000,  # 62
            0b00000000,  # 63
            0b00000000,  # 64
            0b00000010,  # 65
            0b00000000,  # 66
            0b00000000,  # 67
            0b00000000,  # 68
            0b00000000,  # 69
            0b00000000,  # 70
            0b00000000,  # 71
            0b00000010,  # 72
            0b00000000,  # 73
            0b00000000,  # 74
            0b00000000,  # 75
            0b00000000,  # 76
            0b00000000,  # 77
            0b00000000,  # 78
            0b00000000,  # 79
            0b00000000,  # 80
            0b00000000,  # 81
            0b00000000,  # 82
            0b00000000,  # 83
            0b00000000,  # 84
            0b00000000,  # 85
            0b00000010,  # 86
            0b00000000,  # 87
            0b00000000,  # 88
            0b00000000,  # 89
            0b00000000,  # 90
            0b00000000,  # 91
            0b00000000,  # 92
            0b00000010,  # 93
            0b00000000,  # 94
            0b00000000,  # 95
            0b00000000,  # 96
            0b00000000,  # 97
            0b00000000,  # 98
            0b00000000,  # 99
            0b00000000,  # 100
            0b00000000,  # 101
            0b00000010,  # 102
            0b00000000,  # 103
            0b00000000,  # 104
            0b00000000,  # 105
            0b00000000,  # 106
            0b00000000,  # 107
            0b00000000,  # 108
            0b00000000,
        ]  # 109

    def _write_register(self, reg, value):
        self._buf[0] = reg & 0xFF
        self._buf[1] = value & 0xFF
        # print("_write_register reg:0x{:02x} value:0x{:02x}".format(reg, value))
        with self.i2c_device as i2c:
            i2c.write(self._buf)
        self._registers_copy[reg] = value

    def _write_bit(self, reg, pos, value):
        current = self._registers_copy[reg]
        mask = 1 << pos

        if value == 0:
            new = current & (~mask)
        else:
            new = current | mask
        self._write_register(reg, new)

    def _write_multi(self, reg, msb, lsb, value):
        new = self._registers_copy[reg]

        # Clear bits for the range we case about
        for x in range(lsb, msb + 1):
            new = new & (~(1 << x))

        new = new | (value << lsb)

        self._write_register(reg, new)

    def sink_base_register(self, sink):
        if sink == HP_L_OUT:
            return 45
        if sink == HP_L_COM:
            return 52
        if sink == HP_R_OUT:
            return 59
        if sink == HP_R_COM:
            return 66
        if sink == LINE_OUT_L:
            return 80
        if sink == LINE_OUT_R:
            return 87

        raise RuntimeError("invalid sink")

    def source_register_offset(self, source):
        if source == LINE2_L:
            return 0
        if source == PGA_L:
            return 1
        if source == DAC_L1:
            return 2
        if source == LINE2_R:
            return 3
        if source == PGA_R:
            return 4
        if source == DAC_R1:
            return 5
        raise RuntimeError("invalid source")

    def power_on(self, sink):
        reg = self.sink_base_register(sink) + 6
        self._write_bit(reg, 0, 1)

    def power_off(self, sink):
        reg = self.sink_base_register(sink) + 6
        self._write_bit(reg, 0, 0)

    def mute(self, sink):
        reg = self.sink_base_register(sink) + 6
        self._write_bit(reg, 3, 0)

    def unmute(self, sink):
        reg = self.sink_base_register(sink) + 6
        self._write_bit(reg, 3, 1)

    def route(self, source, sink):
        reg = self.sink_base_register(sink) + self.source_register_offset(source)
        self._write_bit(reg, 7, 1)

    def unroute(self, source, sink):
        reg = self.sink_base_register(sink) + self.source_register_offset(source)
        self._write_bit(reg, 7, 0)

    def volume_convert(self, volume, min, max, mute_value):
        if volume == 0.0 or volume < 0.0:
            return mute_value
        elif volume > 1.0:
            volume = 1.0

        if max > min:
            amplitude = max - min
            val = volume * amplitude
            return int(min + val)
        else:
            amplitude = min - max
            val = (1.0 - volume) * amplitude
            return int(max + val)

    def PGA_volume(self, volume):
        return self.volume_convert(volume, 0, 0b1111111, 0)

    def output_stage_volume(self, volume):
        return self.volume_convert(volume, 117, 0, 118)

    def set_volume(self, source, sink, volume):
        vol = self.output_stage_volume(volume)
        reg = self.sink_base_register(sink) + self.source_register_offset(source)
        self._write_multi(reg, 6, 0, vol)

    def set_HP_volume(self, left, right):
        self.set_volume(DAC_L1, HP_L_OUT, left)
        self.set_volume(DAC_R1, HP_R_OUT, right)

    def enable_line_out(self, left, right):
        if left:
            self.power_on(LINE_OUT_L)
            self.route(DAC_L1, LINE_OUT_L)
            self.route(DAC_L1, LINE_OUT_R)
            self.unmute(LINE_OUT_L)
        else:
            self.power_off(LINE_OUT_L)
            self.unroute(DAC_L1, LINE_OUT_L)
            self.unroute(DAC_L1, LINE_OUT_R)
            self.mute(LINE_OUT_L)

        if right:
            self.power_on(LINE_OUT_R)
            self.route(DAC_R1, LINE_OUT_L)
            self.route(DAC_R1, LINE_OUT_R)
            self.unmute(LINE_OUT_R)
        else:
            self.power_off(LINE_OUT_R)
            self.unroute(DAC_R1, LINE_OUT_L)
            self.unroute(DAC_R1, LINE_OUT_R)
            self.mute(LINE_OUT_R)

    def set_line_out_volume(
        self, left_to_left, right_to_right, left_to_right=0.0, right_to_left=0.0
    ):
        self.set_volume(DAC_L1, LINE_OUT_L, left_to_left)
        self.set_volume(DAC_L1, LINE_OUT_R, left_to_right)
        self.set_volume(DAC_R1, LINE_OUT_R, right_to_right)
        self.set_volume(DAC_R1, LINE_OUT_L, right_to_left)

    def enable_mic_bias(self):
        self._write_multi(MICBIAS_CTRL, 7, 6, 0b10)

    def start_i2s_out(self):
        if self._sample_rate == 8000:
            cfg = [5, 4613, 1, 8]
        elif self._sample_rate == 16000:
            cfg = [5, 4613, 1, 4]
        elif self._sample_rate == 22050:
            cfg = [7, 5264, 1, 4]
        elif self._sample_rate == 32000:
            cfg = [5, 4613, 1, 2]
        elif self._sample_rate == 44100:
            cfg = [7, 5264, 1, 2]
        elif self._sample_rate == 48000:
            cfg = [8, 1920, 1, 2]
        else:
            raise RuntimeError("invalid sample rate: " + str(self._sample_rate))

        J = cfg[0]
        D = cfg[1]
        R = cfg[2]
        P = cfg[3]

        # Select Page 0
        self._write_bit(AIC3X_PAGE_SELECT, 0, 0)

        # Soft reset
        self._write_bit(AIC3X_RESET, 7, 1)

        # Let's start with clock configuration.

        # PLL P = 2
        self._write_multi(AIC3X_PLL_PROGA_REG, 2, 0, P)

        # PLL R = 1
        self._write_multi(AIC3X_OVRF_STATUS_AND_PLLR_REG, 3, 0, R)

        # PLL J = 7
        self._write_multi(AIC3X_PLL_PROGB_REG, 7, 2, J)

        # PLL D = 5264
        PLL_D = D
        REG_C = PLL_D >> 6
        REG_D = PLL_D & 0x3F
        self._write_multi(AIC3X_PLL_PROGC_REG, 7, 0, REG_C)
        self._write_multi(AIC3X_PLL_PROGD_REG, 7, 2, REG_D)

        # Select the PLLCLK_IN source. 0: MCLK, 1: GPIO2, 2: BCLK
        self._write_multi(AIC3X_CLKGEN_CTRL_REG, 5, 4, 2)

        # Select the CLKDIV_IN source. 0: MCLK, 1: GPIO2, 2: BCLK
        #
        # Note: When PLL is used CLKDIV_IN still needs some kind of clock
        # signal. So if there's no MCLK, BCLK should be used here as well
        self._write_multi(AIC3X_CLKGEN_CTRL_REG, 6, 7, 0)

        # Enable PLL
        self._write_bit(AIC3X_PLL_PROGA_REG, 7, 1)

        # Set FS(ref) value for AGC time constants to 44.1KHz
        self._write_bit(AIC3X_CODEC_DATAPATH_REG, 7, 1)

        # CODEC_CLKIN Source Selection. 0: PLLDIV_OUT. 1: CLKDIV_OUT
        self._write_bit(AIC3X_CLOCK_REG, 0, 0)

        # Note: We leave the ADC Sample Rate Select and DAC Sample Rate Select
        # at the default value: fs(ref) / 1

        # Audio Serial Data Interface at the default settings: I2S
        # mode, 16bits words,
        self._write_multi(AIC3X_ASD_INTF_CTRLB, 7, 6, 0b00)

        # Output Driver Power-On Delay Control
        self._write_multi(HPOUT_POP_REDUCTION, 7, 4, 0b1000)

        # Driver Ramp-Up Step Timing Control
        self._write_multi(HPOUT_POP_REDUCTION, 3, 2, 0b01)

        # Power outputs
        self.power_on(HP_L_OUT)
        self.power_on(HP_R_OUT)

        # L and R DACs Power On
        self._write_multi(DAC_PWR, 7, 6, 0b11)

        # Left DAC plays left input data
        self._write_multi(AIC3X_CODEC_DATAPATH_REG, 4, 3, 0b01)
        # Right DAC plays right input data
        self._write_multi(AIC3X_CODEC_DATAPATH_REG, 2, 1, 0b01)

        # Unmute L DAC
        self._write_bit(LDAC_VOL, 7, 0)
        # Unmute R DAC
        self._write_bit(RDAC_VOL, 7, 0)

        # Left-DAC output selects DAC_L1 path.
        self._write_multi(DAC_LINE_MUX, 7, 6, 0)
        # Right-DAC output selects DAC_R1 path.
        self._write_multi(DAC_LINE_MUX, 5, 4, 0)

        # DAC to HP
        self.route(DAC_L1, HP_L_OUT)
        self.route(DAC_R1, HP_R_OUT)

        # DAC to Line-Out
        self.route(DAC_L1, LINE_OUT_L)
        self.route(DAC_R1, LINE_OUT_R)

        # Enable Left ADC
        self._write_bit(LINE1L_2_LADC_CTRL, 2, 1)
        # Enable Right ADC
        self._write_bit(LINE1R_2_RADC_CTRL, 2, 1)

        # Unmute L ADC PGA
        self._write_bit(LADC_VOL, 7, 0)
        # Unmute R ADC PGA
        self._write_bit(RADC_VOL, 7, 0)

        # Programs high-power outputs for ac-coupled driver configuration
        self._write_bit(AIC3X_HEADSET_DETECT_CTRL_B, 7, 1)

        # HPLCOM configured as independent single-ended output
        self._write_multi(HPLCOM_CFG, 5, 4, 2)

        # HPRCOM configured as independent single-ended output
        self._write_multi(HPRCOM_CFG, 5, 3, 1)

        # Unmute outputs
        self.unmute(HP_L_OUT)
        self.unmute(HP_R_OUT)
