add_library(scalloplexer STATIC lexer.c scallop/lexer.h)
target_link_libraries(scalloplexer csalt)
target_include_directories(scalloplexer
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
