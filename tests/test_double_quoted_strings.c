#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "\"foo\" foo\"bar\" \"bar\"baz foo\"bar\"baz";

	expect(script,
		make_token(SCALLOP_TOKEN_WORD, 0, 5),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 5, 6),
		make_token(SCALLOP_TOKEN_WORD, 6, 14),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 14, 15),
		make_token(SCALLOP_TOKEN_WORD, 15, 23),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 23, 24),
		make_token(SCALLOP_TOKEN_WORD, 24, 35),
		make_token(SCALLOP_TOKEN_EOF, 35, 36)
	);
}

