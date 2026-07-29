/*
 * Wrapper: includes the original PS/2 mouse driver from the
 * kb_zmk_ps2_mouse_trackpoint_driver module.
 *
 * The original module compiles input_mouse_ps2.c only when
 * CONFIG_ZMK_INPUT_MOUSE_PS2=y, which is blocked on peripheral
 * by `depends on (!ZMK_SPLIT || ZMK_SPLIT_ROLE_CENTRAL)`.
 *
 * This wrapper bypasses the restriction by compiling the driver
 * under our CONFIG_ZMK_INPUT_MOUSE_PS2_PERIPHERAL config.
 *
 * We use a relative path because during ZMK module builds, all
 * west modules are cloned as sibling directories.
 */

/* Prevent the original file's DEVICE_DT_INST_DEFINE from being defined
 * twice (once here and once in the original module, if both configs are
 * enabled on central). The original module already compiles it on central.
 */
#if !IS_ENABLED(CONFIG_ZMK_INPUT_MOUSE_PS2)
#include "../../kb_zmk_ps2_mouse_trackpoint_driver/src/drivers/input/input_mouse_ps2.c"
#endif
