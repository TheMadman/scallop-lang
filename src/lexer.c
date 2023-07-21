/*
 * Scallop - a shell for executing tasks concurrently
 * Copyright (C) <year>  Marcus Harrison
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
#include "scallop/lexer.h"

#include <stdio.h> // EOF
#include <stdlib.h>
#include <csalt/resources.h>
#include <assert.h>
#include <stdbool.h>

// This is all probably horribly inefficient but whatever

static int get_char_internal(csalt_store *store, void *param)
{
	return (int)csalt_store_read(store, param, 1);
}

static int get_char(csalt_store *store, ssize_t index)
{
	char result = 0;
	int success = csalt_store_split(
		store,
		index,
		index + 1,
		get_char_internal,
		&result
	);
	return success <= 0 ? success - 1 : result;
}

enum CHAR_TYPE {
	CHAR_NULL = 1,
	CHAR_ERROR,
	CHAR_EAGAIN,
	CHAR_ASCII_PRINTABLE,
	CHAR_UTF8_START,
	CHAR_UTF8_CONT,
	CHAR_OPEN_CURLY_BRACKET,
	CHAR_CLOSE_CURLY_BRACKET,
	CHAR_OPEN_SQUARE_BRACKET,
	CHAR_CLOSE_SQUARE_BRACKET,
	CHAR_QUOTE,
	CHAR_DOUBLE_QUOTE,
	CHAR_BACKSLASH,
	CHAR_WORD_SEPARATOR,
	CHAR_STATEMENT_SEPARATOR,
	CHAR_PIPE,
	CHAR_UNKNOWN,
};

#define UTF8_FIRST_BIT (1 << 7)
#define UTF8_SECOND_BIT (1 << 6)

static enum CHAR_TYPE char_type(int character)
{
	switch (character) {
		case -2:
			return CHAR_ERROR;
		case -1:
			return CHAR_EAGAIN;
		case '{':
			return CHAR_OPEN_CURLY_BRACKET;
		case '}':
			return CHAR_CLOSE_CURLY_BRACKET;
		case '[':
			return CHAR_OPEN_SQUARE_BRACKET;
		case ']':
			return CHAR_CLOSE_SQUARE_BRACKET;
		case '\'':
			return CHAR_QUOTE;
		case '"':
			return CHAR_DOUBLE_QUOTE;
		case '\\':
			return CHAR_BACKSLASH;
		case ';':
		case '\n':
			return CHAR_STATEMENT_SEPARATOR;
		case ' ':
		case '\t':
			return CHAR_WORD_SEPARATOR;
		case '|':
			return CHAR_PIPE;
		case '\0':
			return CHAR_NULL;
	}

	if (' ' <= character && character <= '~')
		return CHAR_ASCII_PRINTABLE;

	unsigned char utf8_bits = UTF8_FIRST_BIT | UTF8_SECOND_BIT;
	switch (character & utf8_bits) {
		// UTF-8 continuation character
		case UTF8_FIRST_BIT:
			return CHAR_UTF8_CONT;
		// UTF-8 start character
		case UTF8_FIRST_BIT | UTF8_SECOND_BIT:
			return CHAR_UTF8_START;
	}

	return CHAR_UNKNOWN;
}

static enum CHAR_TYPE get_char_type(csalt_store *source, ssize_t offset)
{
	return char_type(get_char(source, offset));
}

typedef void void_fn(void);

struct state_transition {
	void_fn *next_state;
	struct scallop_parse_token new_token;
};

typedef struct state_transition lex_fn(
	struct scallop_parse_token,
	enum CHAR_TYPE input
);

struct transition_row {
	enum CHAR_TYPE input;
	lex_fn *next_state;
};

struct state_transition lex_error(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)token;
	(void)type;

	assert(!"lex_error should never be called!");
	return (struct state_transition) {
		(void_fn *)lex_error,
		token
	};
}

static void_fn *transition_state_bounds(
	const struct transition_row *rows_begin,
	const struct transition_row *rows_end,
	enum CHAR_TYPE input
)
{
	for (
		const struct transition_row *current = rows_begin;
		current < rows_end;
		current++
	) {
		if (current->input == input)
			return (void_fn *)current->next_state;
	}

	return (void_fn *)lex_error;
}

#define transition_state(array, input) \
	transition_state_bounds((array), arrend(array), input)

struct state_transition lex_eof(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_utf8_start(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_utf8_cont(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_quoted_utf8_start(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_quoted_utf8_cont(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_word_separator(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_statement_separator(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_end_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_double_quoted_utf8_start(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_double_quoted_utf8_cont(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_double_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_end_double_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_open_curly_bracket(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_close_curly_bracket(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_open_square_bracket(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_close_square_bracket(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_word(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_escape_word(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_escape_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_escape_double_quoted_string(
	struct scallop_parse_token,
	enum CHAR_TYPE
);
struct state_transition lex_begin(
	struct scallop_parse_token,
	enum CHAR_TYPE
);

struct state_transition lex_eof(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)type;
	token.token = SCALLOP_TOKEN_EOF;
	return (struct state_transition) {
		NULL,
		token,
	};
}

struct state_transition lex_utf8_start(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_UTF8_CONT, lex_utf8_cont },
	};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_utf8_cont(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_UTF8_START, lex_utf8_start },
		{ CHAR_UTF8_CONT, lex_utf8_cont },
		{ CHAR_ASCII_PRINTABLE, lex_word },
		{ CHAR_BACKSLASH, lex_escape_word },
		{ CHAR_WORD_SEPARATOR, NULL },
		{ CHAR_STATEMENT_SEPARATOR, NULL },
		{ CHAR_OPEN_CURLY_BRACKET, NULL },
		{ CHAR_CLOSE_CURLY_BRACKET, NULL },
		{ CHAR_OPEN_SQUARE_BRACKET, NULL },
		{ CHAR_CLOSE_SQUARE_BRACKET, NULL },
		{ CHAR_NULL, NULL },
	};
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_quoted_utf8_start(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_UTF8_CONT, lex_quoted_utf8_cont },
	};
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_quoted_utf8_cont(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_quoted_string },
		{ CHAR_WORD_SEPARATOR, lex_quoted_string },
		{ CHAR_STATEMENT_SEPARATOR, lex_quoted_string },
		{ CHAR_DOUBLE_QUOTE, lex_quoted_string },
		{ CHAR_WORD_SEPARATOR, lex_quoted_string },
		{ CHAR_OPEN_CURLY_BRACKET, lex_quoted_string },
		{ CHAR_CLOSE_CURLY_BRACKET, lex_quoted_string },
		{ CHAR_OPEN_SQUARE_BRACKET, lex_quoted_string },
		{ CHAR_CLOSE_SQUARE_BRACKET, lex_quoted_string },
		{ CHAR_UTF8_START, lex_quoted_utf8_start },
		{ CHAR_UTF8_CONT, lex_quoted_utf8_cont },
		{ CHAR_QUOTE, lex_end_quoted_string },
	};
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_word_separator(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_WORD_SEPARATOR, lex_word_separator },
		{ CHAR_BACKSLASH, NULL },
		{ CHAR_ASCII_PRINTABLE, NULL },
		{ CHAR_QUOTE, NULL },
		{ CHAR_DOUBLE_QUOTE, NULL },
		{ CHAR_UTF8_START, NULL },
		{ CHAR_OPEN_CURLY_BRACKET, NULL },
		{ CHAR_CLOSE_CURLY_BRACKET, NULL },
		{ CHAR_OPEN_SQUARE_BRACKET, NULL },
		{ CHAR_CLOSE_SQUARE_BRACKET, NULL },
		{ CHAR_NULL, NULL },
	};
	token.token = SCALLOP_TOKEN_WORD_SEPARATOR;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_statement_separator(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_STATEMENT_SEPARATOR, lex_statement_separator },
		{ CHAR_ASCII_PRINTABLE, NULL },
		{ CHAR_UTF8_START, NULL },
		{ CHAR_BACKSLASH, NULL },
		{ CHAR_QUOTE, NULL },
		{ CHAR_DOUBLE_QUOTE, NULL },
		{ CHAR_WORD_SEPARATOR, NULL },
		{ CHAR_OPEN_CURLY_BRACKET, NULL },
		{ CHAR_CLOSE_CURLY_BRACKET, NULL },
		{ CHAR_OPEN_SQUARE_BRACKET, NULL },
		{ CHAR_CLOSE_SQUARE_BRACKET, NULL },
		{ CHAR_NULL, NULL },
	};
	token.token = SCALLOP_TOKEN_STATEMENT_SEPARATOR;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_quoted_string },
		{ CHAR_UTF8_START, lex_quoted_utf8_start },
		{ CHAR_WORD_SEPARATOR, lex_quoted_string },
		{ CHAR_STATEMENT_SEPARATOR, lex_quoted_string },
		{ CHAR_DOUBLE_QUOTE, lex_quoted_string },
		{ CHAR_OPEN_CURLY_BRACKET, lex_quoted_string },
		{ CHAR_CLOSE_CURLY_BRACKET, lex_quoted_string },
		{ CHAR_OPEN_SQUARE_BRACKET, lex_quoted_string },
		{ CHAR_CLOSE_SQUARE_BRACKET, lex_quoted_string },
		{ CHAR_BACKSLASH, lex_escape_quoted_string },
		{ CHAR_QUOTE, lex_end_quoted_string },
	};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_end_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_WORD_SEPARATOR, NULL },
		{ CHAR_STATEMENT_SEPARATOR, NULL },
		{ CHAR_ASCII_PRINTABLE, lex_word },
		{ CHAR_UTF8_START, lex_utf8_start },
		{ CHAR_QUOTE, lex_quoted_string },
		{ CHAR_DOUBLE_QUOTE, lex_double_quoted_string },
		{ CHAR_BACKSLASH, lex_escape_word },
		{ CHAR_OPEN_CURLY_BRACKET, NULL },
		{ CHAR_CLOSE_CURLY_BRACKET, NULL },
		{ CHAR_OPEN_SQUARE_BRACKET, NULL },
		{ CHAR_CLOSE_SQUARE_BRACKET, NULL },
		{ CHAR_NULL, lex_eof },
	};
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_double_quoted_utf8_start(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_UTF8_CONT, lex_double_quoted_utf8_cont },
	};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_double_quoted_utf8_cont(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_double_quoted_string },
		{ CHAR_WORD_SEPARATOR, lex_double_quoted_string },
		{ CHAR_STATEMENT_SEPARATOR, lex_double_quoted_string },
		{ CHAR_QUOTE, lex_double_quoted_string },
		{ CHAR_OPEN_CURLY_BRACKET, lex_double_quoted_string },
		{ CHAR_CLOSE_CURLY_BRACKET, lex_double_quoted_string },
		{ CHAR_OPEN_SQUARE_BRACKET, lex_double_quoted_string },
		{ CHAR_CLOSE_SQUARE_BRACKET, lex_double_quoted_string },
		{ CHAR_UTF8_START, lex_double_quoted_utf8_start },
		{ CHAR_UTF8_CONT, lex_double_quoted_utf8_cont },
		{ CHAR_DOUBLE_QUOTE, NULL },
	};
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_double_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_double_quoted_string },
		{ CHAR_WORD_SEPARATOR, lex_double_quoted_string },
		{ CHAR_STATEMENT_SEPARATOR, lex_double_quoted_string },
		{ CHAR_QUOTE, lex_double_quoted_string },
		{ CHAR_UTF8_START, lex_double_quoted_utf8_start },
		{ CHAR_DOUBLE_QUOTE, lex_end_double_quoted_string },
		{ CHAR_OPEN_CURLY_BRACKET, lex_double_quoted_string },
		{ CHAR_CLOSE_CURLY_BRACKET, lex_double_quoted_string },
		{ CHAR_OPEN_SQUARE_BRACKET, lex_double_quoted_string },
		{ CHAR_CLOSE_SQUARE_BRACKET, lex_double_quoted_string },
		{ CHAR_BACKSLASH, lex_escape_double_quoted_string },
		{ CHAR_NULL, NULL },
	};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct state_transition lex_end_double_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	// identical to lex_end_quoted_string
	return lex_end_quoted_string(token, type);
}

struct state_transition lex_open_curly_bracket(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)type;
	token.token = SCALLOP_TOKEN_OPEN_CURLY_BRACKET;
	return (struct state_transition) {
		NULL,
		token,
	};
}

struct state_transition lex_close_curly_bracket(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)type;
	token.token = SCALLOP_TOKEN_CLOSE_CURLY_BRACKET;
	return (struct state_transition) {
		NULL,
		token,
	};
}

struct state_transition lex_open_square_bracket(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)type;
	token.token = SCALLOP_TOKEN_OPEN_SQUARE_BRACKET;
	return (struct state_transition) {
		NULL,
		token,
	};
}

struct state_transition lex_close_square_bracket(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	(void)type;
	token.token = SCALLOP_TOKEN_CLOSE_SQUARE_BRACKET;
	return (struct state_transition) {
		NULL,
		token,
	};
}

struct state_transition lex_word(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_word },
		{ CHAR_UTF8_START, lex_utf8_start },
		{ CHAR_BACKSLASH, lex_escape_word },
		{ CHAR_QUOTE, lex_quoted_string },
		{ CHAR_DOUBLE_QUOTE, lex_double_quoted_string },
		{ CHAR_WORD_SEPARATOR, NULL },
		{ CHAR_STATEMENT_SEPARATOR, NULL },
		{ CHAR_OPEN_CURLY_BRACKET, NULL },
		{ CHAR_CLOSE_CURLY_BRACKET, NULL },
		{ CHAR_OPEN_SQUARE_BRACKET, NULL },
		{ CHAR_CLOSE_SQUARE_BRACKET, NULL },
		{ CHAR_NULL, NULL },
	};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

static struct state_transition escape_out(
	struct scallop_parse_token token,
	enum CHAR_TYPE type,
	lex_fn *out
)
{
	if (type == CHAR_NULL)
		return (struct state_transition) {
			(void_fn *)lex_error,
			{ 0 },
		};
	token.token = SCALLOP_TOKEN_WORD;
	return (struct state_transition) {
		(void_fn *)out,
		token,
	};
}

struct state_transition lex_escape_word(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	return escape_out(token, type, lex_word);
}

struct state_transition lex_escape_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	return escape_out(token, type, lex_quoted_string);
}

struct state_transition lex_escape_double_quoted_string(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	return escape_out(token, type, lex_double_quoted_string);
}

struct state_transition lex_begin(
	struct scallop_parse_token token,
	enum CHAR_TYPE type
)
{
	static const struct transition_row rows[] = {
		{ CHAR_ASCII_PRINTABLE, lex_word },
		{ CHAR_UTF8_START, lex_utf8_start },
		{ CHAR_QUOTE, lex_quoted_string },
		{ CHAR_DOUBLE_QUOTE, lex_double_quoted_string },
		{ CHAR_WORD_SEPARATOR, lex_word_separator },
		{ CHAR_BACKSLASH, lex_escape_word },
		{ CHAR_STATEMENT_SEPARATOR, lex_statement_separator },
		{ CHAR_OPEN_CURLY_BRACKET, lex_open_curly_bracket },
		{ CHAR_CLOSE_CURLY_BRACKET, lex_close_curly_bracket },
		{ CHAR_OPEN_SQUARE_BRACKET, lex_open_square_bracket },
		{ CHAR_CLOSE_SQUARE_BRACKET, lex_close_square_bracket },
		{ CHAR_NULL, lex_eof },
	};

	return (struct state_transition) {
		transition_state(rows, type),
		token,
	};
}

struct scallop_parse_token needs_next_character(
	csalt_store *source,
	lex_fn *current_state,
	struct scallop_parse_token token
)
{
	const enum CHAR_TYPE type = token.pushback_char ?
		token.pushback_char :
		get_char_type(source, token.end_offset);
	token.pushback_char = 0;

	if (type == CHAR_EAGAIN) {
		token.read_finished = false;
		return token;
	}

	struct state_transition transition = current_state(token, type);

	if (transition.next_state) {
		transition.new_token.end_offset++;
		return needs_next_character(
			source,
			(lex_fn *)transition.next_state,
			transition.new_token
		);
	} else {
		transition.new_token.read_finished = true;
		transition.new_token.pushback_char = type;
		return transition.new_token;
	}
}

struct scallop_parse_token scallop_lex(
	csalt_store *source,
	struct scallop_parse_token token
)
{
	if (token.read_finished) {
		token.start_offset = token.end_offset;
	}
	return needs_next_character(source, lex_begin, token);
}

