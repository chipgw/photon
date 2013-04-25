project(photon)
cmake_minimum_required(VERSION 2.8)

include_directories("./include")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -Wall")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp" @ONLY)

# files not needed in build, but included so they show up in IDE.
file(GLOB HEADERS "include/*.h")
file(GLOB GLSL_SOURCES "data/shaders/*.xml" "data/shaders/*.vert" "data/shaders/*.frag")

file(GLOB SOURCES "src/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/windres/res.rc.in" "${CMAKE_CURRENT_SOURCE_DIR}/windres/res.rc" @ONLY)
set(WINDRES "${CMAKE_CURRENT_SOURCE_DIR}/windres/res.rc")

add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCES} ${HEADERS} ${WINDRES})

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})

find_package(PhysFS REQUIRED)
include_directories(${PHYSFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PHYSFS_LIBRARY})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

find_package(SOIL REQUIRED)
include_directories(${SOIL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SOIL_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARY})
