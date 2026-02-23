# Zephyr2CP Tests

This directory contains unit tests for the `zephyr2cp.py` module using real device tree parsing and pytest.

## Running Tests

To run all tests:
```bash
cd /home/tannewt/repos/circuitpython/ports/zephyr-cp
make test
```

For verbose output:
```bash
pytest test_zephyr2cp.py -v
```

To run specific test classes:
```bash
pytest test_zephyr2cp.py::TestFindFlashDevices -v
pytest test_zephyr2cp.py::TestFindRAMRegions -v
pytest test_zephyr2cp.py::TestIntegration -v
```

To run a specific test:
```bash
pytest test_zephyr2cp.py::TestFindFlashDevices::test_valid_flash_device -v
```
