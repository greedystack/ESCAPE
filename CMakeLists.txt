cmake_minimum_required(VERSION 3.20.1) # CMAKE Version -> nicht erforderlich, aber sicherer
project (first) # Projectname
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )
set(CMAKE_VERBOSE_MAKEFILE ON)

#######################

add_executable(first ${CMAKE_SOURCE_DIR}/src/main.cpp src/Object.hpp src/Tex.h src/Tex.cpp src/Game.cpp src/Game.h src/Level.h src/Level.cpp)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/include) #HEADER


##############################################
### SFML 
##############################################
# Good hint: https://remy-villulles.medium.com/compile-and-install-sfml-2-5-1-on-macos-with-clion-and-cmake-b4e52199db1c

#######################
### Local SFML from lib
### if this block is commented out a wizard includes SFML from unicornland
#######################
set(SFML_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SFML-2.5.1/include")
set(SFML_LIBRARY_DIR "${CMAKE_SOURCE_DIR}/lib/SFML-2.5.1/lib")
link_directories(SFML_LIBRARY_DIR)
include_directories(SFML_INCLUDE_DIR)
#######################
#######################


find_package(SFML 2.5.1 COMPONENTS network audio graphics window system REQUIRED)


### LINKING:
target_link_libraries(first sfml-system sfml-window sfml-graphics)



## Auch interessant: CMakeCache.txt (zum debugging, enthält Linkings und so)

