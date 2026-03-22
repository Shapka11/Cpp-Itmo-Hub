cmake_minimum_required(VERSION 3.14)
project(labwork6)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(cpr REQUIRED)
add_subdirectory(json)
add_subdirectory(lib)
add_subdirectory(bin)

target_link_libraries(labwork6 PRIVATE cpr::cpr nlohmann_json::nlohmann_json)