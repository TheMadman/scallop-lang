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

#ifndef SCALLOP_LANG_PARSE
#define SCALLOP_LANG_PARSE

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>

#include <libadt/lptr.h>

#include "classifier.h"

/**
 * \file
 */

/**
 * \brief Represents a single, classifiered lex, for further
 * 	use with the parser API.
 */
struct scallop_lang_parse_lex {
	/**
	 * \brief A pointer to the string for the given
	 * 	lex. For example, if a word is classifiered,
	 * 	this will point to the full (unprocessed)
	 * 	word.
	 *
	 * If .type is scallop_lang_classifier_word, this will
	 * include quoted and escaped chunks as well.
	 */
	struct libadt_const_lptr value;

	/**
	 * \brief The type of lex that was classifiered.
	 */
	scallop_lang_classifier_fn *type;
};

/**
 * \brief Represents a parser for a single script.
 */
struct scallop_lang_parse_parser {
	/**
	 * \brief The script to parse.
	 *
	 * The script must be a multibyte character array.
	 */
	struct libadt_const_lptr script;

	/**
	 * \brief The last lex parsed.
	 *
	 * Will be a null lex if no parsing has been
	 * performed.
	 */
	struct scallop_lang_parse_lex lex;


	mbstate_t _mbstate;
	wint_t _pushback;
};

/**
 * \brief Constructs a parser from a multibyte script.
 */
inline struct scallop_lang_parse_parser scallop_lang_parse_parser(
	struct libadt_const_lptr script
)
{
	return (struct scallop_lang_parse_parser) {
		.script = script,
		._mbstate = { 0 },

		// why do you make me do this GCC
		.lex = {{ 0 }},
	};
}

typedef struct {
	wint_t character;
	struct scallop_lang_parse_parser next;
} _scallop_dequeue_t;

// Gets a wide character from the multibyte
// script in parser and progresses parser
static inline _scallop_dequeue_t _scallop_dequeue(
	struct scallop_lang_parse_parser parser
)
{
	if (parser._pushback) {
		wint_t result = parser._pushback;
		parser._pushback = 0L;
		return (_scallop_dequeue_t) {
			result,
			parser,
		};
	}

	if (!libadt_const_lptr_in_bounds(parser.script)) {
		return (_scallop_dequeue_t) {
			0L,
			parser,
		};
	}

	wchar_t result = 0;
	size_t amount_read = mbrtowc(
		&result,
		parser.script.buffer,
		(size_t)parser.script.length,
		&parser._mbstate
	);

	const bool
		incomplete = amount_read == (size_t)-2,
		invalid = amount_read == (size_t)-1;

	if (incomplete || invalid) {
		return (_scallop_dequeue_t) {
			0L,
			parser,
		};
	}

	parser.script = libadt_const_lptr_index(
		parser.script,

		// in principle, this breaks for values
		// between SSIZE_MAX and SIZE_MAX.
		// In practice, if you have a single multibyte
		// character that long, you deserve what's coming.
		(ssize_t)amount_read
	);

	return (_scallop_dequeue_t) {
		(wint_t)result,
		parser,
	};
}

static inline struct scallop_lang_parse_lex _scallop_parse_word(
	struct scallop_lang_parse_parser parser
)
{

}

inline struct scallop_lang_parse_parser scallop_lang_parse_next(
	struct scallop_lang_parse_parser parser
)
{
	for (
		_scallop_dequeue_t current = _scallop_dequeue(parser);
		current.character;
		current = _scallop_dequeue(parser)
	) {
		wint_t character = current.character;
		parser = current.next;
		parser.lex.type = (scallop_lang_classifier_fn *)parser.lex.type(character);
	}
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LANG_PARSE
