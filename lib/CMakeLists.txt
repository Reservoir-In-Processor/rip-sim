file(GLOB ASM_SOURCES *.cpp)

add_library(asm SHARED ${ASM_SOURCES})
target_include_directories(asm PUBLIC ${PROJECT_SOURCE_DIR}/include)