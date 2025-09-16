from board import *
import audiobusio
import busio
import time
import _aic3105
import _tca6408

_AUDIO = None
_DAC = None
_I2C = None
_IO_EXP = None
_SAMPLE_RATE = 22050


def io_exp():
    global _IO_EXP

    if not _IO_EXP:
        _IO_EXP = _tca6408.TCA6408(i2c())

    return _IO_EXP


def i2c():
    global _I2C

    if not _I2C:
        SCL = GP7
        SDA = GP6
        _I2C = busio.I2C(SCL, SDA)

    return _I2C


def _init_dac(sample_rate):
    global _DAC
    global _SAMPLE_RATE

    if not _DAC:
        _SAMPLE_RATE = sample_rate

        _DAC = _aic3105.AIC3105(i2c(), sample_rate)
        _DAC.start_i2s_out()

    return _DAC


def audio(sample_rate=22050):
    global _AUDIO
    global _DAC

    if not _AUDIO:
        i2s_out = GP1
        i2s_lrclk = GP2
        i2s_bclk = GP3

        io_exp().enable_codec()

        time.sleep(0.1)

        # Must init the dac here
        _init_dac(sample_rate)
        _DAC.set_HP_volume(0.5, 0.5)

        _AUDIO = audiobusio.I2SOut(i2s_bclk, i2s_lrclk, i2s_out, left_justified=False)

    return _AUDIO


def sample_rate():
    return _SAMPLE_RATE


def _check_dac_init():
    global _DAC
    if not _DAC:
        raise RuntimeError("audio not initialized. Call noise_nugget.audio() first.")


def set_HP_volume(left, right):
    global _DAC

    _check_dac_init()

    _DAC.set_HP_volume(left, right)


def enable_speakers(left, right):
    global _DAC

    _check_dac_init()

    _DAC.enable_line_out(left, right)
    io_exp().enable_speakers(left, right)


def set_speakers_volume(left_to_left, right_to_right, left_to_right=0.0, right_to_left=0.0):
    global _DAC

    _check_dac_init()

    _DAC.set_line_out_volume(left_to_left, right_to_right, left_to_right, right_to_left)


def set_speakers_gain(gain):
    if gain == 0:
        io_exp().set_speakers_gain(False, False)
    elif gain == 1:
        io_exp().set_speakers_gain(True, False)
    elif gain == 2:
        io_exp().set_speakers_gain(False, True)
    elif gain == 3:
        io_exp().set_speakers_gain(True, True)
    else:
        raise RuntimeError("invalid speaker gain " + str(gain) + " (must be between 0 and 3)")
