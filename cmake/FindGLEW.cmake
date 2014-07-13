include(FindPackageHandleStandardArgs)

find_path( GLEW_INCLUDE_DIR "GL/glew.h" )
find_library( GLEW_LIBRARIES NAMES GLEW glew32
  PATH_SUFFIXES lib64 lib )

find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_LIBRARIES GLEW_INCLUDE_DIR )