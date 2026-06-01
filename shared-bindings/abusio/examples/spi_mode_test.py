"""
abusio SPI — all 4 clock/phase mode test with loopback verification.

Hardware:
  GP1  →  CS    (logic analyzer D6)
  GP2  →  SCK   (logic analyzer D5)
  GP3  →  MOSI  (logic analyzer D3) ──┐ loopback jumper
  GP4  →  MISO  (logic analyzer D1) ──┘

Wire a jumper between GP3 and GP4.  With the loopback in place rx should
equal tx for every mode.  Without the loopback the RX check is skipped and
we only verify the waveform on the logic analyzer.

To capture each mode on the logic analyzer run:
  .vscode/sigrok-spi.sh --mode <0-3> --rate 1m --samples 200000
and keep it running while the script pauses between modes.

Expected output (with loopback):
  Mode 0 (CPOL=0 CPHA=0): PASS  tx[0..3]=[0,1,2,3]  rx[0..3]=[0,1,2,3]
  Mode 1 (CPOL=0 CPHA=1): PASS  ...
  Mode 2 (CPOL=1 CPHA=0): PASS  ...
  Mode 3 (CPOL=1 CPHA=1): PASS  ...
"""

import array
import asyncio
import time

import board
import digitalio
import abusio

# ── Hardware setup ────────────────────────────────────────────────────────────
cs = digitalio.DigitalInOut(board.GP1)
cs.direction = digitalio.Direction.OUTPUT
cs.value = True

N = 64  # bytes per transfer — small enough to decode easily on the analyzer

TX = array.array("B", [0x53, 0xAC])  # Test bytes
LOOPBACK_DELAY = 0  # seconds between modes (arm the analyzer)

# ── Test ──────────────────────────────────────────────────────────────────────


async def test_mode(spi, cpol, cpha):
    mode = cpol * 2 + cpha
    print(
        f"\nMode {mode} (CPOL={cpol} CPHA={cpha}) — arm analyzer now, waiting {LOOPBACK_DELAY:.0f}s…"
    )
    await asyncio.sleep(LOOPBACK_DELAY)

    while not spi.try_lock():
        await asyncio.sleep(0)

    spi.configure(baudrate=100_000, polarity=cpol, phase=cpha)

    rx = array.array("B", [0xFF] * 2)
    cs.value = False
    await spi.write_readinto(TX, rx)
    cs.value = True

    spi.unlock()

    # Check loopback — if MISO is floating all bytes come back 0xFF, skip check.
    all_ff = all(b == 0xFF for b in rx)
    if all_ff:
        print(f"  Mode {mode}: no loopback detected (rx all 0xFF) — check waveform on analyzer")
    else:
        ok = rx == TX
        status = "PASS" if ok else "FAIL"
        print(f"  Mode {mode}: {status}  tx[0..3]={list(TX[:4])}  rx[0..3]={list(rx[:4])}")
        if not ok:
            # Show first mismatch
            for i, (t, r) in enumerate(zip(TX, rx)):
                if t != r:
                    print(f"    first mismatch at byte {i}: tx={t:#04x} rx={r:#04x}")
                    break


async def main():
    print("abusio SPI mode test")
    print(f"  {N}-byte transfer per mode, TX = 0x00..{N - 1:#04x}")
    print("  Connect jumper GP3→GP4 for loopback RX verification.")

    spi = abusio.SPI(board.GP2, board.GP3, board.GP4)

    for cpol in (0, 1):
        for cpha in (0, 1):
            await test_mode(spi, cpol, cpha)

    print("\nDone.")


asyncio.run(main())
