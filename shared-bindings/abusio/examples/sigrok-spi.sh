#!/usr/bin/env bash
# sigrok-spi.sh — Capture and decode SPI traffic using an fx2lafw logic analyzer.
#
# Pin mapping (Pico2W → fx2lafw):
#   GP1 (CS)   → D6
#   GP2 (SCK)  → D5
#   GP3 (MOSI) → D3
#   GP4 (MISO) → D1
#
# Usage:
#   ./sigrok-spi.sh [options]
#
# Options:
#   --cpol <0|1>     Clock polarity (default: 0)
#   --cpha <0|1>     Clock phase    (default: 0)
#   --mode <0-3>     Shorthand for CPOL/CPHA pair:
#                      0 → CPOL=0 CPHA=0  (idle low,  sample leading edge)
#                      1 → CPOL=0 CPHA=1  (idle low,  sample trailing edge)
#                      2 → CPOL=1 CPHA=0  (idle high, sample leading edge)
#                      3 → CPOL=1 CPHA=1  (idle high, sample trailing edge)
#   --rate <rate>    Sample rate (default: 4m).  Accepted: 200k 500k 1m 2m 4m 8m 12m 16m 24m
#   --samples <n>    Number of samples to capture (default: 1000000)
#   --cs-polarity <active-low|active-high>
#                    CS polarity (default: active-low)
#   --bitorder <msb-first|lsb-first>
#                    Bit order (default: msb-first)
#   --wordsize <n>   Bits per word (default: 8)
#   --output <file>  Write decoded output to file instead of stdout
#   --no-trigger     Disable CS trigger (free-run instead of waiting for CS to fall)
#   -h, --help       Show this help message

set -euo pipefail

# ── Defaults ────────────────────────────────────────────────────────────────
CPOL=0
CPHA=0
RATE="4m"
SAMPLES=100000
CS_POL="active-low"
BITORDER="msb-first"
WORDSIZE=8
OUTPUT=""
TRIGGER=1

# ── Argument parsing ─────────────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
    case "$1" in
        --cpol)
            CPOL="$2"; shift 2
            ;;
        --cpha)
            CPHA="$2"; shift 2
            ;;
        --mode)
            case "$2" in
                0) CPOL=0; CPHA=0 ;;
                1) CPOL=0; CPHA=1 ;;
                2) CPOL=1; CPHA=0 ;;
                3) CPOL=1; CPHA=1 ;;
                *) echo "ERROR: --mode must be 0, 1, 2, or 3" >&2; exit 1 ;;
            esac
            shift 2
            ;;
        --rate)
            RATE="$2"; shift 2
            ;;
        --samples)
            SAMPLES="$2"; shift 2
            ;;
        --cs-polarity)
            CS_POL="$2"; shift 2
            ;;
        --bitorder)
            BITORDER="$2"; shift 2
            ;;
        --wordsize)
            WORDSIZE="$2"; shift 2
            ;;
        --output)
            OUTPUT="$2"; shift 2
            ;;
        --no-trigger)
            TRIGGER=0; shift
            ;;
        -h|--help)
            sed -n '/^# Usage:/,/^[^#]/{ /^[^#]/d; s/^# \{0,3\}//; p }' "$0"
            exit 0
            ;;
        *)
            echo "ERROR: Unknown option: $1" >&2
            echo "Run '$0 --help' for usage." >&2
            exit 1
            ;;
    esac
done

# ── Validate ─────────────────────────────────────────────────────────────────
if [[ "$CPOL" != "0" && "$CPOL" != "1" ]]; then
    echo "ERROR: --cpol must be 0 or 1" >&2; exit 1
fi
if [[ "$CPHA" != "0" && "$CPHA" != "1" ]]; then
    echo "ERROR: --cpha must be 0 or 1" >&2; exit 1
fi

# ── Derive mode number for display ───────────────────────────────────────────
MODE=$(( CPOL * 2 + CPHA ))
case "$MODE" in
    0) MODE_DESC="idle low,  sample on leading  (rising)  edge" ;;
    1) MODE_DESC="idle low,  sample on trailing (falling) edge" ;;
    2) MODE_DESC="idle high, sample on leading  (falling) edge" ;;
    3) MODE_DESC="idle high, sample on trailing (rising)  edge" ;;
esac

# ── Build sigrok-cli command ──────────────────────────────────────────────────
# Channel map: only capture the four channels we need.
# fx2lafw enumerates all 8 channels; name them by their physical labels.
DECODER_OPTS="cs=D6:clk=D5:mosi=D3:miso=D1"
DECODER_OPTS+=":cpol=${CPOL}:cpha=${CPHA}"
DECODER_OPTS+=":cs_polarity=${CS_POL}"
DECODER_OPTS+=":bitorder=${BITORDER}"
DECODER_OPTS+=":wordsize=${WORDSIZE}"

# CS trigger: D6 falls when CS goes active-low (or rises for active-high)
if [[ "$TRIGGER" == "1" ]]; then
    if [[ "$CS_POL" == "active-low" ]]; then
        TRIGGER_SPEC="D6=f"
    else
        TRIGGER_SPEC="D6=r"
    fi
fi

CMD=(
    sigrok-cli
    --driver fx2lafw
    --config "samplerate=${RATE}"
    --samples "$SAMPLES"
    --channels "D1,D3,D5,D6"
)
[[ "$TRIGGER" == "1" ]] && CMD+=(--triggers "$TRIGGER_SPEC")
CMD+=(
    --protocol-decoders "spi:${DECODER_OPTS}"
    --protocol-decoder-samplenum
    --protocol-decoder-annotations spi
)

# ── Print summary ─────────────────────────────────────────────────────────────
cat <<EOF
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 SPI capture — fx2lafw
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 Mode:        SPI mode ${MODE}  (CPOL=${CPOL}, CPHA=${CPHA})
              ${MODE_DESC}
 Sample rate: ${RATE}    Samples: ${SAMPLES}
 CS polarity: ${CS_POL}    Bit order: ${BITORDER}    Word: ${WORDSIZE}-bit
 Trigger:     $( [[ "$TRIGGER" == "1" ]] && echo "CS ${TRIGGER_SPEC}" || echo "none (free-run)" )

 Channel mapping (fx2lafw → Pico2W):
   D6 → GP1  (CS)
   D5 → GP2  (SCK)
   D3 → GP3  (MOSI)
   D1 → GP4  (MISO)

 Command:
   ${CMD[*]}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
EOF

# ── Run ───────────────────────────────────────────────────────────────────────
if [[ -n "$OUTPUT" ]]; then
    "${CMD[@]}" | tee "$OUTPUT"
else
    "${CMD[@]}"
fi
