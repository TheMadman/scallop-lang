#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "[ [a[[💩[;[\"[\"['['";
	expect(script,
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 0, 1),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 1, 2),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 2, 3),
		make_token(SCALLOP_TOKEN_WORD, 3, 4),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 4, 5),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 5, 6),
		make_token(SCALLOP_TOKEN_WORD, 6, 10),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 10, 11),
		make_token(SCALLOP_TOKEN_STATEMENT_SEPARATOR, 11, 12),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 12, 13),
		make_token(SCALLOP_TOKEN_WORD, 13, 16),
		make_token(SCALLOP_TOKEN_OPEN_SQUARE_BRACKET, 16, 17),
		make_token(SCALLOP_TOKEN_EOF, 6, 7)
	);
}

