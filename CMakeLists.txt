cmake_minimum_required(VERSION 3.16)
project(BusinessDashboard VERSION 0.1 LANGUAGES CXX)

# Globale Qt / C++ Einstellungen
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt global suchen, damit src und tests darauf zugreifen können
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Sql LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Sql LinguistTools)

#Plugins
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# 1. Den Quellcode (Hauptprogramm) einbinden
add_subdirectory(src)

# 2. Testing aktivieren und Test-Ordner einbinden
enable_testing()
add_subdirectory(tests)



include(cmake/packaging.cmake)
