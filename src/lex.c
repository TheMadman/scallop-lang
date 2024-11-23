#include "scallop-lang/lex.h"

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define void_fn scallop_lang_void_fn
#define lex_fn scallop_lang_lex_fn

typedef enum {
	CLASS_EOF,
	CLASS_WORD,
	CLASS_WORD_SEPARATOR,
	CLASS_STATEMENT_SEPARATOR,
	CLASS_ESCAPE,
	CLASS_SINGLE_QUOTE,
	CLASS_DOUBLE_QUOTE,
	CLASS_BEGIN_CURLY_BLOCK,
	CLASS_END_CURLY_BLOCK,
	CLASS_BEGIN_SQUARE_BLOCK,
	CLASS_END_SQUARE_BLOCK,

	CLASS_UNKNOWN,
} CHARACTER_CLASS;

static CHARACTER_CLASS get_class(wint_t c)
{
	const bool is_word
		= isalnum(c)
		|| c == '-'
		|| c == '_';

	if (is_word) {
		return CLASS_WORD;
	}

	switch (c) {
		case ' ':
		case '\t':
			return CLASS_WORD_SEPARATOR;
		case '\r':
		case '\n':
		case ';':
			return CLASS_STATEMENT_SEPARATOR;
		case '\\':
			return CLASS_ESCAPE;
		case '\'':
			return CLASS_SINGLE_QUOTE;
		case '"':
			return CLASS_DOUBLE_QUOTE;
		case '{':
			return CLASS_BEGIN_CURLY_BLOCK;
		case '}':
			return CLASS_END_CURLY_BLOCK;
		case '[':
			return CLASS_BEGIN_SQUARE_BLOCK;
		case ']':
			return CLASS_END_SQUARE_BLOCK;
	}

	if (c == WEOF)
		return CLASS_EOF;

	return CLASS_UNKNOWN;
}

static lex_fn *default_context(wint_t input)
{
	switch (get_class(input)) {
		case CLASS_EOF:
			return scallop_lang_lex_end;
		case CLASS_WORD:
			return scallop_lang_lex_word;
		case CLASS_WORD_SEPARATOR:
			return scallop_lang_lex_word_separator;
		case CLASS_ESCAPE:
			return scallop_lang_lex_escape;
		case CLASS_SINGLE_QUOTE:
			return scallop_lang_lex_single_quote;
		default:
			return scallop_lang_lex_unexpected;
	}
}

static lex_fn *single_quoted_context(wint_t input)
{
	if (input == WEOF)
		return scallop_lang_lex_unexpected;
	if (input == '\'')
		return scallop_lang_lex_single_quote_end;
	return scallop_lang_lex_single_quoted_word;
}

static lex_fn *double_quoted_context(wint_t input)
{
	if (input == WEOF)
		return scallop_lang_lex_unexpected;
	if (input == '"')
		return scallop_lang_lex_double_quote_end;
	return scallop_lang_lex_double_quoted_word;
}

static void_fn *lex_end_impl(wint_t c)
{
	(void)c;
	abort();
	return (void_fn *)&lex_end_impl;
}

static void_fn *lex_unexpected_impl(wint_t c)
{
	return lex_end_impl(c);
}

lex_fn *const scallop_lang_lex_end = (lex_fn *)&lex_end_impl;
lex_fn *const scallop_lang_lex_unexpected = (lex_fn *)&lex_unexpected_impl;

lex_fn *scallop_lang_lex_begin(wint_t input)
{
	return default_context(input);
}

void_fn *scallop_lang_lex_word(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_word_separator(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_escape(wint_t input)
{
	if (input == WEOF)
		return (void_fn *)scallop_lang_lex_unexpected;
	return (void_fn *)scallop_lang_lex_word;
}

void_fn *scallop_lang_lex_single_quote(wint_t input)
{
	return (void_fn *)single_quoted_context(input);
}

void_fn *scallop_lang_lex_single_quoted_word(wint_t input)
{
	return (void_fn *)single_quoted_context(input);
}

void_fn *scallop_lang_lex_single_quote_end(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_double_quote(wint_t input)
{
	return (void_fn *)double_quoted_context(input);
}

void_fn *scallop_lang_lex_double_quoted_word(wint_t input)
{
	return (void_fn *)double_quoted_context(input);
}

void_fn *scallop_lang_lex_double_quote_end(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_curly_block(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_curly_block_end(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_square_block(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_square_block_end(wint_t input)
{
	return (void_fn *)default_context(input);
}
