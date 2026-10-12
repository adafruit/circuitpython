// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"
#include "py/obj.h"

#include "lib/protomatter/src/core.h"

#include "shared-bindings/rgbmatrix/RowAddressMode.h"

MAKE_ENUM_VALUE(rgbmatrix_row_address_mode_type, rgbmatrix_row_address_mode, BINARY, PROTOMATTER_ROW_ADDRESS_BINARY);
MAKE_ENUM_VALUE(rgbmatrix_row_address_mode_type, rgbmatrix_row_address_mode, ABC, PROTOMATTER_ROW_ADDRESS_ABC);

//| class RowAddressMode:
//|     """The protocol used to select rows on the matrix."""
//|
//|     BINARY: RowAddressMode
//|     """Use parallel binary row addresses."""
//|
//|     ABC: RowAddressMode
//|     """Use serial A, B, and C row selection."""
//|
MAKE_ENUM_MAP(rgbmatrix_row_address_mode) {
    MAKE_ENUM_MAP_ENTRY(rgbmatrix_row_address_mode, BINARY),
    MAKE_ENUM_MAP_ENTRY(rgbmatrix_row_address_mode, ABC),
};
static MP_DEFINE_CONST_DICT(rgbmatrix_row_address_mode_locals_dict, rgbmatrix_row_address_mode_locals_table);

MAKE_PRINTER(rgbmatrix, rgbmatrix_row_address_mode);
MAKE_ENUM_TYPE(rgbmatrix, RowAddressMode, rgbmatrix_row_address_mode);
