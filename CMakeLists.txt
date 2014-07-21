project(photon)
cmake_minimum_required(VERSION 2.8)

include_directories("./include")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1) # is 1 during alpha.
set(VERSION_PATCH 5) # 5 for alpha5

set(PHOTON_VERSION "alpha5")

if(WIN32)
    set(WITH_GLEW ON)
else(WIN32)
    option(WITH_GLEW "Use GLEW to load OpenGL functions." OFF)
endif(WIN32)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # add any future MSVC related stuff here.
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

# files not needed in build, but included so they show up in IDE.
file(GLOB HEADERS "include/*.h")
file(GLOB GLSL_SOURCES "data/shaders/*.xml" "data/shaders/*.vert" "data/shaders/*.frag")
file(GLOB LUA_SOURCES "data/*.lua")

file(GLOB SOURCES "src/*.cpp")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/core/version.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY)
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/version.cpp")

if(WIN32)
    if(${CMAKE_GENERATOR} MATCHES "Visual Studio*")
        add_definitions(-DPHOTON_MSVC_CONFIGURATION="$(ConfigurationName)")
    endif()

    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/windres/res.rc.in" "${CMAKE_CURRENT_BINARY_DIR}/res.rc" @ONLY)
    set(WINDRES "${CMAKE_CURRENT_BINARY_DIR}/res.rc")
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${GLSL_SOURCES} ${LUA_SOURCES} ${HEADERS} ${WINDRES})

file(GLOB CORE_SOURCES "src/core/*.cpp")
add_library(${PROJECT_NAME}_core STATIC ${CORE_SOURCES})

file(GLOB CONFIG_SOURCES "src/config/*.cpp")
add_library(${PROJECT_NAME}_config STATIC ${CONFIG_SOURCES})

file(GLOB GAME_SOURCES "src/game/*.cpp")
add_library(${PROJECT_NAME}_game STATIC ${GAME_SOURCES})

file(GLOB OPENGL_SOURCES "src/opengl/*.cpp")
add_library(${PROJECT_NAME}_opengl STATIC ${OPENGL_SOURCES})

file(GLOB GUI_SOURCES "src/gui/*.cpp")
add_library(${PROJECT_NAME}_gui STATIC ${GUI_SOURCES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_core ${PROJECT_NAME}_config ${PROJECT_NAME}_game ${PROJECT_NAME}_opengl ${PROJECT_NAME}_gui)

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LIBXML2_LIBRARIES})

if(WITH_GLEW)
    add_definitions(-DPHOTON_WITH_GLEW)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
endif(WITH_GLEW)

find_package(PhysFS REQUIRED)
include_directories(${PHYSFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PHYSFS_LIBRARY})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARY})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARY})

find_package(Lua52 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
