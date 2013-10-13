

set(BOOST_SOURCES  
	libs/system/src/error_code.cpp
	libs/program_options/src/cmdline.cpp
	libs/program_options/src/config_file.cpp
	libs/program_options/src/convert.cpp
	libs/program_options/src/options_description.cpp
	libs/program_options/src/parsers.cpp
	libs/program_options/src/positional_options.cpp
	libs/program_options/src/split.cpp
	libs/program_options/src/utf8_codecvt_facet.cpp
	libs/program_options/src/value_semantic.cpp
	libs/program_options/src/variables_map.cpp
	#libs/program_options/src/winmain.cpp
)

add_library(boost ${BOOST_SOURCES})
