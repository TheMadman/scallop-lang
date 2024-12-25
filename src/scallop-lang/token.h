/*
 * Scallop - A Shell Language for Parallelization (Language Definition)
 * Copyright (C) 2024  Marcus Harrison
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

#ifndef SCALLOP_LANG_TOKEN
#define SCALLOP_LANG_TOKEN

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>

#include <libadt/lptr.h>

#include "lex.h"

/**
 * \file
 *
 * \brief This module provides an API over the lex finite state
 * 	machine, generating tokens from multibyte character scripts.
 */

/**
 * \brief Represents a single token.
 */
struct scallop_lang_token {
	/**
	 * \brief Represents the type of token lexed.
	 */
	scallop_lang_lex_fn *type;

	/**
	 * \brief A pointer to the full script.
	 */
	struct libadt_const_lptr script;

	/**
	 * \brief A pointer to the lexed value.
	 *
	 * This will always be a pointer into .script.
	 */
	struct libadt_const_lptr value;
};

inline size_t _scallop_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
)
{
	if (string.length <= 0) {
		// when does this break?
		*result = (wchar_t)WEOF;
		return 0;
	}
	return mbrtowc(
		result,
		string.buffer,
		(size_t)string.length,
		_mbstate
	);
}

typedef struct {
	size_t amount;
	scallop_lang_lex_fn *type;
	struct libadt_const_lptr script;
} _scallop_read_t;

inline bool _scallop_read_error(_scallop_read_t read)
{
	return read.amount == (size_t)-1
		|| read.amount == (size_t)-2
		|| read.type == scallop_lang_lex_unexpected;
}

inline _scallop_read_t _scallop_read(
	struct libadt_const_lptr script,
	scallop_lang_lex_fn *const previous
)
{
	wchar_t c = 0;
	mbstate_t mbs = { 0 };
	_scallop_read_t result = { 0 };
	result.amount = _scallop_mbrtowc(&c, script, &mbs);
	if (_scallop_read_error(result))
		result.type = (scallop_lang_lex_fn*)scallop_lang_lex_unexpected;
	else
		result.type = (scallop_lang_lex_fn*)previous((wint_t)c);

	result.script = libadt_const_lptr_index(script, (ssize_t)result.amount);
	return result;
}

/**
 * \brief Initializes a token object for use in scallop_lang_token_next().
 *
 * \param script The script to create a token from.
 *
 * \returns A token, valid for passing to scallop_lang_token_next().
 */
inline struct scallop_lang_token scallop_lang_token_init(
	struct libadt_const_lptr script
)
{
	return (struct scallop_lang_token) {
		.type = (scallop_lang_lex_fn*)scallop_lang_lex_begin,
		.script = script,
		.value = libadt_const_lptr_truncate(script, 0),
	};
}

/**
 * \brief Returns the next, raw token in the script referred to by
 * 	previous.
 *
 * Raw tokens contain raw types, such as scallop_lang_lex_double_quote.
 * Separate tokens are returned for beginning quote, the quoted word,
 * and end quote, as well as any consecutive word tokens without
 * separators.
 *
 * It is recommended to use scallop_lang_token_next(), which will
 * return a single scallop_lang_lex_word token in that scenario.
 *
 * \param previous A token returned by scallop_lang_token_init() or
 * 	scallop_lang_token_next_raw().
 *
 * \returns A new token.
 */
inline struct scallop_lang_token scallop_lang_token_next_raw(
	struct scallop_lang_token previous
)
{
	const ssize_t value_offset = (char *)previous.value.buffer
		- (char *)previous.script.buffer;
	struct libadt_const_lptr next = libadt_const_lptr_index(
		previous.script,
		value_offset + previous.value.length
	);

	_scallop_read_t
		read = _scallop_read(next, previous.type),
		previous_read = read;

	if (_scallop_read_error(read))
		return (struct scallop_lang_token) {
			.script = previous.script,
			.type = scallop_lang_lex_unexpected,
			.value = libadt_const_lptr_truncate(next, 0),
		};

	if (read.type == scallop_lang_lex_end) {
		return (struct scallop_lang_token) {
			.script = previous.script,
			.type = read.type,
			.value = libadt_const_lptr_truncate(next, read.amount)
		};
	}

	size_t value_length = read.amount;
	for (
		read = _scallop_read(read.script, read.type);
		!_scallop_read_error(read);
		read = _scallop_read(read.script, read.type)
	) {
		if (read.type != previous_read.type)
			break;

		previous_read = read;
		value_length += read.amount;
	}

	return (struct scallop_lang_token) {
		.script = previous.script,
		.type = previous_read.type,
		.value = libadt_const_lptr_truncate(next, value_length),
	};
}

/**
 * \brief Returns the next token in the script referred to by previous.
 *
 * Word tokens will always have scallop_lang_lex_word
 * type, even for words that contain quoted words or
 * escaped characters.
 *
 * Separators will be grouped into a single token.
 * If the value contains a statement separator, the
 * type is always scallop_lang_lex_statement_separator,
 * even if it also contains word separators.
 *
 * If it contains only word separators, the value is
 * scallop_lang_lex_word_separator.
 *
 * \param previous A token previously returned by
 * 	scallop_lang_token_next(), or initialized from
 * 	scallop_lang_token_init().
 *
 * \returns A token succeeding scallop_lang_token_complete()
 * 	if successful, or failing if an incomplete multibyte
 * 	character was encountered.
 */
inline struct scallop_lang_token scallop_lang_token_next(
	struct scallop_lang_token previous
)
{
	struct scallop_lang_token result = scallop_lang_token_next_raw(
		previous
	);

	const bool end = result.type == scallop_lang_lex_end
		|| result.type == scallop_lang_lex_unexpected;
	if (end)
		return result;

	// This does a tonne more work than necessary but
	// I'm lazy
	struct scallop_lang_token next = scallop_lang_token_next(
		result
	);

	const bool is_word = scallop_lang_lex_is_word(result.type)
		&& scallop_lang_lex_is_word(next.type);
	if (is_word)
		result.value.length += next.value.length;

	const bool has_word_separator
		= result.type == scallop_lang_lex_word_separator
		|| next.type == scallop_lang_lex_word_separator;
	const bool has_statement_separator
		= result.type == scallop_lang_lex_statement_separator
		|| next.type == scallop_lang_lex_statement_separator;
	if (has_word_separator && has_statement_separator) {
		result.type = scallop_lang_lex_statement_separator;
		result.value.length += next.value.length;
	}
	return result;
}

/**
 * \brief Takes a word value from scallop_lang_token_next() and
 * 	normalizes it to the raw word value.
 *
 * The result is written to `out`. Writing stops when the end of
 * the word is reached, or when `out` runs out of space.
 *
 * `word` is not tested for actual word contents. The behaviour for
 * `word` containing non-word values is undefined.
 *
 * \param word A value from scallop_lang_token_next() that contains
 * 	a word.
 * \param out A pointer to the location to write to.
 *
 * \returns The number of characters written, or -1 if an error
 * 	occurred.
 */
inline ssize_t scallop_lang_token_normalize_word(
	struct libadt_const_lptr word,
	struct libadt_lptr out
)
{
	size_t read_amount = 0;
	ssize_t total_read_amount = 0;

	scallop_lang_lex_fn
		*current = (scallop_lang_lex_fn *)scallop_lang_lex_begin;
	for (
		;
		libadt_const_lptr_in_bounds(word)
		&& libadt_lptr_in_bounds(out);
		word = libadt_const_lptr_index(word, (ssize_t)read_amount)
	) {
		wchar_t c = 0;
		mbstate_t state = { 0 };
		read_amount = _scallop_mbrtowc(&c, word, &state);

		const bool read_error = read_amount == (size_t)-1
			|| read_amount == (size_t)-2;

		if (read_error)
			return -1;

		current = (scallop_lang_lex_fn *)current((wint_t)c);

		const bool is_error_type = current == scallop_lang_lex_unexpected;

		if (is_error_type)
			return -1;

		const bool skip_type = current == scallop_lang_lex_single_quote
			|| current == scallop_lang_lex_single_quote_end
			|| current == scallop_lang_lex_double_quote
			|| current == scallop_lang_lex_double_quote_end
			|| current == scallop_lang_lex_escape;

		if (!skip_type) {
			libadt_lptr_memmove(out, libadt_const_lptr_truncate(word, read_amount));
			out = libadt_lptr_index(out, (ssize_t)read_amount);
			total_read_amount += (ssize_t)read_amount;
		}
	}

	return total_read_amount;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LANG_TOKEN
