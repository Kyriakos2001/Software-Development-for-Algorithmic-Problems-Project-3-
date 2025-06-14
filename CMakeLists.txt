# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)

project( polyg )

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall")



# CGAL and its components
find_package( CGAL QUIET COMPONENTS Qt5 )

if ( NOT CGAL_FOUND )
  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  
endif()


# Boost and its components
find_package( Boost REQUIRED COMPONENTS system filesystem )

if ( NOT Boost_FOUND )
  message(STATUS "This project requires the Boost library, and will not be compiled.")
  return()  
endif()

# include for local directory
add_subdirectory(includes)

list(APPEND EXTRA_LIBS utils)
list(APPEND EXTRA_LIBS steiner_strategies)
list(APPEND EXTRA_LIBS json_loader)
list(APPEND EXTRA_LIBS json_exporter)
list(APPEND EXTRA_LIBS ant_colony_structures)
# include for local package


# Creating entries for target: polyg
# ############################

add_executable( polyg main.cpp )

# Link the executable to CGAL and third-party libraries
if (CGAL_Qt5_FOUND)
  add_definitions(-DCGAL_USE_BASIC_VIEWER)  
  target_link_libraries(polyg PRIVATE CGAL::CGAL ${EXTRA_LIBS} CGAL::CGAL_Qt5)
endif()


