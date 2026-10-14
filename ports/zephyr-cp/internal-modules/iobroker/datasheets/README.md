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
| `MCXNP184M150F70.pdf` | `f3a680d2ac36960f81b474e9e7589ab7a54fc2690eaf9baa67de1d442c5135aa` |
| `MCXNP184M150F70_Pinout.xlsx` | `ba047de2d40dc99caf32502abd3afb69e88f8168dd767949f57d37a67e4295a5` |
| `UM12018(3).pdf` | `2036bf71b7a474e7cd37c55e90af2d87d09bd41d2decf30630480f7de1c68469` |
| `FRDM-MCXN947-TOP.avif` | `5ac1064c118947bd1418e248d1b6210d739fda7c8af9d7b4e600910368173e46` |

The nRF52840 Product Specification is Nordic's, downloadable from
https://docs.nordicsemi.com; the copy here came from
https://cdn-learn.adafruit.com/assets/assets/000/092/427/original/nRF52840_PS_v1.1.pdf.
The MDBT50Q module datasheet is Raytac's, from
https://www.raytac.com. The RP2040 datasheet is Raspberry Pi's, from
https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf.

MCXNP184M150F70.pdf is NXP's MCXN947/946/547/546/537/536/527/526/247
product data sheet (document MCXNP184M150F70, Rev. 8.2, 11 June 2026),
downloadable from https://www.nxp.com (MCX-N947 product page).
UM12018(3).pdf is NXP's FRDM-MCXN947 Board User Manual (UM12018 Rev. 2.0),
from https://www.nxp.com/docs/en/user-guide/UM12018.pdf. The FRDM board
photos come from the board's website (https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-MCXN947)
and show the header silkscreen.

MCXNP184M150F70_Pinout.xlsx is the pinout workbook attached to the data
sheet (Signal multiplexing and pin assignments; from the document's
Download PDF bundle at https://docs.nxp.com/bundle/MCXNP184M150F70),
with the full ball/pin map of each package, power pins included.

The MCX N maps in `packages/mcxn947_*.toml` come from the datasheet's
section 6.1 Table 93 (Pinmux): the 184BGA column is the VFBGA184 package
(Zephyr's MCXN947VDF part number, the FRDM board's chip) and the 100HLQFP
N94 column is the MCXN947/MCXN946 100-pin HLQFP (MCXN947VNL). The
transcription command lines are recorded in the header of each
`packages/*.toml`; the pin ids for the VFBGA184 map come from the
workbook's VFBGA184 tab (the full 184-ball map, power balls included, so
the pad ids carry the datasheet's gaps). Table 93 also lists the
analog-only pads (Pad type - ANA), which are not wired to a GPIO
controller: ANA_0 = ADC0_A0 (the FRDM's Arduino A0), ANA_1 = ADC0_B0
(Arduino A1), ANA_4 = ADC1_A0 (the mikroBUS AN input), ANA_5 = ADC1_B0,
ANA_6 = the DAC2 (HPDAC) output, ANA_14/18/22 = OPAMP inputs, ANA_7
unused. Both mcxn947 maps carry them
in the row-major ball numbering; zephyr2cp names their pin objects
ANA_x and binds the board's A0/A1 aliases to them, so analogio reaches
them. The DAC2 output stays unsupported: the iobroker DAC backend binds
the LPDAC (dac0, muxed on the P4_02 pad).

The transcription command lines are recorded in the header of each
`packages/*.toml` (section number, package name, pin count). After running
`pdftotext -layout` on a verified PDF, the same command reproduces the map.

This directory is ignored by git; this README is force-added so the hashes
stay with the code.
