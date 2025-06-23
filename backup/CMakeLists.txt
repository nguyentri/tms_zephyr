cmake_minimum_required(VERSION 3.20.0)

# Set default Zephyr base if not already defined
if(NOT DEFINED ZEPHYR_BASE)
    set(ZEPHYR_BASE ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/zephyr/zephyr)
endif()

# Multi-image build configuration for MIMXRT1160 dual-core
if(SYSBUILD)
    project(transformer_gateway_sysbuild)

    # Additional module paths for third-party components
    list(APPEND ZEPHYR_EXTRA_MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/zephyr/modules
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/open-amp
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/libmetal
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mbedtls
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mcuboot
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/fatfs
    )

    # Add M4 application first (gets flashed to specific location)
    ExternalZephyrProject_Add(
        APPLICATION app_m4
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blue_leap/app_m4
        BOARD mimxrt1160_evk/mimxrt1166/cm4
    )

    # Add M7 application (main application)
    ExternalZephyrProject_Add(
        APPLICATION app_m7
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blue_leap/app_m7
        BOARD mimxrt1160_evk/mimxrt1166/cm7
    )

    # Set build order - M4 must be built and flashed first
    set_property(GLOBAL APPEND PROPERTY SYSBUILD_IMAGES
        app_m4
        app_m7
    )

    # Configure shared memory and image information exchange
    set(SYSBUILD_OPENAMP_REMOTE_APP_NAME app_m4 CACHE INTERNAL "")

else()
    # Regular single-core build
    find_package(Zephyr REQUIRED HINTS ${ZEPHYR_BASE})
    project(transformer_gateway)

    # Additional module paths for third-party components
    list(APPEND ZEPHYR_EXTRA_MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/zephyr/modules
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/open-amp
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/libmetal
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mbedtls
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mcuboot
        ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/fatfs
    )

    # Include the ISW library
    add_subdirectory(blue_leap/app_isw)

    # For single-core builds, include the appropriate app
    if(BOARD MATCHES "cm7")
        add_subdirectory(blue_leap/app_m7)
    elseif(BOARD MATCHES "cm4")
        add_subdirectory(blue_leap/app_m4)
    else()
        message(FATAL_ERROR "Unknown board configuration")
    endif()
endif()

# Global include directories for third-party modules
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/zephyr/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/open-amp/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/libmetal/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mbedtls/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/mcuboot/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd_parties/fatfs/include
)