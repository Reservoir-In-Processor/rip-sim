# cmake_minimum_required(VERSION 3.25)
cmake_minimum_required(VERSION 3.16)
project(asmkheiv CXX)
# FIXME: this seems bad...
# set (CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE PATH "" FORCE)

set(CMAKE_CXX_FLAGS "-Wall -fPIC  ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)
# for valgrind https://hg.mozilla.org/integration/autoland/rev/192810f500b5
# set(CMAKE_CXX_FLAGS_DEBUG "-gdwarf-4")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()


# TODO switch on off by CMAKE option
message(STATUS "build on ${CMAKE_SYSTEM_NAME}, ${CMAKE_SYSTEM_PROCESSOR}")

option(SANITIZE "Enable sanitize options" ON)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(SANITIZE OFF)
endif()

option(ENABLE_WERROR "Enable -Werror options" OFF)

if (ENABLE_WERROR)
  set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
endif()
if (SANITIZE)
  message(STATUS "sanitizer build")
  set(CMAKE_CXX_FLAGS "-fsanitize=undefined ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "-fsanitize=address ${CMAKE_CXX_FLAGS}")
  if (NOT APPLE) # apple clang doesn't have leak sanitizer
    set(CMAKE_CXX_FLAGS "-fsanitize=leak ${CMAKE_CXX_FLAGS}")
  endif()
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_custom_target(build ALL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



include_directories(include)
add_subdirectory(lib)


add_subdirectory(tools)

add_subdirectory(nlohmann_json)

include(FetchContent)

if(${CMAKE_VERSION} VERSION_LESS "3.25")
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
else()
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
endif()
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()
# enable unit tests
set(ALL_TESTS CACHE PATH "" FORCE)
add_subdirectory(unittests)
include(ProcessorCount)
ProcessorCount(N)

# FIXME: work filter
add_custom_target(unittests
  COMMAND ctest --test-dir ./unittests/ --output-on-failure --timeout 5 -j ${N}
  DEPENDS ${ALL_TESTS}
)


add_custom_target(sim-unittests
  COMMAND ctest -R "^SimulatorTest*" --test-dir ./unittests/ --output-on-failure --timeout 5 -j ${N}
  DEPENDS ${ALL_TESTS}
)

add_custom_target(rip-unittests
  COMMAND ctest -R "RIPSimulatorTest*" --test-dir ./unittests/ --output-on-failure --timeout 5 -j ${N}
  DEPENDS ${ALL_TESTS}
)

file(COPY rip-tests DESTINATION ${CMAKE_BINARY_DIR})


# Add custom target to make dependency for test files
add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/rip-tests
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/rip-tests ${CMAKE_BINARY_DIR}/rip-tests
  DEPENDS ${CMAKE_SOURCE_DIR}/rip-tests
)

add_custom_target(copy_tests DEPENDS ${CMAKE_BINARY_DIR}/rip-tests)

add_subdirectory(riscvtests)
# FIXME: do riscvtests
add_custom_target(riscvtests
  COMMAND ctest --test-dir ./riscvtests/ --output-on-failure --timeout 10 -j ${N}
  DEPENDS RiscvTests copy_tests
)

add_subdirectory(rip-riscvtests)
add_custom_target(rip-riscvtests
  COMMAND ctest --test-dir ./rip-riscvtests/ --output-on-failure --timeout 10 -j ${N}
  DEPENDS RIPRiscvTests copy_tests
)

add_subdirectory(dhrystone)
add_custom_target(dhrystone
  COMMAND ctest --test-dir ./dhrystone/ --output-on-failure --timeout 150 -j ${N}
  DEPENDS DhrystoneTest copy_tests
)

add_subdirectory(rip-dhrystone)
add_custom_target(rip-dhrystone
  COMMAND ctest --test-dir ./rip-dhrystone/ --output-on-failure --timeout 150 -j ${N}
  DEPENDS RIPDhrystoneTest copy_tests
)

# TODO: run paralelly.
add_custom_target(rip-all
  DEPENDS rip-unittests rip-dhrystone rip-riscvtests
)

# TODO: run paralelly.
add_custom_target(sim-all
  DEPENDS unittests dhrystone riscvtests
)


add_custom_target(asmtest
  COMMAND make -C ../tests/asm test
  DEPENDS asmkheiv simkheiv
)


set(CMAKE_COLOR_DIAGNOSTICS ON)

# python wrapper for rip-sim
add_custom_target(
    rip-copy-py
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_BINARY_DIR}/bin/rip-sim
            ${CMAKE_SOURCE_DIR}/python-wrapper/rip_simulator/rip-sim
    COMMENT "Copying rip-sim to python_wrapper directory"
    DEPENDS rip-sim
)

add_custom_target(pytest
    COMMAND python3 -m pytest ${CMAKE_SOURCE_DIR}/python-wrapper/tests --capture=no
    DEPENDS rip-sim rip-copy-py
)

add_custom_target(python-online-rls
    COMMAND python3 ${CMAKE_SOURCE_DIR}/python-wrapper/online_reservoir_branch_predictor_rls.py
    DEPENDS rip-sim rip-copy-py
)

add_custom_target(python-online-lms
    COMMAND python3 ${CMAKE_SOURCE_DIR}/python-wrapper/online_reservoir_branch_predictor_lms.py
    DEPENDS rip-sim rip-copy-py
)


# TODO: platform compatibility problem
# add_custom_target(pybuild
#     COMMAND python3 -m build
#     DEPENDS rip-sim rip-copy-py
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )