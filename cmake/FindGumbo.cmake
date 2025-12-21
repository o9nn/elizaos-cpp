# FindGumbo.cmake - CMake module for finding the Gumbo HTML5 parser library
#
# This module defines:
#  Gumbo_FOUND - System has gumbo-parser
#  Gumbo_INCLUDE_DIRS - The gumbo-parser include directories
#  Gumbo_LIBRARIES - The libraries needed to use gumbo-parser
#  Gumbo_VERSION - The version of gumbo-parser found
#
# You can also set the following variables before calling find_package:
#  Gumbo_ROOT_DIR - The root directory to search for gumbo-parser

# Try pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_GUMBO QUIET gumbo)
    set(Gumbo_VERSION ${PC_GUMBO_VERSION})
endif()

# Find the header file
find_path(Gumbo_INCLUDE_DIR
    NAMES gumbo.h
    HINTS
        ${PC_GUMBO_INCLUDEDIR}
        ${PC_GUMBO_INCLUDE_DIRS}
        ${Gumbo_ROOT_DIR}/include
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
    PATH_SUFFIXES
        gumbo
)

# Find the library
find_library(Gumbo_LIBRARY
    NAMES gumbo
    HINTS
        ${PC_GUMBO_LIBDIR}
        ${PC_GUMBO_LIBRARY_DIRS}
        ${Gumbo_ROOT_DIR}/lib
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib/aarch64-linux-gnu
        /usr/lib/arm-linux-gnueabihf
)

# Handle standard arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gumbo
    REQUIRED_VARS
        Gumbo_LIBRARY
        Gumbo_INCLUDE_DIR
    VERSION_VAR
        Gumbo_VERSION
)

# Set output variables
if(Gumbo_FOUND)
    set(Gumbo_LIBRARIES ${Gumbo_LIBRARY})
    set(Gumbo_INCLUDE_DIRS ${Gumbo_INCLUDE_DIR})
    
    # Create imported target
    if(NOT TARGET Gumbo::Gumbo)
        add_library(Gumbo::Gumbo UNKNOWN IMPORTED)
        set_target_properties(Gumbo::Gumbo PROPERTIES
            IMPORTED_LOCATION "${Gumbo_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${Gumbo_INCLUDE_DIR}"
        )
    endif()
    
    mark_as_advanced(Gumbo_INCLUDE_DIR Gumbo_LIBRARY)
endif()

# Print debug information if requested
if(Gumbo_FIND_DEBUG)
    message(STATUS "Gumbo_FOUND: ${Gumbo_FOUND}")
    message(STATUS "Gumbo_INCLUDE_DIR: ${Gumbo_INCLUDE_DIR}")
    message(STATUS "Gumbo_LIBRARY: ${Gumbo_LIBRARY}")
    message(STATUS "Gumbo_VERSION: ${Gumbo_VERSION}")
endif()
