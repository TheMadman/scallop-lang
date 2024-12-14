set(SOURCES lex.c token.c)

add_library(scallopobj OBJECT ${SOURCES})

set_property(TARGET scallopobj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(scalloplang SHARED)
target_link_libraries(scalloplang scallopobj adt)

add_library(scalloplangstatic STATIC)
target_link_libraries(scalloplangstatic scallopobj adt)

target_include_directories(scalloplang
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)
target_include_directories(scalloplangstatic
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:include>)

install(TARGETS scalloplang scalloplangstatic
	DESTINATION lib)
install(DIRECTORY scalloplang DESTINATION include)
