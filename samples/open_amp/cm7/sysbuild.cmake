# SPDX-License-Identifier: Apache-2.0
#
# Copyright 2022 NXP

# Add external project
ExternalZephyrProject_Add(
    APPLICATION cm4
    SOURCE_DIR ${APP_DIR}/../cm4
    BOARD ${SB_CONFIG_OPENAMP_REMOTE_BOARD}
  )

# Add dependencies so that the remote sample will be built first
# This is required because some primary cores need information from the
# remote core's build, such as the output image's LMA
add_dependencies(${DEFAULT_IMAGE} cm4)
sysbuild_add_dependencies(CONFIGURE ${DEFAULT_IMAGE} cm4)

if(SB_CONFIG_BOOTLOADER_MCUBOOT)
  # Make sure MCUboot is flashed first
  sysbuild_add_dependencies(FLASH cm4 mcuboot)
endif()
