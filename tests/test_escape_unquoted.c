#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "\\\"a\\a \\z;\\b";
	expect(script,
		{ SCALLOP_TOKEN_WORD, 0, 0, 5 },
		{ SCALLOP_TOKEN_WORD_SEPARATOR, 0, 5, 6 },
		{ SCALLOP_TOKEN_WORD, 0, 6, 8 },
		{ SCALLOP_TOKEN_STATEMENT_SEPARATOR, 0, 8, 9 },
		{ SCALLOP_TOKEN_WORD, 0, 9, 11 },
		{ SCALLOP_TOKEN_EOF, 0, 11, 12 }
	);
}

