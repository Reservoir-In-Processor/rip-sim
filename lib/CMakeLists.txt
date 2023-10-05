file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS *.cpp)
add_library(common SHARED ${COMMON_SOURCES})
target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/include)

file(GLOB ASM_SOURCES CONFIGURE_DEPENDS Assembler/*.cpp)
add_library(asm SHARED ${ASM_SOURCES})
target_link_libraries(asm common)
target_include_directories(asm PUBLIC ${PROJECT_SOURCE_DIR}/include)

file(GLOB SIM_SOURCES CONFIGURE_DEPENDS Simulator/*.cpp)
add_library(sim SHARED ${SIM_SOURCES})
target_link_libraries(sim common)
target_include_directories(sim PUBLIC ${PROJECT_SOURCE_DIR}/include)

file(GLOB RIPSIM_SOURCES CONFIGURE_DEPENDS RIPSimulator/*.cpp)
add_library(ripsim SHARED ${RIPSIM_SOURCES})
target_link_libraries(ripsim common)
target_include_directories(ripsim PUBLIC ${PROJECT_SOURCE_DIR}/include)