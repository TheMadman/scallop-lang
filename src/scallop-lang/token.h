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
	return mbrtowc(
		result,
		string.buffer,
		(size_t)string.length,
		_mbstate
	);
}

typedef struct {
	size_t read;
	scallop_lang_lex_fn *type;
	struct libadt_const_lptr script;
} _scallop_read_t;

inline bool _scallop_read_error(size_t read)
{
	return read == (size_t)-1
		|| read == (size_t)-2;
}

inline _scallop_read_t _scallop_read(
	struct libadt_const_lptr script,
	scallop_lang_lex_fn *const previous
)
{
	wchar_t c = 0;
	mbstate_t mbs = { 0 };
	_scallop_read_t result = { 0 };
	result.read = _scallop_mbrtowc(&c, script, &mbs);
	if (_scallop_read_error(result.read))
		result.type = (scallop_lang_lex_fn*)scallop_lang_lex_unexpected;
	else
		result.type = (scallop_lang_lex_fn*)previous((wint_t)c);

	result.script = libadt_const_lptr_index(script, (ssize_t)result.read);
	return result;
}

inline scallop_lang_lex_fn *_scallop_type(
	scallop_lang_lex_fn *type
)
{
	return scallop_lang_lex_is_word(type) ? scallop_lang_lex_word : type;
}

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
 * \brief Initializes a new token object from a script.
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
	struct scallop_lang_token previous_token
)
{
	const ssize_t value_offset = (char *)previous_token.value.buffer
		- (char *)previous_token.script.buffer;
	struct libadt_const_lptr next = libadt_const_lptr_index(
		previous_token.script,
		value_offset + previous_token.value.length
	);

	_scallop_read_t read = _scallop_read(next, previous_token.type);

	if (_scallop_read_error(read.read)) {
		return (struct scallop_lang_token) {
			.type = scallop_lang_lex_unexpected,
			.script = previous_token.script,
			.value = libadt_const_lptr_truncate(next, 0),
		};
	}

	size_t total_read = read.read;
	scallop_lang_lex_fn *type = read.type;
	for (
		read = _scallop_read(
			read.script,
			previous_token.type
		);
		!_scallop_read_error(read.read);
		read = _scallop_read(read.script, read.type)
	) {
		if (_scallop_type(type) != _scallop_type(read.type))
			break;

		total_read += read.read;

		if (type == scallop_lang_lex_end)
			break;
	}

	return (struct scallop_lang_token) {
		.type = type,
		.script = previous_token.script,
		.value = libadt_const_lptr_truncate(next, total_read),
	};
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LANG_TOKEN
