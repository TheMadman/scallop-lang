/*
 * Project Name - Project Description
 * Copyright (C) 2024
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdbool.h>
#include "scallop-lang/token.h"

#include <libadt/str.h>

inline struct scallop_lang_token scallop_lang_token_init();
inline struct scallop_lang_token scallop_lang_token_next(
	struct scallop_lang_token previous_token
);

#define lit libadt_str_literal
#define token_init scallop_lang_token_init
#define token_next scallop_lang_token_next
typedef struct scallop_lang_token token_t;

#define TEST_SCRIPT "word second_word"

void test_token_init(void)
{
	token_t token = token_init(lit(TEST_SCRIPT));

	assert(token.script.length == sizeof(TEST_SCRIPT) - 1);
}

void test_token_next_simple(void)
{
	token_t token = token_init(lit(TEST_SCRIPT));
	token = token_next(token);

	assert(token.type == scallop_lang_lex_word);
	assert(token.value.length == sizeof("word") - 1);

	token = token_next(token);

	assert(token.type == scallop_lang_lex_word_separator);
	assert(token.value.length == sizeof(" ") - 1);

	token = token_next(token);
	assert(token.type == scallop_lang_lex_word);
	assert(token.value.length == sizeof("second_word") - 1);

	token = token_next(token);
	assert(token.type == scallop_lang_lex_end);
}

#define WORD_STATEMENT_SEPARATOR "  ;\n  "

void test_token_next_statement_separator_promotion(void)
{
	token_t token = token_init(lit(WORD_STATEMENT_SEPARATOR));
	token = token_next(token);

	assert(token.type == scallop_lang_lex_statement_separator);
	assert(token.value.length == sizeof(WORD_STATEMENT_SEPARATOR) - 1);
}

int main()
{
	test_token_init();
	test_token_next_simple();
	test_token_next_statement_separator_promotion();
}
