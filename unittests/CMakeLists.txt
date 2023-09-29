enable_testing()

include(GoogleTest)
add_subdirectory(Assembler)
add_subdirectory(Simulator)
add_subdirectory(RIPSimulator)

set(ALL_TESTS ${ALL_TESTS} PARENT_SCOPE)