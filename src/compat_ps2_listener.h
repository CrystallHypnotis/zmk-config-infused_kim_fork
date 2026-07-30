/*
 * Compatibility layer: zmk_keymap_layer_activate takes 2 args (layer, sensor)
 * in urob's/infused-kim's ZMK fork, but the upstream PS/2 module calls it with
 * 1 arg (layer only). This header wraps the 1-arg calls with NULL sensor.
 */
#pragma once

#include <zmk/keymap.h>

/* Override 1-arg calls to pass NULL sensor */
#define zmk_keymap_layer_activate_1arg zmk_keymap_layer_activate
#undef zmk_keymap_layer_activate
#define zmk_keymap_layer_activate(layer) \
	zmk_keymap_layer_activate_1arg(layer, NULL)
