set(SOURCES lex.c token.c)

add_library(scallopobj OBJECT ${SOURCES})

set_property(TARGET scallopobj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(scallop-lang SHARED)
target_link_libraries(scallop-lang scallopobj adt)

add_library(scallop-lang-static STATIC)
target_link_libraries(scallop-lang-static scallopobj adt)

target_include_directories(scallop-lang
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_include_directories(scallop-lang-static
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)

install(TARGETS scallop-lang scallop-lang-static
	DESTINATION lib)
install(DIRECTORY scallop-lang DESTINATION include)
