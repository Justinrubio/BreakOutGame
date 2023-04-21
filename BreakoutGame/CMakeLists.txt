cmake_minimum_required(VERSION 3.21)
project(BreakoutGame.cpp)

set(CMAKE_CXX_STANDARD 17)

set(SFML_INCLUDE_DIR "libraries/SFML-2.5.1-windows-vc15-32-bit/SFML-2.5.1/include")
set(SFML_INCLUDE_DIR "libraries/SFML-2.5.1-windows-vc15-32-bit/SFML-2.5.1/lib")

find_package(SFML 2.5.1 COMPONENTS system window graphics network audio)

add_executable(HelloSFML BreakoutGame.cpp breakout_defs.h)
target_link_libraries(HelloSFML sfml-graphics sfml-window sfml-system)