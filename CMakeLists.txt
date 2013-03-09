project(photon)
cmake_minimum_required(VERSION 2.8)

include_directories("./include")

set(CMAKE_CXX_FLAGS                "-std=c++11")
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

set(WINDRES "${CMAKE_CURRENT_SOURCE_DIR}/windres/res.rc")

add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCES} ${HEADERS} ${WINDRES})

find_package(LibXml2 REQUIRED)
if(LIBXML2_FOUND)
  include_directories(${LIBXML2_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES})
else(LIBXML2_FOUND)
  message(FATAL_ERROR "libxml2 not found by CMake!!!")
endif(LIBXML2_FOUND)

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
else(GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found by CMake!!!")
endif(GLEW_FOUND)

find_package(PhysFS REQUIRED)
if(PHYSFS_FOUND)
  include_directories(${PHYSFS_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${PHYSFS_LIBRARY})
else(PHYSFS_FOUND)
  message(FATAL_ERROR "Physics FS not found by CMake!!!")
endif(PHYSFS_FOUND)

find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
  include_directories(${SDL2_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
else(SDL2_FOUND)
  message(FATAL_ERROR "SDL 2 not found by CMake!!!")
endif(SDL2_FOUND)

find_package(SOIL REQUIRED)
if(SOIL_FOUND)
  include_directories(${SOIL_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${SOIL_LIBRARIES})
else(SOIL_FOUND)
  message(FATAL_ERROR "SOIL not found by CMake!!!")
endif(SOIL_FOUND)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
else(OPENGL_FOUND)
  message(FATAL_ERROR "OpenGL not found by CMake!!!")
endif(OPENGL_FOUND)

set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0" "1.48" "1.48.0" "1.49" "1.49.0")

find_package(Boost 1.45 COMPONENTS  system filesystem date_time program_options REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY_RELEASE} ${Boost_DATE_TIME_LIBRARY_RELEASE} ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE})
else(Boost_FOUND)
  message(FATAL_ERROR "Boost not found by CMake!!!")
endif(Boost_FOUND)

find_package(Freetype REQUIRED)
if(FREETYPE_FOUND)
  include_directories(${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARY})
else(FREETYPE_FOUND)
  message(FATAL_ERROR "Freetype library not found by CMake!!!")
endif(FREETYPE_FOUND)
