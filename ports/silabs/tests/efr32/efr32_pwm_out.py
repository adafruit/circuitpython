try:
    import time
    import board
    import pwmio
except ImportError:
    print("SKIP")
    raise SystemExit

if board.board_id == "brd2601b":
    ledr = pwmio.PWMOut(board.LEDR, frequency=5000, duty_cycle=0)
    ledg = pwmio.PWMOut(board.LEDG, frequency=5000, duty_cycle=0)
    ledb = pwmio.PWMOut(board.LEDB, frequency=5000, duty_cycle=0)

    for index in range(0, 65535, 655):
        ledr.duty_cycle = 65535 - index
        ledg.duty_cycle = 65535 - index
        ledb.duty_cycle = 65535 - index
        time.sleep(0.01)

    for index in range(0, 65535, 655):
        ledr.duty_cycle = index
        ledg.duty_cycle = index
        ledb.duty_cycle = index
        time.sleep(0.01)
    ledr.deinit()
    ledg.deinit()
    ledb.deinit()
    del ledr, ledg, ledb

elif board.board_id == "brd2703a":
    led0 = pwmio.PWMOut(board.LED0, frequency=5000, duty_cycle=0)
    led1 = pwmio.PWMOut(board.LED1, frequency=5000, duty_cycle=0)

    for index in range(0, 65535, 655):
        led0.duty_cycle = 65535 - index
        led1.duty_cycle = 65535 - index
        time.sleep(0.01)

    for index in range(0, 65535, 655):
        led0.duty_cycle = index
        led1.duty_cycle = index
        time.sleep(0.01)
    led0.deinit()
    led1.deinit()
    del led0, led1

elif board.board_id == "brd2704a":
    ledr = pwmio.PWMOut(board.LED, frequency=5000, duty_cycle=0)

    for index in range(0, 65535, 655):
        ledr.duty_cycle = 65535 - index
        time.sleep(0.01)

    for index in range(0, 65535, 655):
        ledr.duty_cycle = index
        time.sleep(0.01)
    ledr.deinit()
    del ledr

print("OK")
