# cmake file for oxmfields.h, oxmfields.cpp and oxmfieldsdata.cpp.

set(oxmfields_h "${CMAKE_BINARY_DIR}/include/ofp/oxmfields.h")
set(oxmfields_cpp "${CMAKE_CURRENT_BINARY_DIR}/oxmfields.cpp")
set(oxmfieldsdata_cpp ${CMAKE_CURRENT_BINARY_DIR}/oxmfieldsdata.cpp)
set(oxmfields_main_cpp ${CMAKE_CURRENT_BINARY_DIR}/oxmfields_main.cpp)

set(oxmfields_tab ${CMAKE_CURRENT_SOURCE_DIR}/oxmfields.tab)
set(oxmfields_compile_cpp ${CMAKE_CURRENT_SOURCE_DIR}/oxmfields_compile_main.cpp)
set(oxmfields_compile3_cpp ${CMAKE_CURRENT_SOURCE_DIR}/oxmfields_compile3_main.cpp)
set(oxmlist_cpp "${CMAKE_SOURCE_DIR}/src/ofp/oxmlist.cpp")
set(bytelist_cpp "${CMAKE_SOURCE_DIR}/src/ofp/bytelist.cpp")

# Make ofp sub-directory for oxmfields.h

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ofp)

# Build `oxmfields_compile`.

add_executable(oxmfields_compile "${oxmfields_compile_cpp}")

# Use `oxmfields_compile` to produce oxmfields_main.cpp and oxmfields.h.

add_custom_command(
  OUTPUT "${oxmfields_main_cpp}"
  COMMAND oxmfields_compile < "${oxmfields_tab}" > "${oxmfields_main_cpp}"
  DEPENDS oxmfields_compile ${oxmfields_tab}
)

add_custom_command(
  OUTPUT "${oxmfields_h}"
  COMMAND oxmfields_compile -h  < "${oxmfields_tab}" > "${oxmfields_h}"
  DEPENDS oxmfields_compile ${oxmfields_tab}
)

# Build `oxmfields_compile2`.

add_executable(oxmfields_compile2 ${oxmfields_main_cpp} ${oxmlist_cpp} ${oxmfields_h} ${bytelist_cpp})

# Use `oxmfields_compile2` to produce oxmfields.cpp.

add_custom_command(
  OUTPUT "${oxmfields_cpp}"
  COMMAND oxmfields_compile2 > "${oxmfields_cpp}"
  DEPENDS oxmfields_compile2
)

# Build `oxmfields_compile3`.

add_executable(oxmfields_compile3 ${oxmlist_cpp} ${oxmfields_cpp} ${oxmfields_compile3_cpp} ${bytelist_cpp})

# Use `oxmfields_compile3` to produce oxmfieldsdata.cpp.

add_custom_command(
  OUTPUT "${oxmfieldsdata_cpp}"
  COMMAND oxmfields_compile3 > "${oxmfieldsdata_cpp}"
  DEPENDS oxmfields_compile3
)


add_custom_target(libofp_oxmfields_h DEPENDS ${oxmfields_h})
add_custom_target(libofp_oxmfields_cpp DEPENDS "${oxmfields_cpp}")
add_custom_target(libofp_oxmfieldsdata_cpp DEPENDS "${oxmfieldsdata_cpp}")
