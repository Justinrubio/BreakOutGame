cmake_minimum_required(VERSION 3.21)
project(BreakoutGame.cpp)

set(CMAKE_CXX_STANDARD 17)

set(SFML_INCLUDE_DIR "/usr/local/include")
set(SFML_INCLUDE_DIR "/usr/local/lib")

find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)

add_executable(HelloSFML BreakoutGame.cpp breakout_defs.h)
target_link_libraries(HelloSFML sfml-graphics sfml-window sfml-system)