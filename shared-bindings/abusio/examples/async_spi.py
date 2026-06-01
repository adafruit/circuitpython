import array
import asyncio
import time

import board
import digitalio
import abusio

spi = abusio.SPI(board.GP2, board.GP3, board.GP4)
spi.configure(baudrate=100_000, polarity=0, phase=0)
cs = digitalio.DigitalInOut(board.GP1)
cs.direction = digitalio.Direction.OUTPUT
cs.value = True


async def spier():
    while True:
        while not spi.try_lock():
            await asyncio.sleep(0)
        tx_buf = array.array("h", list(range(1000)))
        rx_buf = array.array("h", [0] * 1000)
        tick = time.monotonic()
        for i in range(10):
            cs.value = False
            await spi.write_readinto(tx_buf, rx_buf)
            cs.value = True
        # await asyncio.sleep(0)  # Uncomment if you suspect the driver is not yielding
        tock = time.monotonic()
        spi.unlock()
        print(f"Harumph {tock - tick}")


async def talker():
    msg = "Hi there SPI!"
    while True:
        for s in msg.split():
            print(s, end=" ")
            await asyncio.sleep(0)
        print("")
        await asyncio.sleep(1)


async def main():
    await asyncio.gather(spier(), talker())


asyncio.run(main())
