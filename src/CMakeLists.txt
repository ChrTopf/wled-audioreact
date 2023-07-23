cmake_minimum_required(VERSION 3.23)
project(wled-audioreact)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)

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
target_link_libraries(wled-audioreact PRIVATE /usr/lib/x86_64-linux-gnu/libportaudio.so)
#target_link_libraries(wled-audioreact PRIVATE /usr/lib/libportaudio.so)
