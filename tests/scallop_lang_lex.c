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

#include <scallop-lang/lex.h>
#include <stdio.h>

#define fn scallop_lang_lex_fn

#define word scallop_lang_lex_word
#define word_separator scallop_lang_lex_word_separator
#define end scallop_lang_lex_end
#define escape scallop_lang_lex_escape
#define single_quote scallop_lang_lex_single_quote
#define unexpected scallop_lang_lex_unexpected

void test_word()
{
	assert((fn*)word(WEOF) == end);
	assert((fn*)word('a') == word);
	assert((fn*)word(' ') == word_separator);
	assert((fn*)word('\\') == escape);
	assert((fn*)word('\'') == single_quote);
	assert((fn*)word(1) == unexpected);
}

int main()
{
	test_word();
}
