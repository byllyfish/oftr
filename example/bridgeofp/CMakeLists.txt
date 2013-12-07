
add_executable(bridgeofp bridgeofp_main.cpp bridgelistener.cpp)
target_link_libraries(bridgeofp ofp ${LIBOFP_LINKED_LIBS})
