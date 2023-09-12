cmake_minimum_required(VERSION 3.25)
project(asmkheiv CXX)
# FIXME: this seems bad...
set (CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE PATH "" FORCE)

set(CMAKE_CXX_FLAGS "-Wall -march=native -fPIC  ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)
# for valgrind https://hg.mozilla.org/integration/autoland/rev/192810f500b5
# set(CMAKE_CXX_FLAGS_DEBUG "-gdwarf-4")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Default build type: RelWithDebInfo" FORCE)
endif()

# TODO switch on off by CMAKE option
message(STATUS "build on ${CMAKE_SYSTEM_NAME}, ${CMAKE_SYSTEM_PROCESSOR}")

option(SANITIZE "Enable sanitize options" ON)

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(SANITIZE OFF)
endif()

if (SANITIZE)
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



include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# enable unit tests
add_subdirectory(unittests)

add_custom_target(unittests
  COMMAND ctest --test-dir ./unittests/ --output-on-failure 
  DEPENDS ${ALL_TESTS}
)
set(CMAKE_COLOR_DIAGNOSTICS ON)


add_custom_target(test
  COMMAND make -C ../tests test
  DEPENDS asmkheiv simkheiv
)
