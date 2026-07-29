# SPDX-License-Identifier: MIT

# Find the PS2 driver module directory at CMake configure time.
# During ZMK module builds, all west modules are available and their
# directories are exposed as ZEPHYR_<MODULE>_MODULE_DIR variables.
set(PS2_MODULE_NAME "kb_zmk_ps2_mouse_trackpoint_driver")
string(TOUPPER ${PS2_MODULE_NAME} PS2_MODULE_NAME_UPPER)
string(REGEX REPLACE "[-.]" "_" PS2_MODULE_VAR_NAME "${PS2_MODULE_NAME_UPPER}")
set(PS2_MODULE_DIR_VAR "ZEPHYR_${PS2_MODULE_VAR_NAME}_MODULE_DIR")

if(DEFINED ${PS2_MODULE_DIR_VAR})
    set(PS2_DRIVER_SRC "${${PS2_MODULE_DIR_VAR}}/src/drivers/input/input_mouse_ps2.c")

    if(EXISTS ${PS2_DRIVER_SRC})
        # Compile the original PS/2 mouse driver when our peripheral config is enabled.
        # This bypasses the Kconfig `depends on (!ZMK_SPLIT || ZMK_SPLIT_ROLE_CENTRAL)`
        # restriction by compiling the same source file under a different config flag.
        target_sources_ifdef(CONFIG_ZMK_INPUT_MOUSE_PS2_PERIPHERAL app PRIVATE ${PS2_DRIVER_SRC})

        # Also add the include path for the driver's header
        zephyr_include_directories(${${PS2_MODULE_DIR_VAR}}/include)
    else()
        message(WARNING "PS2 mouse driver source not found at ${PS2_DRIVER_SRC}")
    endif()
else()
    message(WARNING "PS2 module directory variable ${PS2_MODULE_DIR_VAR} not defined")
endif()

# Peripheral-side input listener
target_sources_ifdef(CONFIG_ZMK_INPUT_LISTENER_PS2_PERIPHERAL app PRIVATE src/input_listener_ps2_peripheral.c)

# Central-side input listener
target_sources_ifdef(CONFIG_ZMK_INPUT_LISTENER_SPLIT_CENTRAL app PRIVATE src/input_listener_split_central.c)
