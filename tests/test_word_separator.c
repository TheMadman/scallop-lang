#include "test_macros.h"

#include <csalt/stores.h>

int main()
{
	static const char script[] = " \t";
	
	expect(script,
		make_token(SCALLOP_TOKEN_WORD_SEPARATOR, 0, 2),
		make_token(SCALLOP_TOKEN_EOF, 2, 3)
	);
}


