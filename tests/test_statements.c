#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "foo; bar baz\nbarry;";

	expect(script,
		make_token(SCALLOP_TOKEN_WORD, 0, 3),
		make_token(SCALLOP_TOKEN_STATEMENT_SEPARATOR, 3, 4),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 4, 5),
		make_token(SCALLOP_TOKEN_WORD, 5, 8),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 8, 9),
		make_token(SCALLOP_TOKEN_WORD, 9, 12),
		make_token(SCALLOP_TOKEN_STATEMENT_SEPARATOR, 12, 13),
		make_token(SCALLOP_TOKEN_WORD, 13, 18),
		make_token(SCALLOP_TOKEN_STATEMENT_SEPARATOR, 18, 19),
		make_token(SCALLOP_TOKEN_EOF, 19, 20)
	);
}

