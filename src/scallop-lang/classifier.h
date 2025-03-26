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
\
#ifndef SCALLOP_LANG_CLASSIFIER
#define SCALLOP_LANG_CLASSIFIER

#ifdef __cplusplus
extern "C" {
#endif

#include <libadt/init.h>

#include <stdbool.h>
#include <wchar.h>

/**
 * \file
 * \brief This file provides the interface for the classifierer.
 *
 * The classifierer is a finite state machine, where each state is a
 * function. Each state takes an input and returns the next state.
 *
 * The classifierer state machine functions take a wide character input and
 * return a classifierer state function. The return value should
 * be cast to a scallop_lang_classifier_fn* before use.
 *
 * The entry point of the finite state machine is
 * scallop_lang_classifier_begin().
 *
 * If the current state receives an unexpected input, it will
 * return scallop_lang_classifier_unexpected().
 *
 * You _must_ check that the return value of the current
 * state is scallop_lang_classifier_unexpected() and handle it
 * correctly. Calling scallop_lang_classifier_unexpected() will
 * call abort().
 *
 * scallop_lang_classifier_end() indicates that the state machine
 * has terminated and no further states apply. You
 * _must_ check for scallop_lang_classifier_end() to indicate
 * that processing finished successfully.
 *
 * Example:
 * \include classifier-example.c
 */

/**
 * \brief Type definition of a "void function".
 *
 * A pointer to this should be used similarly to a pointer
 * to void: its only use is to be passed along to something
 * else, or to be cast to a more useful type.
 */
typedef void *scallop_lang_void_fn();

/**
 * \brief Type definition for a classifierer state function.
 */
typedef scallop_lang_void_fn *scallop_lang_classifier_fn(wint_t);

/**
 * \brief The default entry point of the state machine.
 *
 * Call this on the first character of your shell script
 * to get the first classifiered state.
 */
scallop_lang_classifier_fn *scallop_lang_classifier_begin(wint_t input);

/**
 * \brief Represents the end of a classifier script.
 *
 * In most cases, this is hit when encountering a WEOF character.
 *
 * Some states expect a closing state before encountering
 * a WEOF character, particularly quoted strings.
 *
 * You should use this to terminate your classifier loop.
 *
 * \sa scallop_lang_classifier_fn for an example.
 */
EXPORT extern scallop_lang_classifier_fn *const scallop_lang_classifier_end;

/**
 * \brief Represents an unexpected input for the current state.
 *
 * Use this to test for classifier errors in the script, such as
 * a WEOF in a quoted string, or an unrecognized character.
 *
 * This should be used for checking for errors in your loop and
 * handling them accordingly.
 */
EXPORT extern scallop_lang_classifier_fn *const scallop_lang_classifier_unexpected;

/**
 * \brief Represents an unquoted word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_word(wint_t input);

/**
 * \brief Represents a word separator.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_word_separator(wint_t input);

/**
 * \brief Represents an escape character, '\'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_escape(wint_t input);

/**
 * \brief Represents an opening single quote.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_single_quote(wint_t input);

/**
 * \brief Represents a single quote, closing a single-quoted string.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_single_quote_end(wint_t input);

/**
 * \brief Represents characters in a single-quoted string
 * 	which contribute to a word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_single_quote_word(wint_t input);

/**
 * \brief Represents an opening double quote.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_double_quote(wint_t input);

/**
 * \brief Represents a double quote, closing a double-quoted string.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_double_quote_end(wint_t input);

/**
 * \brief Represents characters in a double-quoted string
 * 	which contribute to a word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_double_quote_word(wint_t input);

/**
 * \brief Represents the opening of a curly bracket block '{'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_curly_block(wint_t input);

/**
 * \brief Represents the closing of a curly bracket block '}'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_curly_block_end(wint_t input);

/**
 * \brief Represents the opening of a square bracket block '['.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_square_block(wint_t input);

/**
 * \brief Represents the closing of a square bracket block ']'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_square_block_end(wint_t input);

/**
 * \brief Represents a statement separator, such as ';' or newline.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_statement_separator(wint_t input);

/**
 * \brief Represents a line comment, starting with a hash '#'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
EXPORT scallop_lang_void_fn *scallop_lang_classifier_line_comment(wint_t input);

/**
 * \brief Tests if a lex type contributes to a word.
 *
 * \param type The type to test.
 *
 * \returns True if the type contributes to a word, false otherwise.
 */
EXPORT inline bool scallop_lang_classifier_is_word(scallop_lang_classifier_fn *type)
{
	return type == scallop_lang_classifier_word
		|| type == scallop_lang_classifier_single_quote
		|| type == scallop_lang_classifier_single_quote_word
		|| type == scallop_lang_classifier_single_quote_end
		|| type == scallop_lang_classifier_double_quote
		|| type == scallop_lang_classifier_double_quote_word
		|| type == scallop_lang_classifier_double_quote_end
		|| type == scallop_lang_classifier_escape;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LANG_CLASSIFIER
