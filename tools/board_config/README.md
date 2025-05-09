# CircuitPython Board Configuration System

This system allows board configurations to be defined using a single `board_setting.toml` file,
which is then automatically converted into the required C and Makefile configurations.

## File Structure

- `board_setting.toml` - Main configuration file
- `generate_board_config.py` - Generator script
- `test_board_config.py` - Test script
- `board_template.toml` - Template with all available options

## Usage

1. Create a `board_setting.toml` in your board directory following the template
2. Run the generator:
   ```bash
   python tools/generate_board_config.py ports/<port>/boards/<board_name>
   ```
3. The following files will be generated/updated:
   - `mpconfigboard.h`
   - `mpconfigboard.mk`
   - `pins.c`
   - `board.c`

## GitHub Integration

The `.github/workflows/board_builder.yml` workflow will automatically:
- Detect changes to `board_setting.toml` files
- Generate the configuration files
- Build the firmware
- Run tests
- Upload artifacts

## Testing

To test the configuration generator:
```bash
python tools/test_board_config.py
```

## Adding a New Board

1. Copy `board_template.toml` to `ports/<port>/boards/<new_board>/board_setting.toml`
2. Fill in the configuration values
3. Run the generator script
4. Commit both the TOML and generated files

## Best Practices

- Keep the TOML file well-commented
- Test configurations locally before committing
- Verify all pin definitions match the board schematic
- Update the configuration when making hardware changes