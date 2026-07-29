# SPDX-License-Identifier: MIT

# PS/2 Mouse driver wrapper - compiles the original input_mouse_ps2.c when
# CONFIG_ZMK_INPUT_MOUSE_PS2_PERIPHERAL is enabled (bypasses the
# `depends on (!ZMK_SPLIT || ZMK_SPLIT_ROLE_CENTRAL)` restriction).
target_sources_ifdef(CONFIG_ZMK_INPUT_MOUSE_PS2_PERIPHERAL app PRIVATE src/input_mouse_ps2_wrapper.c)

# Peripheral-side input listener - replaces zmk_hid_* calls with input subsystem calls
target_sources_ifdef(CONFIG_ZMK_INPUT_LISTENER_PS2_PERIPHERAL app PRIVATE src/input_listener_ps2_peripheral.c)

# Central-side input listener - receives split-input events and converts to HID mouse reports
target_sources_ifdef(CONFIG_ZMK_INPUT_LISTENER_SPLIT_CENTRAL app PRIVATE src/input_listener_split_central.c)

# Add include path for the PS2 mouse driver header
zephyr_include_directories(../kb_zmk_ps2_mouse_trackpoint_driver/include)
