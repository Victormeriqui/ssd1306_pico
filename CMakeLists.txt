# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

set(LIBRARY_NAME "ssd1306_pico")
file(GLOB LIBRARY_SOURCES "src/*.cpp")

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(${LIBRARY_NAME} PRIVATE pico_stdlib hardware_i2c)

# only build the example if this is the top-level project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "loading ssd1306_pico as a self-contained project")

    set(TARGET_NAME "ssd1306_example")
    cmake_minimum_required(VERSION 3.13)

    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    include(pico_sdk_import.cmake)
    project(ssd1306_example CXX C ASM)

    pico_sdk_init()

    set(BUILD_TESTS OFF CACHE BOOL "Disable ETL tests")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)  # Skip try_run for cross-compiling

    include(FetchContent)
    FetchContent_Declare(etl
        GIT_REPOSITORY https://github.com/ETLCPP/etl
        GIT_TAG 20.44.1)
    FetchContent_MakeAvailable(etl)
    target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src  ${etl_SOURCE_DIR}/include)
    target_link_libraries(${LIBRARY_NAME} PRIVATE pico_stdlib hardware_i2c etl::etl)

    add_executable(${TARGET_NAME} example/main.cpp)
    pico_set_program_name(ssd1306_example "ssd1306_example")
    pico_set_program_version(ssd1306_example "1.0")

    target_link_libraries(${TARGET_NAME} PRIVATE ${LIBRARY_NAME})

    pico_add_extra_outputs(ssd1306_example)
else()
    message(STATUS "loading ssd1306_pico as a library")
    target_link_libraries(${LIBRARY_NAME} PUBLIC etl::etl)
endif()