#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "\\\"a\\a \\z;\\b";
	expect(script,
		make_token(SCALLOP_TOKEN_WORD, 0, 5),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 5, 6),
		make_token(SCALLOP_TOKEN_WORD, 6, 8),
		make_token(SCALLOP_TOKEN_STATEMENT_SEPARATOR, 8, 9),
		make_token(SCALLOP_TOKEN_WORD, 9, 11),
		make_token(SCALLOP_TOKEN_EOF, 11, 12)
	);
}

