try:
    import time
    import board
    import analogio
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

analogio_arr = []
index = 0

for pin in list_pin_busy:
    try:
        adc = analogio.AnalogIn(pin)
        print("FAIL")
    except:
        pass

for pin in list_pin_free:

    try:
        adc_in = analogio.AnalogIn(pin)
        analogio_arr.append(adc_in)
        val = adc_in.value
        index += 1
    except:
        print("FAIL")

    if index > 7:
        for pin in analogio_arr:
            pin.deinit()
        analogio_arr.clear()
        index = 0

for pin in analogio_arr:
    pin.deinit()

print("OK")
del list_pin_free, list_pin_busy, analogio_arr
