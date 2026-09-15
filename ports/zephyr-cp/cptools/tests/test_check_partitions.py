"""Unit tests for cptools/check_partitions.py using real device tree parsing.

Each case here is a layout that actually occurs in this port, so the checks are
exercised against devicetree as edtlib resolves it rather than against
hand-built stand-ins.
"""

import pathlib
import sys
import tempfile

import pytest

portdir = pathlib.Path(__file__).parent.parent.parent
sys.path.append(str(portdir / "zephyr/scripts/dts/python-devicetree/src/"))

from devicetree import edtlib  # noqa: E402

sys.path.insert(0, str(pathlib.Path(__file__).parent.parent))

from check_partitions import (  # noqa: E402
    check_layout,
    check_parity,
    device_size,
    evaluate,
    iter_partitions,
    read_defines,
    reference_layout,
)

BINDINGS = [str(portdir / "zephyr/dts/bindings")]


def parse_dts_string(dts_content):
    """Parse device tree source and return the resolved edtlib.EDT."""
    with tempfile.NamedTemporaryFile(mode="w", suffix=".dts", delete=False) as f:
        f.write(dts_content)
        f.flush()
        temp_path = f.name

    try:
        return edtlib.EDT(temp_path, BINDINGS)
    finally:
        pathlib.Path(temp_path).unlink()


def xip_flash(partitions, ranges="ranges;", size="0x200000"):
    """A memory-mapped flash at 0x10000000, the RP2040 arrangement.

    ``ranges`` is what the overlays get wrong: the board DTS declares it on the
    partitions node, and an overlay that deletes and rebuilds that node without
    it leaves every partition untranslated.
    """
    return f"""/dts-v1/;

/ {{
    #address-cells = <1>;
    #size-cells = <1>;

    soc {{
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        flash0: flash@10000000 {{
            compatible = "soc-nv-flash";
            erase-block-size = <4096>;
            reg = <0x10000000 {size}>;
            ranges = <0x0 0x10000000 {size}>;
            #address-cells = <1>;
            #size-cells = <1>;

            partitions {{
                {ranges}
                #address-cells = <1>;
                #size-cells = <1>;
{partitions}
            }};
        }};
    }};
}};
"""


MAPPED_PARTS = """
                code_partition: partition@100 {
                    compatible = "zephyr,mapped-partition";
                    label = "code-partition";
                    reg = <0x100 0x17ff00>;
                };

                circuitpy_partition: partition@180000 {
                    compatible = "zephyr,mapped-partition";
                    label = "circuitpy";
                    reg = <0x180000 0x80000>;
                };
"""


class TestAddressTranslation:
    """The partitions node must keep translating addresses to the SoC's space."""

    def test_missing_ranges_is_reported(self):
        """This is the bug the checker exists for: without ranges; on the
        rebuilt partitions node the partitions resolve to bare offsets."""
        edt = parse_dts_string(xip_flash(MAPPED_PARTS, ranges=""))
        problems = check_layout(edt)

        assert len(problems) == 2
        assert any("code_partition" in p and "0x100" in p for p in problems)
        assert all("ranges" in p for p in problems)

    def test_ranges_present_passes(self):
        """The same layout with ranges; must be clean, or the check would only
        ever be able to fail."""
        edt = parse_dts_string(xip_flash(MAPPED_PARTS))

        assert check_layout(edt) == []

    def test_absolute_addresses_pass(self):
        """Some overlays write the absolute address into reg and leave the
        partitions node without ranges. That resolves to the same correct
        address, so it must not be reported."""
        parts = """
                code_partition: partition@10000100 {
                    compatible = "zephyr,mapped-partition";
                    label = "code-partition";
                    reg = <0x10000100 0x17ff00>;
                };
"""
        edt = parse_dts_string(xip_flash(parts, ranges=""))

        assert check_layout(edt) == []

    def test_flash_based_at_zero_passes(self):
        """On a device based at 0 -- the nRF arrangement -- the translated and
        the bare value coincide, so nothing is wrong either way."""
        dts = """/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    soc {
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        flash0: flash@0 {
            compatible = "soc-nv-flash";
            erase-block-size = <4096>;
            reg = <0x0 0x100000>;
            ranges = <0x0 0x0 0x100000>;
            #address-cells = <1>;
            #size-cells = <1>;

            partitions {
                #address-cells = <1>;
                #size-cells = <1>;

                slot0_partition: partition@10000 {
                    compatible = "zephyr,mapped-partition";
                    label = "image-0";
                    reg = <0x10000 0x10000>;
                };
            };
        };
    };
};
"""
        edt = parse_dts_string(dts)

        assert check_layout(edt) == []


class TestPartitionDiscovery:
    """What counts as a partition is decided by position, not by compatible."""

    def test_partition_without_compatible_is_seen(self):
        """An overlay may add a partition carrying no compatible at all. It
        still occupies the space, and treating it as absent would report a
        board that has a filesystem as having none."""
        parts = """
                storage_partition: partition@180000 {
                    label = "storage";
                    reg = <0x180000 0x1000>;
                };

                circuitpy_partition: partition@181000 {
                    label = "circuitpy";
                    reg = <0x181000 0x7f000>;
                };
"""
        edt = parse_dts_string(xip_flash(parts))
        labels = [node.labels[0] for node, _dev, _off, _size, _mapped in iter_partitions(edt)]

        assert "storage_partition" in labels
        assert "circuitpy_partition" in labels


class TestDeviceSize:
    """Bus-attached flash keeps its capacity in size, not in reg."""

    def test_size_property_wins_over_chip_select(self):
        """An SPI NOR's reg is a chip select. Reading a size out of it gives 0,
        which would silently skip every check on the device CIRCUITPY usually
        lives on."""
        dts = """/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    soc {
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        spi@0 {
            compatible = "vnd,spi";
            reg = <0x0 0x100>;
            #address-cells = <1>;
            #size-cells = <0>;
            status = "okay";

            mx25r64: mx25r6435f@0 {
                compatible = "jedec,spi-nor";
                reg = <0>;
                size = <67108864>;
                spi-max-frequency = <8000000>;
                jedec-id = [c2 28 17];
            };
        };
    };
};
"""
        edt = parse_dts_string(dts)
        node = edt.get_node("/soc/spi@0/mx25r6435f@0")

        assert device_size(node) == 8 * 1024 * 1024


class TestGeometry:
    """Partitions must not overlap or leave their device."""

    def test_partition_spanning_several_others_is_reported(self):
        """Comparing neighbouring pairs only would catch the first overlap and
        miss the rest, so a partition covering three others must report more
        than once."""
        parts = """
                slot0_partition: partition@0 {
                    compatible = "zephyr,mapped-partition";
                    label = "image-0";
                    reg = <0x0 0x30000>;
                };

                nvm_partition: partition@10000 {
                    compatible = "zephyr,mapped-partition";
                    label = "nvm";
                    reg = <0x10000 0x1000>;
                };

                storage_partition: partition@20000 {
                    compatible = "zephyr,mapped-partition";
                    label = "storage";
                    reg = <0x20000 0x1000>;
                };
"""
        edt = parse_dts_string(xip_flash(parts))
        problems = [p for p in check_layout(edt) if "overlaps" in p]

        assert len(problems) == 2

    def test_partition_past_end_of_device_is_reported(self):
        """A partition may not run past the flash it lives in."""
        parts = """
                circuitpy_partition: partition@1f0000 {
                    compatible = "zephyr,mapped-partition";
                    label = "circuitpy";
                    reg = <0x1f0000 0x20000>;
                };
"""
        edt = parse_dts_string(xip_flash(parts))

        assert any("past the end" in p for p in check_layout(edt))


class TestEmptyLayout:
    """A board whose overlay defines nothing must not look verified."""

    def test_no_partitions_yields_nothing_checked(self):
        """check_layout has nothing to complain about here, which is why the
        caller reports how many partitions it inspected: an empty layout and a
        good one are otherwise indistinguishable."""
        edt = parse_dts_string(xip_flash(""))

        assert check_layout(edt) == []
        assert list(iter_partitions(edt)) == []


if __name__ == "__main__":
    sys.exit(pytest.main([__file__]))


class TestParity:
    """Agreement with the non-Zephyr build of the same board.

    The reference values are what ports/raspberrypi/mpconfigport.h and
    ports/nordic/mpconfigport.h derive for these boards; the tests read the real
    board files so a change there is caught here.
    """

    def test_evaluate_follows_references(self):
        defines = {"A": "(2 * B)", "B": "(512 * 1024)", "C": "A + UNKNOWN"}
        assert evaluate(defines, "A") == 1024 * 1024
        assert evaluate(defines, "B") == 512 * 1024
        assert evaluate(defines, "C") is None
        assert evaluate(defines, "MISSING", 7) == 7

    def test_read_defines_precedence(self, tmp_path):
        mk = tmp_path / "mpconfigboard.mk"
        mk.write_text(
            "CFLAGS += -DCIRCUITPY_FIRMWARE_SIZE='(1536 * 1024)'\nQSPI_FLASH_FILESYSTEM = 1\n"
        )
        header = tmp_path / "mpconfigport.h"
        header.write_text(
            "#ifndef CIRCUITPY_FIRMWARE_SIZE\n#define CIRCUITPY_FIRMWARE_SIZE (1020 * 1024)\n#endif\n"
        )
        defines = read_defines([mk, header])
        assert evaluate(defines, "CIRCUITPY_FIRMWARE_SIZE") == 1536 * 1024
        assert defines["QSPI_FLASH_FILESYSTEM"] == "1"

    def test_raspberrypi_default_firmware_size(self):
        layout = reference_layout("raspberrypi/raspberry_pi_pico", 0x200000)
        assert layout["nvm"] == (0xFF000, 0x1000)
        assert layout["circuitpy"] == (0x100000, 0x100000)

    def test_raspberrypi_board_firmware_size(self):
        layout = reference_layout("raspberrypi/raspberry_pi_pico_w", 0x200000)
        assert layout["nvm"] == (0x180000, 0x1000)
        assert layout["circuitpy"] == (0x181000, 0x7F000)

    def test_nordic_external_drive(self):
        layout = reference_layout("nordic/feather_nrf52840_express", 0x100000)
        assert layout["nvm"] == (0xF2000, 0x2000)
        assert layout["circuitpy"] == "external"

    def test_unknown_port_and_board_rejected(self):
        with pytest.raises(ValueError):
            reference_layout("espressif/adafruit_feather_esp32s3", 0x400000)
        with pytest.raises(ValueError):
            reference_layout("raspberrypi/no_such_board", 0x200000)

    def rp2040_layout(self, nvm_offset):
        return xip_flash(
            f"""
            code_partition: partition@100 {{
                compatible = "zephyr,mapped-partition";
                reg = <0x100 0xfdf00>;
            }};
            nvm_partition: partition@{nvm_offset:x} {{
                compatible = "zephyr,mapped-partition";
                label = "nvm";
                reg = <0x{nvm_offset:x} 0x1000>;
            }};
            circuitpy_partition: partition@100000 {{
                compatible = "zephyr,mapped-partition";
                label = "circuitpy";
                reg = <0x100000 0x100000>;
            }};
            """
        )

    def test_parity_matches(self):
        edt = parse_dts_string(self.rp2040_layout(0xFF000))
        assert check_parity(edt, "raspberrypi/raspberry_pi_pico") == []

    def test_parity_reports_moved_nvm(self):
        edt = parse_dts_string(self.rp2040_layout(0xFE000))
        problems = check_parity(edt, "raspberrypi/raspberry_pi_pico")
        assert len(problems) == 1
        assert "nvm_partition" in problems[0]
        assert "0xfe000" in problems[0] and "0xff000" in problems[0]

    def test_parity_reports_missing_partition(self):
        edt = parse_dts_string(
            xip_flash(
                """
            code_partition: partition@100 {
                compatible = "zephyr,mapped-partition";
                reg = <0x100 0xfdf00>;
            };
            """
            )
        )
        problems = check_parity(edt, "raspberrypi/raspberry_pi_pico")
        assert any(p.startswith("nvm_partition: missing") for p in problems)
        assert any(p.startswith("circuitpy_partition: missing") for p in problems)
