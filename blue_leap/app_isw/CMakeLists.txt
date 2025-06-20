# ISW Library CMakeLists.txt
zephyr_library_named(bl_isw)

# Add source files based on the core being built
if(CONFIG_SOC_MIMXRT1166_CM7)
    zephyr_library_sources(
        src/bl_isw_m7.c
        src/bl_zephyr_osal_cfg.c
    )
elseif(CONFIG_SOC_MIMXRT1166_CM4)
    zephyr_library_sources(
        src/bl_isw_m4.c
        src/bl_zephyr_osal_cfg.c
    )
endif()

# Include directories
zephyr_library_include_directories(src)

# Include third-party headers if needed
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/open-amp)
    zephyr_library_include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/open-amp/lib/include
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/libmetal)
    zephyr_library_include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/libmetal/lib/include
    )
endif()

# Compile definitions
zephyr_library_compile_definitions_ifdef(
    CONFIG_SOC_MIMXRT1166_CM7
    CORE_CM7
)

zephyr_library_compile_definitions_ifdef(
    CONFIG_SOC_MIMXRT1166_CM4
    CORE_CM4
)