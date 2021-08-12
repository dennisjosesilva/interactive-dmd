project(interactive-dmd)

# CMAKE MINIMUM VERSION
cmake_minimum_required(VERSION 3.10)

# INCLUDE DIRECTORIES
include_directories(include)

# CXX STANDARD
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QT CONFIG
find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

# CONAN CONFIG
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# SOURCES
file(GLOB_RECURSE PROJECT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# MOC SOURCE
qt5_wrap_cpp(PROJECT_SOURCE 
  include/MainWindow.hpp
  include/MainWidget.hpp)

# EXECUTABLES
add_executable(interactive-dmd ${PROJECT_SOURCE})
target_link_libraries(interactive-dmd PUBLIC
  ${CONAN_LIBS}
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  cgraph
  gvc)