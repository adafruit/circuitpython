try:
    import board
    import busio
    import digitalio
except ImportError:
    print("SKIP")
    raise SystemExit

list_uart_pin = []
list_pin_free = []

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

    list_uart_pin = [
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

    list_uart_pin = [
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
    list_uart_pin = [
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
for tx in list_pin_free:
    for rx in list_pin_free:
        if tx == rx:
            continue
        if tx in list_uart_pin and rx in list_uart_pin:
            uart = busio.UART(tx, rx, baudrate=115200, timeout=0)
            uart.read(16)
            uart.write(bytes("circuitpython", "utf-8"))
            uart.deinit()
        else:
            try:
                busio.UART(tx, rx, 115200)
                print("FAIL")
            except:
                pass
print("OK")
del list_pin_free, list_uart_pin
