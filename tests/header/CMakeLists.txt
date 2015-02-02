#  Copyright (c) 2015 Agustin Berge
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

file(GLOB_RECURSE headers RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/hpx/*.hpp")
foreach(header ${headers})
  set(HPX_HEADER ${header})
  configure_file(
      "${CMAKE_SOURCE_DIR}/cmake/templates/header_test.cpp.in"
      "${CMAKE_BINARY_DIR}/tests/header/${header}.cpp"
      @ONLY)

  set(sources
    "${CMAKE_BINARY_DIR}/tests/header/${header}.cpp")

  source_group("Source Files" FILES ${sources})

  # add example executable
  string(REPLACE "/" "_" test ${header})

  add_hpx_executable(${test}_test
                     SOURCES ${sources}
                     ${${test}_FLAGS}
                     EXCLUDE_FROM_ALL
                     HPX_PREFIX ${HPX_BUILD_PREFIX}
                     FOLDER "Tests/Header/")

  string(REPLACE "/" "." test ${header})

  add_hpx_header_test(${test} ${${test}_PARAMETERS})

  # add a custom target for this example
  add_hpx_pseudo_target(tests.header.${test})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(tests.header
                              tests.header.${test})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(tests.header.${test}
                              ${test}_test_exe)
endforeach()
