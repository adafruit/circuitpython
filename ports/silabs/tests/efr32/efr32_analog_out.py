try:
    import time
    import board
    import analogio
except ImportError:
    print("SKIP")
    raise SystemExit

list_pin_free = []
list_dac_pin = []

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
    list_dac_pin = [board.PB0, board.PB1, board.PB2, board.PB3]

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
    list_dac_pin = [board.PB0, board.PB1, board.PB2, board.PB3]

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
    list_dac_pin = [board.PB0, board.PB1, board.PB2, board.PB3]

analogio_arr = []

for pin in list_pin_free:

    if pin in list_dac_pin:
        try:
            adc_out = analogio.AnalogOut(pin)
            analogio_arr.append(adc_out)
        except:
            print("FAIL")
    else:
        try:
            analogio.AnalogOut(pin)
            print("FAIL")
        except:
            pass

for pin in analogio_arr:
    pin.deinit()

print("OK")
del list_pin_free, analogio_arr
