/*
 * Scallop - A Shell Language for Parallelization (Language Definition)
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
\
#include "macros.h"

#include <scallop-lang/classifier.h>
#include <stdio.h>

#define fn scallop_lang_classifier_fn

#define word scallop_lang_classifier_word
#define word_separator scallop_lang_classifier_word_separator
#define end scallop_lang_classifier_end
#define escape scallop_lang_classifier_escape
#define single_quote scallop_lang_classifier_single_quote
#define unexpected scallop_lang_classifier_unexpected
#define single_quote scallop_lang_classifier_single_quote
#define single_quote_word scallop_lang_classifier_single_quote_word
#define single_quote_end scallop_lang_classifier_single_quote_end
#define double_quote scallop_lang_classifier_double_quote
#define double_quote_word scallop_lang_classifier_double_quote_word
#define double_quote_end scallop_lang_classifier_double_quote_end
#define curly_block scallop_lang_classifier_curly_block
#define curly_block_end scallop_lang_classifier_curly_block_end
#define square_block scallop_lang_classifier_square_block
#define square_block_end scallop_lang_classifier_square_block_end
#define statement_separator scallop_lang_classifier_statement_separator
#define escape scallop_lang_classifier_escape
#define line_comment scallop_lang_classifier_line_comment

void default_context_asserts(fn *state)
{
	assert((fn*)state(WEOF) == end);
	assert((fn*)state('a') == word);
	assert((fn*)state(' ') == word_separator);
	assert((fn*)state('\\') == escape);
	assert((fn*)state('\'') == single_quote);
	assert((fn*)state(';') == statement_separator);
	assert((fn*)state('#') == line_comment);
	assert((fn*)state(1) == unexpected);
}

void single_quote_context_asserts(fn *state)
{
	assert((fn*)state(WEOF) == unexpected);
	assert((fn*)state('\'') == single_quote_end);
	assert((fn*)state('a') == single_quote_word);
	assert((fn*)state('"') == single_quote_word);
}

void double_quote_context_asserts(fn *state)
{
	assert((fn*)state(WEOF) == unexpected);
	assert((fn*)state('"') == double_quote_end);
	assert((fn*)state('\'') == double_quote_word);
	assert((fn*)state('a') == double_quote_word);
}

void test_word(void)
{
	default_context_asserts(word);
}

void test_word_separator(void)
{
	default_context_asserts(word_separator);
}

void test_single_quote(void)
{
	single_quote_context_asserts(single_quote);
}

void test_single_quote_word(void)
{
	single_quote_context_asserts(single_quote_word);
}

void test_single_quote_end(void)
{
	default_context_asserts(single_quote_end);
}

void test_double_quote(void)
{
	double_quote_context_asserts(double_quote);
}

void test_double_quote_word(void)
{
	double_quote_context_asserts(double_quote_word);
}

void test_double_quote_end(void)
{
	default_context_asserts(double_quote_end);
}

void test_curly_block(void)
{
	default_context_asserts(curly_block);
}

void test_curly_block_end(void)
{
	default_context_asserts(curly_block_end);
}

void test_square_block(void)
{
	default_context_asserts(square_block);
}

void test_square_block_end(void)
{
	default_context_asserts(square_block_end);
}

void test_statement_separator(void)
{
	default_context_asserts(statement_separator);
}

void test_escape(void)
{
	assert((fn*)escape('a') == word);
	assert((fn*)escape('\\') == word);
	assert((fn*)escape('\'') == word);
	assert((fn*)escape('"') == word);
	assert((fn*)escape(WEOF) == unexpected);
}

void test_line_comment(void)
{
	assert((fn*)line_comment('a') == line_comment);
	assert((fn*)line_comment(' ') == line_comment);
	assert((fn*)line_comment(';') == line_comment);
	assert((fn*)line_comment('\'') == line_comment);
	assert((fn*)line_comment('"') == line_comment);
	assert((fn*)line_comment('\r') == statement_separator);
	assert((fn*)line_comment('\n') == statement_separator);
	assert((fn*)line_comment(WEOF) == end);
}

int main()
{
	test_word();
	test_word_separator();
	test_single_quote_end();
	test_curly_block();
	test_curly_block_end();
	test_square_block();
	test_square_block_end();
	test_single_quote();
	test_escape();
	test_line_comment();
}
