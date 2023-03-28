try:
    import time
    import board
    from digitalio import DigitalInOut, Direction, Pull
except ImportError:
    print("SKIP")
    raise SystemExit

list_pin_free = []
list_pin_busy = []

if board.board_id == "brd2601b":
    list_pin_free = [
        board.PA0,
        board.PA4,
        board.PA7,
        board.PB0,
        board.PB1,
        board.PB2,
        board.PB3,
        board.PC4,
        board.PC5,
        board.PC9,
        board.PD2,
        board.PD3,
        board.PD4,
        board.PD5,
    ]

    list_pin_busy = [
        board.PA5,
        board.PA6,
        board.PC0,
        board.PC1,
        board.PC2,
        board.PC3,
    ]
elif board.board_id == "brd2703a":
    list_pin_free = [
        board.PA0,
        board.PA4,
        board.PA7,
        board.PB0,
        board.PB1,
        board.PB2,
        board.PB3,
        board.PB4,
        board.PB5,
        board.PC0,
        board.PC1,
        board.PC2,
        board.PC3,
        board.PC4,
        board.PC5,
        board.PC8,
        board.PC9,
        board.PD2,
        board.PD3,
        board.PD4,
        board.PD5,
    ]
    list_pin_busy = [
        board.PA5,
        board.PA6,
    ]

elif board.board_id == "brd2704a":
    list_pin_free = [
        board.PA0,
        board.PA4,
        board.PA7,
        board.PB0,
        board.PB1,
        board.PB2,
        board.PB3,
        board.PC0,
        board.PC1,
        board.PC2,
        board.PC3,
        board.PC4,
        board.PC5,
        board.PC6,
        board.PC7,
        board.PD0,
        board.PD1,
        board.PD2,
        board.PD3,
    ]
    list_pin_busy = [
        board.PA5,
        board.PA6,
    ]
io_free = []

for pin in list_pin_busy:
    try:
        io = DigitalInOut(pin)
        print("FAIL")
    except:
        pass

for pin in list_pin_free:
    try:
        io_free.append(DigitalInOut(pin))
    except:
        print("FAIL")

for io in io_free:
    io.direction = Direction.OUTPUT
    io.value = False
    time.sleep(0.1)
    io.value = True
    time.sleep(0.1)
    io.direction = Direction.INPUT
    io.pull = Pull.UP

for io in io_free:
    io.deinit()

print("OK")
del list_pin_free, list_pin_busy, io_free
