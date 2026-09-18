# Datasheets

The package pin maps in `../packages/` are transcribed from Nordic's SoC
datasheets. The PDFs are not redistributed in this repository; download them
from Nordic (https://docs.nordicsemi.com) into this directory and verify them
against the SHA-256 hashes below before regenerating a map.

| File | SHA-256 |
|------|---------|
| `nRF54L15_nRF54L10_nRF54L05_Datasheet_v1.0.pdf` | `ade0d340ba95e31f8299e6721b08d53a702962335d9b87fa202f8d2767603554` |
| `nRF54LM20A_nRF54LM20B_Datasheet_v1.0.pdf` | `03a834fb52be8cf6248df6f67737f055c035cfb83492d44d58bdc2f0bfa3a3af` |
| `nRF5340_PS_v1.6.pdf` | `fe5c7e8908c94080548a1ddd4ec8b96aa09a4b97239fe9b32afb49eb8cf2fc87` |
| `nRF52840_PS_v1.1.pdf` | `c619e336b9c0610663273041f057f2537a65fd408ce0c5b8214a26de2aa88422` |
| `[nRF52840] MDBT50Q-1MV2 & MDBT50Q-P1MV2_Ver.L spec.pdf` | `61fec8c0c9f8c33175be2237a8ebba73c6cfc0a3572fe3835fd341079c103d03` |
| `rp2040-datasheet.pdf` | `be56fbb75ba0ae9e26558a73c93ac3e75c2ad4e6878d3b6703de2a76d886ea8c` |

The nRF52840 Product Specification is Nordic's, downloadable from
https://docs.nordicsemi.com; the copy here came from
https://cdn-learn.adafruit.com/assets/assets/000/092/427/original/nRF52840_PS_v1.1.pdf.
The MDBT50Q module datasheet is Raytac's, from
https://www.raytac.com. The RP2040 datasheet is Raspberry Pi's, from
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf.

The transcription command lines are recorded in the header of each
`packages/*.toml` (section number, package name, pin count). After running
`pdftotext -layout` on a verified PDF, the same command reproduces the map.

This directory is ignored by git; this README is force-added so the hashes
stay with the code.
