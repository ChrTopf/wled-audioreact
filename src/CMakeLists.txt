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
    #copy the settings file into release output directory
    add_custom_command(
            TARGET wled-audioreact POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/settings.json
            ${CMAKE_CURRENT_BINARY_DIR}/settings.json)
    add_compile_definitions(CONFIG_FILE="settings.json")
    #include standard libraries in the windows release build
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "-static")
    endif()
endif()

#include the header only libraries
target_include_directories(wled-audioreact PRIVATE ../dependencies/)
target_include_directories(wled-audioreact PRIVATE ../dependencies/asio/)

#windows build
if(WIN32)
    add_definitions(-DWINDOWS_BUILD)
    target_include_directories(wled-audioreact PRIVATE win/)
    target_sources(wled-audioreact PRIVATE win/AudioProcessor.cpp)
    target_link_libraries(wled-audioreact PRIVATE ws2_32)
    target_link_libraries(wled-audioreact PRIVATE ole32.dll)
    target_link_libraries(wled-audioreact PRIVATE mmdevapi.dll)
    target_link_libraries(wled-audioreact PRIVATE propsys.dll)
    target_link_libraries(wled-audioreact PRIVATE Kernel32.lib)
    target_link_libraries(wled-audioreact PRIVATE user32.lib)
    target_link_libraries(wled-audioreact PRIVATE winmm.lib)
endif()

#linux build
if(UNIX AND NOT APPLE AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DDEBIAN_BUILD)
    target_include_directories(wled-audioreact PRIVATE linux/)
    target_sources(wled-audioreact PRIVATE linux/AudioProcessor.cpp)
    #comment/uncomment this, depending on your linux distro
    target_link_libraries(wled-audioreact PRIVATE /usr/lib/x86_64-linux-gnu/libportaudio.so)
    #target_link_libraries(wled-audioreact PRIVATE /usr/lib/libportaudio.so)
endif()