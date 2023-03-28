try:
    import time
    import busio
    import board
    from digitalio import DigitalInOut, Direction, Pull
except ImportError:
    print("SKIP")
    raise SystemExit

device_list = []
i2c_sensor_en = ""


if board.board_id == "brd2601b":
    device_list = [0x29, 0x30, 0x40, 0x77]
    i2c_sensor_en = DigitalInOut(board.SENSOR_CS)
    i2c_sensor_en.direction = Direction.OUTPUT
    i2c_sensor_en.value = True
elif board.board_id == "brd2704a":
    device_list = [0x36]
else:
    print("SKIP")
    raise SystemExit

time.sleep(0.1)

i2c = board.I2C()
i2c.try_lock()
scan_list = i2c.scan()

for addr in device_list:
    if addr in scan_list:
        pass
    else:
        print("FAIL")
i2c.unlock()
i2c.deinit()
print("OK")
del device_list, i2c_sensor_en, i2c
