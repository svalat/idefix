# Locate the twin-checker library.
#
# Defines the following variables:
#
#   TWINCHECKER_FOUND - Found the twin-checker framework
#   TWINCHECKER_INCLUDE_DIRS - Include directories
#
# Also defines the library variables below as normal
# variables.  These contain debug/optimized keywords when
# a debugging library is found.
#
#   TWINCHECKER_LIBRARIES - libtwin-checker
#
# Accepts the following variables as input:
#
#   TwinChecker_ROOT - (as a CMake or environment variable)
#                The root directory of the fftw install prefix
#-----------------------------------------------
# Example Usage:
#
#    find_package(TwinChecker REQUIRED)
#    include_directories(${TWINCHECKER_INCLUDE_DIRS})
#
#    add_executable(foo foo.cc)
#    target_link_libraries(foo ${TWINCHECKER_LIBRARIES})
#
#-----------------------------------------------

# search for .h file
find_path(
  TWINCHECKER_INCLUDE_DIRS
  NAMES twin-checker/CheckerApi.h
  DOC "twin-checker include dir"
)
mark_as_advanced(TWINCHECKER_INCLUDE_DIRS)

# search lib
find_library(
  TWINCHECKER_LIBRARIES
  NAMES twin-checker
  DOC "twin-checker dynamic library"
)

# set final variable for libs
mark_as_advanced(TWINCHECKER_LIBRARIES)

# handle
include( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( TwinChecker DEFAULT_MSG TWINCHECKER_LIBRARIES TWINCHECKER_INCLUDE_DIRS)

# error message if not found
if(NOT TWINCHECKER_FOUND)
  message( STATUS "FindTwinChecker looked for twin-checker" )
endif()
