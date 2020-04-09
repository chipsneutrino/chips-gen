#---Setup the project
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(chips-gen CXX)

#---Build Configuration
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE off)

#---Default to a 'release' build with O2 optimisations
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#---Add the cosmicGen executable and link to all the required libraries
add_executable(cosmicGen src/cosmicGen.cc)
target_link_libraries(cosmicGen $ENV{CRYDIR}/lib/libCRY.a)