#include "scallop-lang/lex.h"

#include <stdlib.h>
#include <stdbool.h>
#include <wctype.h>

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
	CLASS_LINE_COMMENT,

	CLASS_UNKNOWN,
} CHARACTER_CLASS;

static bool in(wchar_t c, wchar_t *set)
{
	for (; *set; set++)
		if (c == *set)
			return true;
	return false;
}

static CHARACTER_CLASS get_class(wint_t c)
{
	const bool is_word
		= iswalnum(c)
		|| in((wchar_t)c, L"-_.:");

	if (is_word) {
		return CLASS_WORD;
	}

	switch (c) {
		case L' ':
		case L'\t':
			return CLASS_WORD_SEPARATOR;
		case L'\r':
		case L'\n':
		case L';':
			return CLASS_STATEMENT_SEPARATOR;
		case L'\\':
			return CLASS_ESCAPE;
		case L'\'':
			return CLASS_SINGLE_QUOTE;
		case L'"':
			return CLASS_DOUBLE_QUOTE;
		case L'{':
			return CLASS_BEGIN_CURLY_BLOCK;
		case L'}':
			return CLASS_END_CURLY_BLOCK;
		case L'[':
			return CLASS_BEGIN_SQUARE_BLOCK;
		case L']':
			return CLASS_END_SQUARE_BLOCK;
		case L'#':
			return CLASS_LINE_COMMENT;
		case WEOF:
			return CLASS_EOF;
	}

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
		case CLASS_DOUBLE_QUOTE:
			return scallop_lang_lex_double_quote;
		case CLASS_STATEMENT_SEPARATOR:
			return scallop_lang_lex_statement_separator;
		case CLASS_LINE_COMMENT:
			return scallop_lang_lex_line_comment;
		default:
			return scallop_lang_lex_unexpected;
	}
}

static lex_fn *single_quote_context(wint_t input)
{
	const CHARACTER_CLASS class = get_class(input);
	switch (class) {
		case CLASS_EOF:
			return scallop_lang_lex_unexpected;
		case CLASS_SINGLE_QUOTE:
			return scallop_lang_lex_single_quote_end;
		default:
			return scallop_lang_lex_single_quote_word;
	}
}

static lex_fn *double_quote_context(wint_t input)
{
	const CHARACTER_CLASS class = get_class(input);
	switch (class) {
		case CLASS_EOF:
			return scallop_lang_lex_unexpected;
		case CLASS_DOUBLE_QUOTE:
			return scallop_lang_lex_double_quote_end;
		default:
			return scallop_lang_lex_double_quote_word;
	}
}

static void_fn *lex_end_impl(wint_t c)
{
	(void)c;
	/*
	 * abort()? In MY library code?!
	 *
	 * Yes.
	 *
	 * If you didn't check for lex_end the first time, you
	 * won't check the next time, or the time after
	 * that either.
	 *
	 * Returning NULL from here is _often_ the same
	 * as calling abort(), and sometimes _worse_.
	 *
	 * Just returning &lex_end_impl again will trap
	 * the program in an infinite loop.
	 */
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

void_fn *scallop_lang_lex_statement_separator(wint_t input)
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
	return (void_fn *)single_quote_context(input);
}

void_fn *scallop_lang_lex_single_quote_word(wint_t input)
{
	return (void_fn *)single_quote_context(input);
}

void_fn *scallop_lang_lex_single_quote_end(wint_t input)
{
	return (void_fn *)default_context(input);
}

void_fn *scallop_lang_lex_double_quote(wint_t input)
{
	return (void_fn *)double_quote_context(input);
}

void_fn *scallop_lang_lex_double_quote_word(wint_t input)
{
	return (void_fn *)double_quote_context(input);
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

void_fn *scallop_lang_lex_line_comment(wint_t input)
{
	// We have to treat input specially here, since
	// the only way to end a line comment is a newline
	switch (input) {
		case '\r':
		case '\n':
			return (void_fn *)scallop_lang_lex_statement_separator;
		case WEOF:
			return (void_fn *)scallop_lang_lex_end;
		default:
			return (void_fn *)scallop_lang_lex_line_comment;
	}
}

bool scallop_lang_lex_is_word(scallop_lang_lex_fn *type);
