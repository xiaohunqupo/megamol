#
# MegaMol™ Console Front End
# Copyright 2015, by MegaMol TEAM
# Alle Rechte vorbehalten. All rights reserved.
#
cmake_minimum_required(VERSION 2.8.12)

project(MegaMolCon)
set(Building_MegaMolCon TRUE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
	message(STATUS "You should always specify a build type! Defaulting build type to 'Release'")
	set(CMAKE_BUILD_TYPE Release)
endif()


# finding main dependencies
find_package(MegaMolCore REQUIRED HINTS ${MegaMolCore_DIR})
message(STATUS "MegaMolCore suggests vislib at: ${MegaMolCore_vislib_DIR}")
message(STATUS "MegaMolCore suggests install prefix: ${MegaMolCore_INSTALL_PREFIX}")
if (USE_MEGAMOLCORE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX ${MegaMolCore_INSTALL_PREFIX})
	message(STATUS "Using MegaMolCore install prefix")
endif()
find_package(vislib REQUIRED HINTS ${MegaMolCore_vislib_DIR})
find_package(AntTweakBar)

# processor word size detection
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BITS 64)
else()
	set(BITS 32)
endif()

# compiler options
set(MegaMolCon_common_definitions "-Wall -pedantic -ansi -fPIC -DUNIX -D_GNU_SOURCE -D_LIN${BITS}")
#add_definitions(-Wall -pedantic -ansi -fPIC -DUNIX -D_GNU_SOURCE -D_LIN${BITS})
if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
	message(STATUS "Version < 4.7")
	set(MegaMolCon_common_definitions "${MegaMolCon_common_definitions} -std=c++0x")
	#add_definitions(-std=c++0x)
else()
	set(MegaMolCon_common_definitions "${MegaMolCon_common_definitions} -std=c++11")
	#add_definitions(-std=c++11)
endif()

# Set CXX flags for debug mode
set(CMAKE_CXX_FLAGS_DEBUG -DDEBUG)


# Adding subdirectories
add_subdirectory(Viewer)
add_subdirectory(Glut)
add_subdirectory(Console)

# install target
set(cfg_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib/megamol")

set(cfg_MEGAMOLCON "${CMAKE_INSTALL_PREFIX}/bin/megamol/MegaMolCon")
set(MEGAMOL_SHELL_START_SCRIPT "megamol.sh")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(cfg_MEGAMOLCON "${CMAKE_INSTALL_PREFIX}/bin/megamol/MegaMolCond")
	set(MEGAMOL_SHELL_START_SCRIPT "megamold.sh")
endif()
configure_file(extra/megamol.sh ${CMAKE_BINARY_DIR}/${MEGAMOL_SHELL_START_SCRIPT} @ONLY)
install(PROGRAMS ${CMAKE_BINARY_DIR}/${MEGAMOL_SHELL_START_SCRIPT} DESTINATION "bin")

configure_file(extra/megamol.cfg ${CMAKE_BINARY_DIR}/megamol.cfg @ONLY)
configure_file(extra/install_megamol_cfg.cmake.in ${CMAKE_BINARY_DIR}/install_megamol_cfg.cmake @ONLY)
install(SCRIPT ${CMAKE_BINARY_DIR}/install_megamol_cfg.cmake)


# Specifying subproject dependencies
add_dependencies(MegaMolGlut MegaMolCon_Viewer)
add_dependencies(MegaMolCon MegaMolCon_Viewer)
# this one is not really a dependency for compile time, but for the overall result:
add_dependencies(MegaMolCon MegaMolGlut)
