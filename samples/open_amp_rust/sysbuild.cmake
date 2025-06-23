
# SPDX-License-Identifier: Apache-2.0

# Add the CM4 application as an external project
ExternalZephyrProject_Add(
    APPLICATION cm4
    SOURCE_DIR ${APP_DIR}/cm4
    BOARD ${SB_CONFIG_REMOTE_BOARD}
)

# Add a dependency so that the remote sample will be built and flashed first
# This is required because some primary cores need information from the
# remote core's build, such as the VMA addresses of the remote application.
add_dependencies(cm7 cm4)
sysbuild_add_dependencies(CONFIGURE cm7 cm4)

if(SB_CONFIG_BOOTLOADER_MCUBOOT)
  # Make sure MCUboot is flashed first
  sysbuild_add_dependencies(FLASH cm4 mcuboot)
  sysbuild_add_dependencies(FLASH cm7 mcuboot)
endif()
