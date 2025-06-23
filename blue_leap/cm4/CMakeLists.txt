# M4 Application CMakeLists.txt
cmake_minimum_required(VERSION 3.20.0)

# Set Zephyr base if building standalone
if(NOT DEFINED ZEPHYR_BASE)
    set(ZEPHYR_BASE ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/zephyr/zephyr)
endif()

# Explicitly set the project configuration file
set(CONF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/prj.conf)

find_package(Zephyr REQUIRED HINTS ${ZEPHYR_BASE})
project(transformer_gateway_m4)

# Include the ISW library
target_link_libraries(app PRIVATE bl_isw)

# Add M4 specific source files
target_sources(app PRIVATE
    src/main.c
)

# Include directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../app_isw/src
)

# Include third-party module paths if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/open-amp)
    target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/open-amp/lib/include
    )
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/libmetal)
    target_include_directories(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_parties/libmetal/lib/include
    )
endif()

# Compile definitions for M4
target_compile_definitions(app PRIVATE
    CONFIG_CORE_M4
    CONFIG_TRANSFORMER_GATEWAY_M4
)

# Link with required libraries for M4 features
if(CONFIG_OPENAMP)
    target_link_libraries(app PRIVATE openamp)
endif()

# Performance optimizations for M4
if(CONFIG_SPEED_OPTIMIZATIONS)
    target_compile_options(app PRIVATE -O2)
endif()