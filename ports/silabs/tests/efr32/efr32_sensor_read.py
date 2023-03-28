try:
    import time
    import board
    import sensor
except ImportError:
    print("SKIP")
    raise SystemExit

if board.board_id == "brd2704a":
    print("SKIP")
    raise SystemExit

elif board.board_id == "brd2703a":
    print("SKIP")
    raise SystemExit

i2c = board.I2C()
sensor.init(i2c)
try:
    if sensor.temperature() != False:
        print("Temperature OK")

    if sensor.humidity() != False:
        print("Humidity OK")

    if sensor.lux() != False:
        print("Lux OK")

    if sensor.pressure() != False:
        print("Pressure OK")

    if sensor.hall() != False:
        print("HALL OK")
except:
    print("FAIL")

sensor.deinit()
i2c.deinit()
