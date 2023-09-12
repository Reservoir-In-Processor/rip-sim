file(GLOB ASM_SOURCES *.cpp)
# TODO: reduce library for each

add_library(asm SHARED ${ASM_SOURCES})
target_include_directories(asm PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(sim SHARED ${ASM_SOURCES})
target_include_directories(sim PUBLIC ${PROJECT_SOURCE_DIR}/include)
