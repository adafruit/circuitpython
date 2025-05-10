#!/usr/bin/env python3
"""
Test script for board configuration generator
"""

import unittest
import tempfile
import os
import shutil
from pathlib import Path
import generate_board_config  # Import the generator module


class TestBoardConfigGenerator(unittest.TestCase):
    def setUp(self):
        # Create a temporary directory
        self.test_dir = tempfile.mkdtemp()
        self.board_dir = os.path.join(self.test_dir, "test_board")
        os.makedirs(self.board_dir)

        # Create a sample board_setting.toml
        self.toml_content = """\
[metadata]
name = "Test Board"
vendor = "Test Vendor"
url = "https://test.com"
mcu = "ESP32S3"

[usb]
vid = "0x303a"
pid = "0x81B5"
product = "Test Product"
manufacturer = "Test Manufacturer"

[build]
idf_target = "esp32s3"
flash_mode = "qio"
flash_freq = "80m"
flash_size = "4MB"
psram_size = "2MB"
psram_mode = "qio"
psram_freq = "80m"

[features]
circuitpy_esp_camera = false
circuitpy_bitmapfilter = false
circuitpy_codeop = false
circuitpy_paralleldisplaybus = false

[frozen]
libraries = ["Adafruit_CircuitPython_NeoPixel"]

[pins]
button = {number = 0, capabilities = ["digital"]}
neopixel = {number = 21, capabilities = ["digital"]}

[default_interfaces]
uart_rx = 44
uart_tx = 43
"""
        with open(os.path.join(self.board_dir, "board_setting.toml"), "w") as f:
            f.write(self.toml_content)

    def test_generate_config_files(self):
        # Run the generator
        generate_board_config.generate_config_files(self.board_dir)

        # Verify files were created
        expected_files = ["mpconfigboard.h", "mpconfigboard.mk", "pins.c", "board.c"]

        for file in expected_files:
            path = os.path.join(self.board_dir, file)
            self.assertTrue(os.path.exists(path), f"{file} was not generated")

            # Basic content verification
            with open(path) as f:
                content = f.read()
                self.assertIn("Test Board", content)
                self.assertIn("ESP32S3", content)

    def tearDown(self):
        # Clean up
        shutil.rmtree(self.test_dir)


if __name__ == "__main__":
    unittest.main()
