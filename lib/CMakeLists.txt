file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS *.cpp)

file(GLOB ASM_SOURCES CONFIGURE_DEPENDS Assembler/*.cpp)
add_library(asm SHARED ${ASM_SOURCES} ${COMMON_SOURCES})
target_include_directories(asm PUBLIC ${PROJECT_SOURCE_DIR}/include)

file(GLOB SIM_SOURCES CONFIGURE_DEPENDS Simulator/*.cpp)
add_library(sim SHARED ${SIM_SOURCES} ${COMMON_SOURCES})
target_include_directories(sim PUBLIC ${PROJECT_SOURCE_DIR}/include)

file(GLOB RIPSIM_SOURCES CONFIGURE_DEPENDS RIPSimulator/*.cpp)
add_library(ripsim SHARED ${RIPSIM_SOURCES} ${COMMON_SOURCES})
target_include_directories(ripsim PUBLIC ${PROJECT_SOURCE_DIR}/include)