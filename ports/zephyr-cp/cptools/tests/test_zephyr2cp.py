import sys
import pathlib
import tempfile

# Add devicetree library to path
portdir = pathlib.Path(__file__).parent.parent.parent
sys.path.append(str(portdir / "zephyr/scripts/dts/python-devicetree/src/"))

from devicetree import dtlib

# Add parent directory to path to import zephyr2cp
sys.path.insert(0, str(pathlib.Path(__file__).parent.parent))

# Mock cpbuild before importing
sys.modules["cpbuild"] = type(sys)("cpbuild")
sys.modules["cpbuild"].run_in_thread = lambda x: x

from zephyr2cp import find_flash_devices, find_ram_regions, BLOCKED_FLASH_COMPAT, MINIMUM_RAM_SIZE


def parse_dts_string(dts_content):
    """
    Parse a device tree string and return the dtlib.DT object.

    Args:
        dts_content: String containing device tree source

    Returns:
        dtlib.DT object with parsed device tree
    """
    with tempfile.NamedTemporaryFile(mode="w", suffix=".dts", delete=False) as f:
        f.write(dts_content)
        f.flush()
        temp_path = f.name

    try:
        dt = dtlib.DT(temp_path)
        return dt
    finally:
        pathlib.Path(temp_path).unlink()


class TestFindFlashDevices:
    """Test suite for find_flash_devices function."""

    def test_no_compatible_returns_empty(self):
        """Test that device tree with no flash devices returns empty list."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    memory@0 {
        reg = <0x0 0x100000>;
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert result == []

    def test_chosen_flash_excluded(self):
        """Test that chosen flash nodes are excluded."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    flash0: flash@0 {
        compatible = "soc-nv-flash";
        reg = <0x0 0x100000>;
    };

    chosen {
        zephyr,flash = &flash0;
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert result == [], "Chosen flash should be excluded"

    def test_blocked_compat_excluded(self):
        """Test that blocked compatible strings are excluded."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    qspi0: qspi@40000 {
        compatible = "renesas,ra-qspi";
        reg = <0x40000 0x1000>;
    };

    spi0: spi@50000 {
        compatible = "nordic,nrf-spim";
        reg = <0x50000 0x1000>;
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert result == [], "Blocked flash controllers should be excluded"

    def test_valid_flash_device(self):
        """Test that valid flash device is detected."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    flash0: flash@0 {
        compatible = "jedec,spi-nor";
        reg = <0x0 0x100000>;
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert len(result) == 1
        assert result[0] == "flash0"

    def test_valid_flash_device_multiple_drivers(self):
        """Test that valid flash device is detected."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    flash0: flash@0 {
        compatible = "other,driver", "jedec,spi-nor";
        reg = <0x0 0x100000>;
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert len(result) == 1
        assert result[0] == "flash0"

    def test_external_flash_not_chosen(self):
        """Test external flash is included when internal is chosen."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    internal_flash: flash@0 {
        compatible = "soc-nv-flash";
        reg = <0x0 0x100000>;
    };

    external_flash: flash@1000000 {
        compatible = "jedec,spi-nor";
        reg = <0x1000000 0x800000>;
    };

    chosen {
        zephyr,flash = &internal_flash;
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)

        # Should only include external flash, not chosen internal flash
        assert len(result) == 1
        assert "external_flash" in result[0]
        assert "internal_flash" not in result[0]

    def test_disabled_flash_excluded(self):
        """Test that disabled flash devices are excluded."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    flash0: flash@0 {
        compatible = "jedec,spi-nor";
        reg = <0x0 0x100000>;
        status = "disabled";
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_flash_devices(dt)
        assert result == [], "Disabled flash should be excluded"


class TestFindRAMRegions:
    """Test suite for find_ram_regions function."""

    def test_no_ram_returns_empty(self):
        """Test that device tree with no RAM returns empty list."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    flash@0 {
        compatible = "soc-nv-flash";
        reg = <0x0 0x100000>;
    };

    chosen {
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_ram_regions(dt)
        assert result == []

    def test_chosen_sram_basic(self):
        """Test chosen sram region is detected correctly."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    sram0: memory@20000000 {
        compatible = "mmio-sram";
        reg = <0x20000000 0x40000>;
    };

    chosen {
        zephyr,sram = &sram0;
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_ram_regions(dt)

        assert len(result) == 1
        label, start, end, size, path = result[0]

        assert label == "sram0"
        assert start == "z_mapped_end"
        assert (
            end
            == "(uint32_t*) (DT_REG_ADDR(DT_NODELABEL(sram0)) + DT_REG_SIZE(DT_NODELABEL(sram0)))"
        )
        assert size == 0x40000

    def test_memory_region_with_custom_name(self):
        """Test memory region with zephyr,memory-region property."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    sram0: memory@20000000 {
        compatible = "mmio-sram";
        reg = <0x20000000 0x40000>;
    };

    reserved_mem: memory@30000000 {
        compatible = "zephyr,memory-region", "mmio-sram";
        reg = <0x30000000 0x10000>;
        zephyr,memory-region = "CUSTOM_REGION";
    };

    chosen {
        zephyr,sram = &sram0;
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_ram_regions(dt)

        # Should have both regions, chosen first
        assert len(result) == 2

        # First should be chosen SRAM
        assert result[0][0] == "sram0"

        # Second should be custom region
        label, start, end, size, path = result[1]
        assert label == "reserved_mem"
        assert start == "__CUSTOM_REGION_end"

    def test_disabled_ram_excluded(self):
        """Test that disabled RAM regions are excluded."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    sram0: memory@20000000 {
        compatible = "mmio-sram";
        reg = <0x20000000 0x40000>;
    };

    sram1: memory@30000000 {
        compatible = "zephyr,memory-region", "mmio-sram";
        reg = <0x30000000 0x10000>;
        status = "disabled";
        zephyr,memory-region = "CUSTOM_REGION";
    };

    chosen {
        zephyr,sram = &sram0;
    };
};
"""
        dt = parse_dts_string(dts)
        result = find_ram_regions(dt)

        # Should only have chosen SRAM, not disabled one
        assert len(result) == 1
        assert result[0][0] == "sram0"


class TestIntegration:
    """Integration tests with realistic device tree configurations."""

    def test_typical_nrf_board_configuration(self):
        """Test typical Nordic nRF board with internal and external flash."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;

    soc {
        #address-cells = <1>;
        #size-cells = <1>;

        flash0: flash@0 {
            compatible = "soc-nv-flash";
            reg = <0x0 0x100000>;
        };

        sram0: memory@20000000 {
            compatible = "mmio-sram";
            reg = <0x20000000 0x40000>;
        };
    };

    external_flash: spi_flash@0 {
        compatible = "jedec,spi-nor";
        reg = <0x0 0x800000>;
    };

    chosen {
        zephyr,flash = &flash0;
        zephyr,sram = &sram0;
    };
};
"""
        dt = parse_dts_string(dts)

        # Test flash detection
        flashes = find_flash_devices(dt)
        assert len(flashes) == 1, "Should find external flash only"
        assert "external_flash" in flashes[0]

        # Test RAM detection
        rams = find_ram_regions(dt)
        assert len(rams) == 1, "Should find chosen SRAM only"
        assert rams[0][0] == "sram0"

    def test_board_with_nrf5340_regions(self):
        """Test that RAM subregions are included with the right addresses."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;
    chosen {
        zephyr,sram = &sram0_image;
    };
    /* node '/soc/peripheral@50000000/qspi@2b000' defined in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:475 */
			qspi: qspi@2b000 {
				compatible = "nordic,nrf-qspi";  /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:476 */
				#address-cells = < 0x1 >;        /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:477 */
				#size-cells = < 0x0 >;           /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:478 */
				reg = < 0x2b000 0x1000 >,
				      < 0x10000000 0x10000000 >; /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:479 */
				reg-names = "qspi",
				            "qspi_mm";           /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:480 */
				interrupts = < 0x2b 0x1 >;       /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_peripherals.dtsi:481 */
				status = "okay";                 /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:118 */

				/* node '/soc/peripheral@50000000/qspi@2b000/mx25r6435f@0' defined in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:123 */
				mx25r64: mx25r6435f@0 {
					compatible = "nordic,qspi-nor";                                                               /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:124 */
					reg = < 0x0 >;                                                                                /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:125 */
					writeoc = "pp4io";                                                                            /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:127 */
					readoc = "read4io";                                                                           /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:129 */
					sck-frequency = < 0x7a1200 >;                                                                 /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:130 */
					jedec-id = [ C2 28 17 ];                                                                      /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:131 */
					sfdp-bfp = [ E5 20 F1 FF FF FF FF 03 44 EB 08 6B 08 3B 04 BB EE FF FF FF FF FF 00 FF FF FF 00
					             FF 0C 20 0F 52 10 D8 00 FF 23 72 F5 00 82 ED 04 CC 44 83 68 44 30 B0 30 B0 F7 C4
					             D5 5C 00 BE 29 FF F0 D0 FF FF ];                                                 /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:132 */
					size = < 0x4000000 >;                                                                         /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:136 */
					has-dpd;                                                                                      /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:137 */
					t-enter-dpd = < 0x2710 >;                                                                     /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:138 */
					t-exit-dpd = < 0x88b8 >;                                                                      /* in zephyr/boards/nordic/nrf5340dk/nrf5340_cpuapp_common.dtsi:139 */
				};
			};

    		/* node '/soc/memory@20000000' defined in zephyr/dts/arm/nordic/nrf5340_cpuapp.dtsi:55 */
		sram0: memory@20000000 {
			compatible = "mmio-sram";            /* in zephyr/dts/arm/nordic/nrf5340_cpuapp.dtsi:56 */
			#address-cells = < 0x1 >;            /* in zephyr/dts/arm/nordic/nrf5340_cpuapp.dtsi:57 */
			#size-cells = < 0x1 >;               /* in zephyr/dts/arm/nordic/nrf5340_cpuapp.dtsi:58 */
			reg = < 0x20000000 0x80000 >;        /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_qkaa.dtsi:15 */
			ranges = < 0x0 0x20000000 0x80000 >; /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_qkaa.dtsi:16 */

			/* node '/soc/memory@20000000/sram@0' defined in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:16 */
			sram0_image: sram@0 {
				reg = < 0x0 0x70000 >;        /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:18 */
				ranges = < 0x0 0x0 0x70000 >; /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:19 */
				#address-cells = < 0x1 >;     /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:20 */
				#size-cells = < 0x1 >;        /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:21 */

				/* node '/soc/memory@20000000/sram@0/sram0_image@0' defined in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:23 */
				sram0_s: sram0_image@0 {
					reg = < 0x0 0x40000 >; /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:25 */
				};
			};

			/* node '/soc/memory@20000000/sram@40000' defined in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:33 */
			sram0_ns: sram@40000 {
				reg = < 0x40000 0x40000 >;        /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:35 */
				ranges = < 0x0 0x40000 0x40000 >; /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:36 */
				#address-cells = < 0x1 >;         /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:37 */
				#size-cells = < 0x1 >;            /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:38 */

				/* node '/soc/memory@20000000/sram@40000/sram0_ns@0' defined in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:40 */
				sram0_ns_app: sram0_ns@0 {
					reg = < 0x0 0x30000 >; /* in zephyr/dts/vendor/nordic/nrf5340_sram_partition.dtsi:42 */
				};
			};

			/* node '/soc/memory@20000000/sram@70000' defined in zephyr/dts/vendor/nordic/nrf5340_shared_sram_partition.dtsi:27 */
			sram0_shared: sram@70000 {
				reg = < 0x70000 0x10000 >; /* in zephyr/dts/vendor/nordic/nrf5340_shared_sram_partition.dtsi:29 */
				phandle = < 0x11 >;        /* in zephyr/dts/arm/nordic/nrf5340_cpuapp_ipc.dtsi:9 */
			};
		};
};
"""
        dt = parse_dts_string(dts)
        flashes = find_flash_devices(dt)
        rams = find_ram_regions(dt)

        # Should only get chosen SRAM
        assert len(rams) == 1
        assert rams[0][0] == "sram0_image"

        assert len(flashes) == 1
        assert flashes[0] == "mx25r64"

    def test_board_with_chosen_memory_region(self):
        """Test that RAM subregions are included with the right addresses."""
        dts = """
/dts-v1/;

/ {
    #address-cells = <1>;
    #size-cells = <1>;
    chosen {
    zephyr,sram = &axisram2;     /* in zephyr/boards/st/nucleo_n657x0_q/nucleo_n657x0_q_common.dtsi:18 */
    };


	/* node '/memory@34000000' defined in zephyr/dts/arm/st/n6/stm32n6.dtsi:42 */
	axisram1: memory@34000000 {
		compatible = "zephyr,memory-region",
		             "mmio-sram";            /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:43 */
		zephyr,memory-region = "AXISRAM1";   /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:44 */
		reg = < 0x34000000 0x200000 >;       /* in zephyr/dts/arm/st/n6/stm32n657X0.dtsi:12 */
	};

	/* node '/memory@34180400' defined in zephyr/dts/arm/st/n6/stm32n6.dtsi:47 */
	axisram2: memory@34180400 {
		compatible = "zephyr,memory-region",
		             "mmio-sram";            /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:48 */
		zephyr,memory-region = "AXISRAM2";   /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:49 */
		reg = < 0x34180400 0x7fc00 >;        /* in zephyr/dts/arm/st/n6/stm32n657X0.dtsi:17 */
	};

	/* node '/soc/ramcfg@42023100' defined in zephyr/dts/arm/st/n6/stm32n6.dtsi:251 */
	ramcfg_sram3_axi: ramcfg@42023100 {
		compatible = "st,stm32n6-ramcfg"; /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:252 */
		#address-cells = < 0x1 >;         /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:253 */
		#size-cells = < 0x1 >;            /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:254 */
		reg = < 0x42023100 0x80 >;        /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:255 */

		/* node '/soc/ramcfg@42023100/memory@34200000' defined in zephyr/dts/arm/st/n6/stm32n6.dtsi:259 */
		axisram3: memory@34200000 {
			compatible = "zephyr,memory-region",
			             "mmio-sram";            /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:260 */
			zephyr,memory-region = "AXISRAM3";   /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:261 */
			zephyr,memory-attr = < 0x100000 >;   /* in zephyr/dts/arm/st/n6/stm32n6.dtsi:262 */
			reg = < 0x34200000 0x70000 >;        /* in zephyr/dts/arm/st/n6/stm32n657X0.dtsi:23 */
			status = "disabled";                 /* in zephyr/dts/arm/st/n6/stm32n657X0.dtsi:24 */
		};
	};
};
"""
        dt = parse_dts_string(dts)
        rams = find_ram_regions(dt)

        print(rams)

        # Should only get chosen SRAM
        assert len(rams) == 2
        assert rams[0][0] == "axisram2"
        assert rams[1][0] == "axisram1"
