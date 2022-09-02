#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "{ {a{{💩{;{\"{\"{'{'";
	expect(script,
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 0, 1 },
		{ SCALLOP_TOKEN_WORD_SEPARATOR, 0, 1, 2 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 2, 3 },
		{ SCALLOP_TOKEN_WORD, 0, 3, 4 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 4, 5 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 5, 6 },
		{ SCALLOP_TOKEN_WORD, 0, 6, 10 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 10, 11 },
		{ SCALLOP_TOKEN_STATEMENT_SEPARATOR, 0, 11, 12 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 12, 13 },
		{ SCALLOP_TOKEN_WORD, 0, 13, 16 },
		{ SCALLOP_TOKEN_OPEN_CURLY_BRACKET, 0, 16, 17 },
		{ SCALLOP_TOKEN_EOF, 0, 6, 7 }
	);
}

