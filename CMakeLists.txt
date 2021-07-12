cmake_minimum_required(VERSION 3.10)

project(GuiComps VERSION 1.0)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(GuiComps main.cpp)
configure_file(config.h.in config.h)

target_include_directories(GuiComps PUBLIC ${PROJECT_BINARY_DIR})

find_package (SFML REQUIRED system window graphics network audio)
target_link_libraries(GuiComps sfml-graphics sfml-audio sfml-network)

