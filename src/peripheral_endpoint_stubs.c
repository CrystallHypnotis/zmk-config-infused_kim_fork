/*
 * Copyright (c) 2024
 * SPDX-License-Identifier: MIT
 *
 * Weak stub implementations of central-only ZMK functions.
 *
 * When building for a PERIPHERAL split role, the original PS/2 input listener
 * (input_listener_ps2.c from the module) is compiled but references functions
 * that only exist on central: zmk_endpoints_send_mouse_report(),
 * zmk_keymap_layer_activate(), zmk_keymap_layer_deactivate(),
 * zmk_keymap_layer_active().
 *
 * These weak stubs satisfy the linker on peripheral without pulling in
 * central-only code. On central, the strong definitions from ZMK take
 * precedence.
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * zmk_endpoints_send_mouse_report() - no-op on peripheral.
 * Mouse events are forwarded via ZMK_INPUT_SPLIT instead.
 */
void __attribute__((weak)) zmk_endpoints_send_mouse_report(void)
{
	/* No-op: mouse reports are sent via input subsystem on peripheral */
}

/*
 * zmk_keymap_layer_activate(layer, sensor) - accepts 2-arg urob-style call.
 * The original listener uses `zmk_keymap_layer_activate(layer, false)`.
 */
void __attribute__((weak)) zmk_keymap_layer_activate(uint8_t layer, bool sensor)
{
	(void)layer;
	(void)sensor;
	/* No-op: layer toggling is handled by our peripheral listener */
}

/*
 * zmk_keymap_layer_deactivate() - no-op on peripheral.
 */
void __attribute__((weak)) zmk_keymap_layer_deactivate(uint8_t layer)
{
	(void)layer;
}

/*
 * zmk_keymap_layer_active() - always returns false on peripheral.
 */
bool __attribute__((weak)) zmk_keymap_layer_active(uint8_t layer)
{
	(void)layer;
	return false;
}
