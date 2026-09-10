# Stage 4 - Multimedia - LJSpeechTools implementation
add_library(elizaos-ljspeechtools STATIC
    src/ljspeechtools.cpp
)

set(ELIZAOS_LJSPEECHTOOLS_CXX_STANDARD "${CMAKE_CXX_STANDARD}" CACHE STRING
    "C++ standard used by the focused LJSpeechTools targets (17 or 20)")
set_property(CACHE ELIZAOS_LJSPEECHTOOLS_CXX_STANDARD PROPERTY STRINGS 17 20)
if(NOT ELIZAOS_LJSPEECHTOOLS_CXX_STANDARD MATCHES "^(17|20)$")
    message(FATAL_ERROR "ELIZAOS_LJSPEECHTOOLS_CXX_STANDARD must be 17 or 20")
endif()
set_property(TARGET elizaos-ljspeechtools PROPERTY
    CXX_STANDARD ${ELIZAOS_LJSPEECHTOOLS_CXX_STANDARD})
set_property(TARGET elizaos-ljspeechtools PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET elizaos-ljspeechtools PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(elizaos-ljspeechtools PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
if(MSVC)
    target_compile_options(elizaos-ljspeechtools PRIVATE /WX)
else()
    target_compile_options(elizaos-ljspeechtools PRIVATE -Werror)
endif()

# Try to find audio processing libraries.
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SNDFILE QUIET sndfile)
endif()

target_link_libraries(elizaos-ljspeechtools
    elizaos-core
    elizaos-agentlogger
)

# Link with sndfile if found for broader real audio I/O.
if(SNDFILE_FOUND)
    target_link_libraries(elizaos-ljspeechtools ${SNDFILE_LIBRARIES})
    target_include_directories(elizaos-ljspeechtools PRIVATE ${SNDFILE_INCLUDE_DIRS})
    target_compile_definitions(elizaos-ljspeechtools PRIVATE HAVE_SNDFILE)
    message(STATUS "LJSpeechTools: Using libsndfile for audio I/O")
else()
    message(STATUS "LJSpeechTools: Using dependency-free PCM WAV implementation (libsndfile not found)")
endif()

# Future integrations must remain explicit rather than simulated.
# - FLAC, OGG, MP3 support without libsndfile
# - Speech recognition backend (must be injected explicitly)
# - Real TTS engine
