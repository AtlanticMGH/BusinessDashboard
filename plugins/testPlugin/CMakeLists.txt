cmake_minimum_required(VERSION 3.16)
project(testplugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(testplugin MODULE
    testPlugin.cpp
    testPlugin.h
    PluginInterface.h
    testPlugin.json
)

target_link_libraries(testplugin PRIVATE Qt6::Widgets)

# Ausgabe direkt in einen "plugins"-Ordner neben der Hauptanwendung.
set_target_properties(testplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)
