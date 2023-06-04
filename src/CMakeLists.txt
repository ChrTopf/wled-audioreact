cmake_minimum_required(VERSION 3.23)
project(wled-audioreact)
include(FetchContent)

set(CMAKE_CXX_STANDARD 23)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(wled-audioreact main.cpp Log.cpp Config.cpp NetworkHandler.cpp WLEDSocket.cpp)

target_link_libraries(wled-audioreact PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(wled-audioreact PRIVATE sfml)
