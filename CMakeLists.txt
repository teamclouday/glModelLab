cmake_minimum_required(VERSION 3.12.0)
project(ModelLoader VERSION 1.0.0 LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2")

if(WIN32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mconsole")
endif(WIN32)

add_compile_definitions(GLEW_STATIC)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
SET(MY_SRC_DIR "${CMAKE_SOURCE_DIR}/src")


# prepare the source files
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/externel/externel.zip)
execute_process(
    COMMAND git pull origin master
)
endif()
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/externel/glew)
execute_process (
    COMMAND unzip ${CMAKE_SOURCE_DIR}/externel/externel.zip -d ${CMAKE_SOURCE_DIR}/externel
)
endif()

if(WIN32)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/SDL2.dll)
execute_process (
    COMMAND cp ${CMAKE_SOURCE_DIR}/externel/SDL2/bin/SDL2.dll ${CMAKE_SOURCE_DIR}/SDL2.dll
)
endif()
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/libassimp.dll)
execute_process (
    COMMAND cp ${CMAKE_SOURCE_DIR}/externel/assimp/bin/libassimp.dll ${CMAKE_SOURCE_DIR}/libassimp.dll
)
endif()
endif(WIN32)

FILE(GLOB_RECURSE SRC_FILES "${MY_SRC_DIR}/*.cpp")
FILE(GLOB_RECURSE SRC_IMGUI "${CMAKE_SOURCE_DIR}/externel/imgui/src/*.cpp")

if(WIN32)
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/externel/glew/include"
    "${CMAKE_SOURCE_DIR}/externel/glm/include"
    "${CMAKE_SOURCE_DIR}/externel/SDL2/include"
    "${CMAKE_SOURCE_DIR}/externel/imgui/include"
    "${CMAKE_SOURCE_DIR}/externel/assimp/include"
    "${CMAKE_SOURCE_DIR}/externel/SOIL/include"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/externel/glew/lib"
    "${CMAKE_SOURCE_DIR}/externel/SDL2/lib"
    "${CMAKE_SOURCE_DIR}/externel/assimp/lib"
    "${CMAKE_SOURCE_DIR}/externel/SOIL/lib"
)
endif(WIN32)

find_package(OpenGL REQUIRED)

if(UNIX)
include_directories(
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/externel/imgui/include"
)
# setup glew
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
else()
    message("glew is not found on this machine")
endif()
# setup SDL2
FIND_PATH(SDL2_INCLUDE_DIRS SDL.h PATH_SUFFIXES SDL2)
FIND_LIBRARY(SDL2_LIBRARIES SDL2)
if(SDL2_INCLUDE_DIR_NOTFOUND)
    message("SDL2 is not found on this machine")
else()
    include_directories(${SDL2_INCLUDE_DIRS})
endif()
# setup assimp
find_package(ASSIMP REQUIRED)
if(ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIRS})
else()
    message("Assimp is not found on this machine")
endif()
# setup soil
FIND_PATH(SOIL_INCLUDE_DIRS SOIL.h PATH_SUFFIXES SOIL)
FIND_LIBRARY(SOIL_LIBRARIES SOIL)
if(SOIL_INCLUDE_DIRS_NOTFOUND)
    message("SOIL is not found on this machine")
else()
    include_directories(${SOIL_INCLUDE_DIRS})
endif()
endif(UNIX)

add_executable(mdlab ${SRC_FILES} ${SRC_IMGUI})

if(WIN32)
target_link_libraries(mdlab
    mingw32
    SDL2main
    message(${SDL2_INCLUDE_DIRS})
    SDL2
    glew32s
    assimp
    SOIL
    ${OPENGL_LIBRARIES}
)
endif(WIN32)

if(UNIX)
target_link_libraries(mdlab
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${SOIL_LIBRARIES}
    ${OPENGL_LIBRARIES}
)
endif(UNIX)