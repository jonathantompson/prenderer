project(prenderer2)
set(CMAKE_VERBOSE_MAKEFILE root_VERBOSE_MAKEFILE)
cmake_minimum_required(VERSION 2.8.10)

message("*************************************************")
message("************* PRENDERER CMAKELISTS **************")
message("*************************************************")

if(BUILD MATCHES debug)
  message("cmake compilation is in debug mode, target will be prenderer2_d") 
  set(TARGET_NAME prenderer2_d)
else()
  message("cmake compilation is in release mode, target will be prenderer2")
  set(TARGET_NAME prenderer2)
endif()

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# CMAKE MODULES PATH
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# EXTRA LIBS
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
if(APPLE)
    message("Detected Apple")
    SET(EXTRA_LIBS "-framework Cocoa -framework OpenGL -framework IOKit -framework QuartzCore")
elseif(MSVC)
    message( FATAL_ERROR, "Windows cmake is not supported.  Use visual studio." )
endif(APPLE)

set(LIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/MAC_OS_X)

set(JTIL_LIB_DIR ${root_BINARY_DIR}/jtil/)
set(GLFW_LIB_DIR ${LIB_ROOT}/GLFW)
set(ROCKET_LIB_DIR ${LIB_ROOT}/Rocket)
set(ASSIMP_LIB_DIR ${LIB_ROOT}/assimp)
set(FREEIMAGE_LIB_DIR ${LIB_ROOT}/freeimage)
set(FREETYPE_LIB_DIR ${LIB_ROOT}/freetype)

find_library(GLFW_LIB_OBJ NAMES glfw3 HINTS ${GLFW_LIB_DIR})
find_library(ROCKET_CONTROLS_LIB_OBJ NAMES RocketControls HINTS ${ROCKET_LIB_DIR})
find_library(ROCKET_CORE_LIB_OBJ NAMES RocketCore HINTS ${ROCKET_LIB_DIR})
find_library(ROCKET_DEBUGGER_LIB_OBJ NAMES RocketDebugger HINTS ${ROCKET_LIB_DIR})
find_library(ASSIMP_LIB_OBJ NAMES assimp HINTS ${ASSIMP_LIB_DIR})
find_library(FREEIMAGE_LIB_OBJ NAMES freeimage HINTS ${FREEIMAGE_LIB_DIR})
find_library(FREETYPE_LIB_OBJ NAMES freetype HINTS ${FREETYPE_LIB_DIR})

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# INCLUDE DIRECTORIES
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#

set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../include/MAC_OS_X)

set(JTIL_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../jtil/include/)

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# COMPILER FLAGS AND PREDEFINES
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++#

# check the c and C++ compiler versions
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
    message(STATUS "gcc version >= 4.7")
else()
    message( FATAL_ERROR, "GCC version 4.7 or greater is required" )
    message( "on mac you need 'sudo port install gcc49' and add 'export CXX=/opt/local/bin/g++-mp-4.9'")
endif()

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GXX_VERSION)
if (GXX_VERSION VERSION_GREATER 4.7 OR GXX_VERSION VERSION_EQUAL 4.7)
    message(STATUS "g++ Version >= 4.7")
else()
    message( FATAL_ERROR, "GCC version 4.7 or greater is required" )
    message( "on mac you need 'sudo port install gcc49' and add 'export CC=/opt/local/bin/gcc-mp-4.9'")
endif()

# -g adds debugging symbols
# -Wall turns on all warnings
# -Wextra turns on a lot of warnings (but not too pedantic)
add_definitions(-DGLFW_INCLUDE_GLCOREARB)
add_definitions(-DASSIMP_BUILD_BOOST_WORKAROUND)

# Use: "cmake -DCMAKE_BUILD_TYPE=Debug" for debug
if(BUILD MATCHES debug)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter -std=c++11")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter")
    message(STATUS "adding debug flags")
    add_definitions(-DDEBUG)
    add_definitions(-D_DEBUG)
    add_definitions(-DBREAK_ON_EXCEPTION)
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter -std=c++11 -O3 -msse -msse2")
    SET(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS_RELEASE} -fopenmp -g -Wextra -Wno-ignored-qualifiers -Wno-unused-parameter -O3 -msse -msse2")
    message(STATUS "adding release flags")
endif()

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# PRENDERER SOURCE AND HEADERS
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
set(ROOT_HEADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/headers/")
set(ROOT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# PRENDERER TARGET
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#

file(GLOB_RECURSE PRENDERER_CXX_SOURCE ${ROOT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE PRENDERER_CCC_SOURCE ${ROOT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE PRENDERER_HEADER ${ROOT_HEADER_DIR}/*.h)

if(root_PRINT_SOURCE_FILES MATCHES true)
    message( "SOURCE C++ FILES:" )
    message( STATUS ${PRENDERER_CXX_SOURCE} )
    message( "SOURCE C FILES:" )
    message( STATUS ${PRENDERER_CC_SOURCE} )
    message( "HEADER FILES:" )
    message( STATUS ${PRENDERER_HEADER} )
endif()

message( "INCLUDE DIRECTORIES:" )
message( STATUS "jtil: " ${JTIL_INC_DIR} )

include_directories(
    ${ROOT_HEADER_DIR} 
    ${JTIL_INC_DIR}
)

message( "LIBRARY OBJECTS:" )
message( STATUS "glfw: " ${GLFW_LIB_OBJ})
message( STATUS "RocketControls: " ${ROCKET_CONTROLS_LIB_OBJ})
message( STATUS "RocketCore: " ${ROCKET_CORE_LIB_OBJ})
message( STATUS "RocketDebugger: " ${ROCKET_DEBUGGER_LIB_OBJ})
message( STATUS "assimp: " ${ASSIMP_LIB_OBJ})
message( STATUS "freeimage: " ${FREEIMAGE_LIB_OBJ})
message( STATUS "fretype: " ${FREETYPE_LIB_OBJ})

message( "EXTRA_LIBS to be linked in: " ${EXTRA_LIBS} )

add_executable(${PROJECT_NAME} ${PRENDERER_CXX_SOURCE} ${PRENDERER_CC_SOURCE} ${PRENDERER_HEADER})

target_link_libraries(${PROJECT_NAME} ${GLFW_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${ROCKET_CONTROLS_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${ROCKET_CORE_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${ROCKET_DEBUGGER_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${FREEIMAGE_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIB_OBJ})
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS})
if(BUILD MATCHES debug)
  target_link_libraries(${PROJECT_NAME} jtil_d)
else()
  target_link_libraries(${PROJECT_NAME} jtil)
endif()
