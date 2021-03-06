# Copyright (c) 2014-2015 Hartmut Kaiser
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests
    adjacentfind
    adjacentfind_binary
    all_of
    any_of
    copy
    #copyif
    copyn
    count
    countif
    equal
    equal_binary
    exclusive_scan
    fill
    filln
    find
    findend
    findfirstof
    findfirstof_binary
    findif
    findifnot
    foreach
    foreachn
    generate
    generaten
    inclusive_scan
    is_partitioned
    is_sorted
    is_sorted_until
    lexicographical_compare
    max_element
    min_element
    minmax_element
    mismatch
    mismatch_binary
    move
    none_of
    reduce_
    replace
    replace_if
    replace_copy
    replace_copy_if
    reverse
    reverse_copy
    rotate
    rotate_copy
    search
    searchn
    swapranges
    task_region
    transform
    transform_binary
    transform_exclusive_scan
    transform_inclusive_scan
    transform_reduce
    uninitialized_copy
    uninitialized_copyn
    uninitialized_fill
    uninitialized_filln
   )

set(task_region_FLAGS DEPENDENCIES iostreams_component)

foreach(test ${tests})
  set(sources
      ${test}.cpp)

  set(${test}_PARAMETERS THREADS_PER_LOCALITY 4)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${test}_test
                     SOURCES ${sources}
                     ${${test}_FLAGS}
                     EXCLUDE_FROM_ALL
                     HPX_PREFIX ${HPX_BUILD_PREFIX}
                     FOLDER "Tests/Unit/Parallel/")

  add_hpx_unit_test("parallel" ${test} ${${test}_PARAMETERS})

  # add a custom target for this example
  add_hpx_pseudo_target(tests.unit.parallel.${test})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.parallel
                              tests.unit.parallel.${test})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.parallel.${test}
                              ${test}_test_exe)
endforeach()
