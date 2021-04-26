cmake_minimum_required(VERSION 3.12)
include(CheckCXXCompilerFlag)
project(libfive)

# Properly distinguish between Apple and upstream Clang
cmake_policy(SET CMP0025 NEW)

option(BUILD_STUDIO_APP "Build Studio application" ON)
option(BUILD_GUILE_BINDINGS "Build Guile bindings" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)

option(LIBFIVE_PACKED_OPCODES
    "Tightly pack opcodes (breaks compatibility with older saved f-reps)"
    OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if (LIBFIVE_PACKED_OPCODES)
    message("Using packed opcodes, which breaks compatibility with saved f-reps!")
    add_definitions(-DLIBFIVE_PACKED_OPCODES)
endif()

add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

# Build libfive with ccache if the package is present
set(LIBFIVE_CCACHE_BUILD OFF CACHE BOOL "Set to ON for a ccache enabled build")
if(LIBFIVE_CCACHE_BUILD)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set(LIBFIVE_CCACHE_MAXSIZE "" CACHE STRING "Size of ccache")
      set(LIBFIVE_CCACHE_DIR "" CACHE STRING "Directory to keep ccached data")
      set(LIBFIVE_CCACHE_PARAMS "CCACHE_CPP2=yes CCACHE_HASHDIR=yes"
          CACHE STRING "Parameters to pass through to ccache")

      set(CCACHE_PROGRAM "${LIBFIVE_CCACHE_PARAMS} ${CCACHE_PROGRAM}")
      if (LIBFIVE_CCACHE_MAXSIZE)
        set(CCACHE_PROGRAM "CCACHE_MAXSIZE=${LIBFIVE_CCACHE_MAXSIZE} ${CCACHE_PROGRAM}")
      endif()
      if (LIBFIVE_CCACHE_DIR)
        set(CCACHE_PROGRAM "CCACHE_DIR=${LIBFIVE_CCACHE_DIR} ${CCACHE_PROGRAM}")
      endif()
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
  else()
    message(FATAL_ERROR "Unable to find the program ccache. Set LIBFIVE_CCACHE_BUILD to OFF")
  endif()
endif()

################################################################################

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -fPIC -pedantic -Werror=switch")
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "-O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DEIGEN_NO_DEBUG")
else()
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_CXX_FLAGS "/EHsc /WX /D_USE_MATH_DEFINES /D_SCL_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267 /wd4244 /wd4305")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD/ /DEIGEN_NO_DEBUG /arch:AVX2")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

# Sometimes the flag is not supported (this is the case for Apple Silicon)
check_cxx_compiler_flag("-march=native" MARCH_SUPPORTED)
if(MARCH_SUPPORTED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

# Work around an issue with Boost::Interval on OpenBSD and MinGW on Windows
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD" OR MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_ISOC99")
endif()

################################################################################
# Find all packages here at the top level so we can print debugging info
if(NOT MSVC)
    # These packages have different naming conventions in MSVC vs Linux
    find_package(Boost REQUIRED)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GUILE guile-2.2>=2.2.1)
    if (NOT(GUILE_FOUND))
        pkg_check_modules(GUILE guile-3.0>=3.0.0)
    endif()

    pkg_check_modules(EIGEN REQUIRED eigen3>=3.2.92)
else()
    find_package(eigen3 REQUIRED)
    find_package(boost REQUIRED 1.65.0)
endif()

# These packages have consistent names across OS's
find_package(PNG REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development)

# Both Windows and Linux will attempt to build the Studio UI
if (BUILD_STUDIO_APP)
    find_package(Qt5Core 5.12)
endif()

if (UNIX AND NOT(APPLE))
    find_package(Threads REQUIRED)
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
endif(UNIX AND NOT(APPLE))

################################################################################
# Inform the user which subsystems will be built and any missing deps
message("Checking dependencies:")
message("  libfive:		✓")

if (BUILD_GUILE_BINDINGS)
  if (GUILE_FOUND)
    message("  Guile bindings:	✓")
  else ()
    message("  Guile bindings:	✘   (needs Guile 2.2 or later)")
  endif()
else()
    message("  Guile bindings:	✘   (skipping)")
endif()

if (BUILD_PYTHON_BINDINGS)
  if (Python3_FOUND)
    message("  Python bindings:	✓")
  else()
    message("  Python bindings:	✘   (Python 3 not found)")
  endif()
else()
  message("  Python bindings:	✘   (skipping)")
endif()

if (BUILD_STUDIO_APP)
  if (GUILE_FOUND AND BUILD_GUILE_BINDINGS)
    set(STUDIO_WITH_GUILE ON)
  else()
    set(STUDIO_WITH_GUILE OFF)
  endif()

  if (Python3_FOUND AND BUILD_PYTHON_BINDINGS)
    set(STUDIO_WITH_PYTHON ON)
  else()
    set(STUDIO_WITH_PYTHON OFF)
  endif()

  if (Qt5Core_FOUND AND STUDIO_WITH_GUILE AND STUDIO_WITH_PYTHON)
    message("  Studio:		✓   (Python + Guile)")
  elseif (Qt5Core_FOUND AND STUDIO_WITH_GUILE)
    message("  Studio:		✓   (Guile only)")
  elseif (Qt5Core_FOUND AND STUDIO_WITH_PYTHON)
    message("  Studio:		✓   (Python only)")
  else ()
    if (Qt5Core_FOUND)
      message("  Studio:		✘   (needs Guile or Python)")
    elseif (STUDIO_WITH_GUILE OR STUDIO_WITH_PYTHON)
      message("  Studio:		✘   (needs Qt 5.12 or later)")
    else()
      message("  Studio:		✘   (needs Qt 5.12 or later, and Guile or Python")
    endif()
  endif()
else()
    message("  Studio:		✘   (skipping)")
endif()

################################################################################

# Set a flag to detect the case where users run CMake in the wrong directory
set(LIBFIVE_BUILD_FROM_ROOT true)

# Always build the kernel and test suite
add_subdirectory(libfive)

if(BUILD_STUDIO_APP AND (STUDIO_WITH_GUILE OR STUDIO_WITH_PYTHON) AND Qt5Core_FOUND)
    add_subdirectory(studio)
endif(BUILD_STUDIO_APP AND (STUDIO_WITH_GUILE OR STUDIO_WITH_PYTHON) AND Qt5Core_FOUND)
