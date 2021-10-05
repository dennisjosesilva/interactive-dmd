#set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
project(interactive-dmd)

# CMAKE MINIMUM VERSION
cmake_minimum_required(VERSION 3.10)


# CXX STANDARD
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FIND CUDA PACKAGE
find_package(CUDA REQUIRED) 

# INCLUDE DIRECTORIES
include_directories(include "${CUDA_INCLUDE_DIRS}")
link_directories(/usr/local/cuda/lib64)

# SETUP CUDA STANDARD
if (NOT DEFINED CMAKE_CUDA_STANDARD)
  set(CMAKE_CUDA_STANDARD 11)
  set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()


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
  include/MainWidget.hpp
  include/TreeVisualiser/RecNodeButton.hpp
  include/TreeVisualiser/TreeVisualiser.hpp
  include/TreeVisualiser/SimpleImageViewer.hpp
  include/InteractiveSdmd.hpp
  include/CustomWidgets/LabelWithProgressBar.hpp
  include/CustomWidgets/ClosableDockWidget.hpp
  include/CustomWidgets/TitleColorBar.hpp
  include/ManipulateCPs/ManipulateCPs.hpp
  include/ManipulateCPs/CpViewer.hpp
  include/ManipulateCPs/node.hpp
  include/ManipulateCPs/edge.hpp)

# SET RESOURCE FILES
qt5_add_resources(PROJECT_SOURCE resources.qrc)

# EXECUTABLES
add_executable(interactive-dmd ${PROJECT_SOURCE})
target_link_libraries(interactive-dmd PUBLIC
  ${CONAN_LIBS}  
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  cgraph
  gvc
  cudart)
