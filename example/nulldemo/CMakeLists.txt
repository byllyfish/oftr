# Build nullagent and nullcontroller.

add_executable(nullagent "nullagent_main.cpp")
target_link_libraries(nullagent ofp)

add_executable(nullcontroller "nullcontroller_main.cpp")
target_link_libraries(nullcontroller ofp)

