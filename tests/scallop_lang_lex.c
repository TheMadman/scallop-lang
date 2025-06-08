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
#include "scallop-lang/lex.h"

#include <libadt/str.h>

#define lit libadt_str_literal
#define lex_init scallop_lang_lex_init
#define lex_next scallop_lang_lex_next
typedef struct scallop_lang_lex lex_t;
typedef struct libadt_const_lptr const_lptr_t;
typedef struct libadt_lptr lptr_t;

#define TEST_SCRIPT "word second_word"

void test_lex_init(void)
{
	lex_t lex = lex_init(lit(TEST_SCRIPT));

	assert(lex.script.length == sizeof(TEST_SCRIPT) - 1);
}

void test_lex_next_simple(void)
{
	lex_t lex = lex_init(lit(TEST_SCRIPT));
	lex = lex_next(lex);

	assert(lex.type == scallop_lang_classifier_word);
	assert(lex.value.length == sizeof("word") - 1);

	lex = lex_next(lex);

	assert(lex.type == scallop_lang_classifier_word_separator);
	assert(lex.value.length == sizeof(" ") - 1);

	lex = lex_next(lex);
	assert(lex.type == scallop_lang_classifier_word);
	assert(lex.value.length == sizeof("second_word") - 1);

	lex = lex_next(lex);
	assert(lex.type == scallop_lang_classifier_end);
}

#define WORD_STATEMENT_SEPARATOR "  ;\n  "

void test_lex_next_statement_separator_promotion(void)
{
	lex_t lex = lex_init(lit(WORD_STATEMENT_SEPARATOR));
	lex = lex_next(lex);

	assert(lex.type == scallop_lang_classifier_statement_separator);
	assert(lex.value.length == sizeof(WORD_STATEMENT_SEPARATOR) - 1);
}

void test_lex_normalize_word(void)
{
	const char word_buffer[] = "\"Hello, \"'world'\\!";
	char out_buffer[255] = { 0 };

	const_lptr_t word = lit(word_buffer);
	lptr_t out = libadt_lptr_init_array(out_buffer);

	ssize_t result = scallop_lang_lex_normalize_word(word, out);

	assert(result == sizeof("Hello, world!") - 1);
	assert(0 == strcmp(out_buffer, "Hello, world!"));
}

int main()
{
	test_lex_init();
	test_lex_next_simple();
	test_lex_next_statement_separator_promotion();
	test_lex_normalize_word();
}
