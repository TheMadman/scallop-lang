add_library(lexer STATIC lexer.c scallop/lexer.h)
target_link_libraries(lexer csalt)
target_include_directories(lexer
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
