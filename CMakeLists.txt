cmake_minimum_required(VERSION 3.20.1) # CMAKE Version -> nicht erforderlich, aber sicherer
project (escape) # Projectname
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread" )
set(CMAKE_VERBOSE_MAKEFILE ON)

#######################

add_executable(escape ${CMAKE_SOURCE_DIR}/src/main.cpp src/Object.hpp src/Texsheet.hpp src/Level.hpp src/Bag.hpp src/O_Facility.hpp src/O_Living.hpp src/O_Player.hpp src/O_Enemy.hpp src/O_Item.hpp src/Itempanel.hpp)

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
target_link_libraries(escape sfml-system sfml-window sfml-graphics)



## Auch interessant: CMakeCache.txt (zum debugging, enthält Linkings und so)

