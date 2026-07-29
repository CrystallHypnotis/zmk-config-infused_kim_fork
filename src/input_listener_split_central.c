/*
 * Copyright (c) 2020 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * Central-side split input listener for mouse events forwarded from peripheral.
 *
 * Receives input events (REL_X, REL_Y, BTN_*) forwarded via ZMK_INPUT_SPLIT
 * from the peripheral half, accumulates them, and generates HID mouse reports
 * via zmk_hid_mouse_* and zmk_endpoints_send_mouse_report().
 */

#define DT_DRV_COMPAT zmk_input_listener_split_central

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <zmk/endpoints.h>
#include <zmk/mouse/types.h>
#include <zmk/mouse/hid.h>

#define ONE_IF_DEV_OK(n)                                                                           \
    COND_CODE_1(DT_NODE_HAS_STATUS(DT_INST_PHANDLE(n, device), okay), (1 +), (0 +))

#define VALID_LISTENER_COUNT (DT_INST_FOREACH_STATUS_OKAY(ONE_IF_DEV_OK) 0)

#if VALID_LISTENER_COUNT > 0

enum input_listener_xy_data_mode {
    INPUT_LISTENER_XY_DATA_MODE_NONE,
    INPUT_LISTENER_XY_DATA_MODE_REL,
};

struct input_listener_xy_data {
    enum input_listener_xy_data_mode mode;
    int16_t x;
    int16_t y;
};

struct input_listener_split_central_data {
    const struct device *dev;

    struct {
        struct input_listener_xy_data data;
        struct input_listener_xy_data wheel_data;
        uint8_t button_set;
        uint8_t button_clear;
    } mouse;
};

static void clear_xy_data(struct input_listener_xy_data *data) {
    data->x = data->y = 0;
    data->mode = INPUT_LISTENER_XY_DATA_MODE_NONE;
}

static void input_handler_split_central(const struct device *dev_unused, struct input_event *evt,
                                         void *user_data) {
    ARG_UNUSED(dev_unused);
    struct input_listener_split_central_data *data = user_data;

    switch (evt->type) {
    case INPUT_EV_REL:
        switch (evt->code) {
        case INPUT_REL_X:
            data->mouse.data.mode = INPUT_LISTENER_XY_DATA_MODE_REL;
            data->mouse.data.x += evt->value;
            break;
        case INPUT_REL_Y:
            data->mouse.data.mode = INPUT_LISTENER_XY_DATA_MODE_REL;
            data->mouse.data.y += evt->value;
            break;
        case INPUT_REL_WHEEL:
            data->mouse.wheel_data.mode = INPUT_LISTENER_XY_DATA_MODE_REL;
            data->mouse.wheel_data.y += evt->value;
            break;
        case INPUT_REL_HWHEEL:
            data->mouse.wheel_data.mode = INPUT_LISTENER_XY_DATA_MODE_REL;
            data->mouse.wheel_data.x += evt->value;
            break;
        }
        break;

    case INPUT_EV_KEY:
        if (evt->code >= INPUT_BTN_0 && evt->code <= INPUT_BTN_4) {
            int8_t btn = evt->code - INPUT_BTN_0;
            if (evt->value > 0) {
                WRITE_BIT(data->mouse.button_set, btn, 1);
            } else {
                WRITE_BIT(data->mouse.button_clear, btn, 1);
            }
        }
        break;
    }

    if (evt->sync) {
        // Generate HID scroll report
        if (data->mouse.wheel_data.mode == INPUT_LISTENER_XY_DATA_MODE_REL) {
            zmk_hid_mouse_scroll_set(data->mouse.wheel_data.x, data->mouse.wheel_data.y);
        }

        // Generate HID movement report
        if (data->mouse.data.mode == INPUT_LISTENER_XY_DATA_MODE_REL) {
            zmk_hid_mouse_movement_set(data->mouse.data.x, data->mouse.data.y);
        }

        // Generate HID button press reports
        for (int i = 0; i < ZMK_MOUSE_HID_NUM_BUTTONS; i++) {
            if ((data->mouse.button_set & BIT(i)) != 0) {
                zmk_hid_mouse_button_press(i);
            }
        }

        // Generate HID button release reports
        for (int i = 0; i < ZMK_MOUSE_HID_NUM_BUTTONS; i++) {
            if ((data->mouse.button_clear & BIT(i)) != 0) {
                zmk_hid_mouse_button_release(i);
            }
        }

        // Send the HID report to the host
        zmk_endpoints_send_mouse_report();

        // Reset accumulated state for next report
        zmk_hid_mouse_scroll_set(0, 0);
        zmk_hid_mouse_movement_set(0, 0);

        clear_xy_data(&data->mouse.data);
        clear_xy_data(&data->mouse.wheel_data);
        data->mouse.button_set = data->mouse.button_clear = 0;
    }
}

#endif // VALID_LISTENER_COUNT > 0

#define IL_INST(n)                                                                                 \
    COND_CODE_1(                                                                                   \
        DT_NODE_HAS_STATUS(DT_INST_PHANDLE(n, device), okay),                                      \
        (                                                                                          \
            static struct input_listener_split_central_data data_##n = {                           \
                .dev = DEVICE_DT_INST_GET(n),                                                      \
            };                                                                                     \
            INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_INST_PHANDLE(n, device)),                   \
                                   input_handler_split_central, &data_##n);                      \
            static int zmk_input_listener_split_central_init_##n(const struct device *dev) {       \
                return 0;                                                                          \
            }                                                                                      \
            DEVICE_DT_INST_DEFINE(n, &zmk_input_listener_split_central_init_##n,                   \
                                  NULL, &data_##n, NULL,                                           \
                                  POST_KERNEL, CONFIG_APPLICATION_INIT_PRIORITY, NULL);),          \
        ())

DT_INST_FOREACH_STATUS_OKAY(IL_INST)
