
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rip-riscvtests)
enable_testing()

include(GoogleTest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS *.cpp)
foreach(TEST_SRC ${TEST_SOURCES})
    
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} GTest::gtest_main ripsim)
    target_include_directories(${TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

    gtest_discover_tests(${TEST_NAME})
endforeach()
