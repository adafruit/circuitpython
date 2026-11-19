// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Descriptor-level invalidation, shared by OnDiskBitmap. See ImageRoot.h.

#include "shared-module/displayio/lvgl/ImageRoot.h"

#if CIRCUITPY_DISPLAYIO_LVGL

void displayio_lvgl_image_root_add(displayio_lvgl_image_root_t **roots, lv_obj_t *obj) {
    for (displayio_lvgl_image_root_t *root = *roots; root != NULL; root = root->next) {
        if (root->obj == obj) {
            return;
        }
    }
    displayio_lvgl_image_root_t *root = lv_malloc_zeroed(sizeof(*root));
    if (root == NULL) {
        return;
    }
    root->obj = obj;
    root->next = *roots;
    *roots = root;
}

void displayio_lvgl_image_root_remove(displayio_lvgl_image_root_t **roots, lv_obj_t *obj) {
    displayio_lvgl_image_root_t **link = roots;
    while (*link != NULL) {
        if ((*link)->obj == obj) {
            displayio_lvgl_image_root_t *found = *link;
            *link = found->next;
            lv_free(found);
            return;
        }
        link = &(*link)->next;
    }
}

void displayio_lvgl_image_invalidate_all(displayio_lvgl_image_root_t *roots,
    const lv_image_dsc_t *dsc) {
    if (dsc->data == NULL) {
        return;
    }
    for (displayio_lvgl_image_root_t *root = roots; root != NULL; root = root->next) {
        // A TileGrid mirror is either the image itself or a container holding the images, and
        // both cover exactly the grid's area.
        lv_obj_invalidate(root->obj);
    }
}

#endif  // CIRCUITPY_DISPLAYIO_LVGL
