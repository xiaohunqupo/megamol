# MegaMol
# Copyright (c) 2021, MegaMol Dev Team
# All rights reserved.
#

megamol_plugin(molecularmaps
  BUILD_DEFAULT OFF
  DEPENDS_FEATURES
    cuda
  DEPENDS_PLUGINS
    mmstd
    mmstd_gl
    protein_calls
    geometry_calls
    geometry_calls_gl)

if (molecularmaps_PLUGIN_ENABLED)
  find_package(Eigen3 CONFIG REQUIRED)

  if(NOT CMAKE_CUDA_COMPILER_VERSION VERSION_GREATER_EQUAL 10)
    message(FATAL_ERROR "Molecularmaps currently only supports CUDA Version 11 or above. The version found was CUDA ${CUDA_VERSION_MAJOR} (${CMAKE_CUDA_COMPILER_VERSION}). Please turn off the Molecularmaps plugin or install a correct version of CUDA." )
  endif()

  set_target_properties(molecularmaps PROPERTIES CUDA_ARCHITECTURES 52)
  target_compile_options(molecularmaps PRIVATE $<$<AND:$<COMPILE_LANGUAGE:CUDA>,$<CONFIG:RELEASE>>:-O3>)

  file(GLOB_RECURSE cuda_source_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cu")
  file(GLOB_RECURSE cuda_header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cuh")
  file(GLOB_RECURSE cuda_helpers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/helper_includes/*.h")

  # Set language to CUDA for respective files.
  set_source_files_properties(cuda_source_files PROPERTIES LANGUAGE CUDA)

  # Target definition
  target_sources(molecularmaps PRIVATE ${cuda_header_files} ${cuda_source_files})
  set_target_properties(molecularmaps PROPERTIES CUDA_STANDARD 17)

  target_include_directories(molecularmaps PUBLIC "src/helper_includes")
  target_link_libraries(molecularmaps PRIVATE vislib Eigen3::Eigen)

  if(NOT MSVC)
    get_filename_component(CUDA_BIN_DIR ${CMAKE_CUDA_COMPILER} DIRECTORY)
    target_include_directories(molecularmaps PUBLIC "${CUDA_BIN_DIR}/../include")
    target_compile_definitions(molecularmaps PRIVATE __CUDACC__)
  endif()

  # Grouping in Visual Studio
  source_group("CUDA Header Files" FILES ${cuda_header_files})
  source_group("CUDA Source Files" FILES ${cuda_source_files})
endif ()
