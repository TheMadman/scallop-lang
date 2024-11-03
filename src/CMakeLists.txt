set(SOURCES lex.c)

add_library(scalloplang SHARED ${SOURCES})
add_library(scalloplangstatic STATIC ${SOURCES})
