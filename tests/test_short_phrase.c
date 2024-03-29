#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = "foo bar\tbaz";

	expect(script,
		make_token(SCALLOP_TOKEN_WORD, 0, 3),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 3, 4),
		make_token(SCALLOP_TOKEN_WORD, 4, 7),
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 7, 8),
		make_token(SCALLOP_TOKEN_WORD, 8, 11),
		make_token(SCALLOP_TOKEN_EOF, 11, 12)
	);
}

