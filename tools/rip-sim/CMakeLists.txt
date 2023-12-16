add_executable(rip-sim rip-sim.cpp)
target_link_libraries(rip-sim ripsim)

add_custom_command(
    TARGET rip-sim
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:rip-sim>
            ${CMAKE_SOURCE_DIR}/python-wrapper/rip_simulator/rip-sim
    COMMENT "Copying rip-sim to python_wrapper directory"
)
