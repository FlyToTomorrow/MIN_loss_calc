cmake_minimum_required(VERSION 3.14)
project(switch_fabric)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(switch_fabric
        src/BasicElements.cpp
        src/BasicElements.h
        src/FODON.cpp
        src/FODON.h
        src/Main.cpp
        src/StageSwitch.cpp
        src/StageSwitch.h src/Main.h)
