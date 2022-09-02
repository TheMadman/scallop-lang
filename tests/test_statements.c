#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "foo; bar baz\nbarry;";

	expect(script,
		{ SCALLOP_TOKEN_WORD, 0, 0, 3 },
		{ SCALLOP_TOKEN_STATEMENT_SEPARATOR, 0, 3, 4 },
		{ SCALLOP_TOKEN_WORD_SEPARATOR, 0, 4, 5 },
		{ SCALLOP_TOKEN_WORD, 0, 5, 8 },
		{ SCALLOP_TOKEN_WORD_SEPARATOR, 0, 8, 9 },
		{ SCALLOP_TOKEN_WORD, 0, 9, 12 },
		{ SCALLOP_TOKEN_STATEMENT_SEPARATOR, 0, 12, 13 },
		{ SCALLOP_TOKEN_WORD, 0, 13, 18 },
		{ SCALLOP_TOKEN_STATEMENT_SEPARATOR, 0, 18, 19 },
		{ SCALLOP_TOKEN_EOF, 0, 19, 20 }
	);
}

