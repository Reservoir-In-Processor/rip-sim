enable_testing()

set(ALL_TESTS "")
file(GLOB TEST_SOURCES *.cpp)
include(GoogleTest)
foreach(TEST_SRC ${TEST_SOURCES})
    
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} GTest::gtest_main asm)
    target_include_directories(${TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
    
    list(APPEND ALL_TESTS ${TEST_NAME})

    gtest_discover_tests(${TEST_NAME})
endforeach()

set(ALL_TESTS ${ALL_TESTS} PARENT_SCOPE)