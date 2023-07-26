cmake_minimum_required(VERSION 3.23)
project(wled-audioreact)

set(CMAKE_CXX_STANDARD 23)

#define available build types
set(available_build_types Debug Release)
#check if the selected build type is valid
if(NOT CMAKE_BUILD_TYPE IN_LIST available_build_types)
    message(FATAL_ERROR "Invalid build type! Use one of: ${available_build_types}")
endif()

add_executable(wled-audioreact
        main.cpp
        Log.cpp
        Config.cpp
        NetworkHandler.cpp
        WLEDSocket.cpp
        AudioProcessor.cpp
        SignalController.cpp
        Effect.h
        EffectParameters.h
        effects/Effects.h
        effects/RMSMaxVolumeEffect.cpp
        effects/AverageVolumeEffect.cpp
        effects/MaxVolumeEffect.cpp)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(CONFIG_FILE="../settings.json")
else()
    add_custom_command(
            TARGET wled-audioreact POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/settings.json
            ${CMAKE_CURRENT_BINARY_DIR}/settings.json)
    add_compile_definitions(CONFIG_FILE="settings.json")
endif()

#include the header only libraries
target_include_directories(wled-audioreact PRIVATE ../dependencies/)
target_include_directories(wled-audioreact PRIVATE ../dependencies/asio/)

# Variante für Windows
if(WIN32)
    add_definitions(-DWINDOWS_BUILD)
    target_link_libraries(wled-audioreact PRIVATE ws2_32)
    target_link_libraries(wled-audioreact PRIVATE ${PROJECT_SOURCE_DIR}/../dependencies/portaudio_windows_x64/portaudio_x64.dll)
endif()

# Variante für Debian
if(UNIX AND NOT APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DDEBIAN_BUILD)
    target_link_libraries(wled-audioreact PRIVATE /usr/lib/x86_64-linux-gnu/libportaudio.so)
endif()

# Variante für Manjaro
if(UNIX AND NOT APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_NAME STREQUAL "Manjaro")
    add_definitions(-DMANJARO_BUILD)
    target_link_libraries(wled-audioreact PRIVATE /usr/lib/libportaudio.so)
endif()