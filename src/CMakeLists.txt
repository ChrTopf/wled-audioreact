cmake_minimum_required(VERSION 3.23)
project(wled-audioreact)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)

#use new cmake policy for downloading sources
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()
set(DOWNLOAD_EXTRACT_TIMESTAMP true)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)
include_directories(/usr/include)

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

target_link_libraries(wled-audioreact PRIVATE nlohmann_json::nlohmann_json)


# Variante für Windows
if(WIN32)
    add_definitions(-DWINDOWS_BUILD)
    #port audio
    target_link_libraries(wled-audioreact PRIVATE C:/Users/ChrTopf/Documents/repos/portaudio/cmake-build-debug/libportaudio.dll)
    #asio
    target_link_libraries(wled-audioreact PRIVATE ws2_32)
    target_include_directories(wled-audioreact PRIVATE C:/Users/ChrTopf/Documents/repos/asio/include)
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