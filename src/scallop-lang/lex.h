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
#ifndef SCALLOP_LANG_LEX
#define SCALLOP_LANG_LEX

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>

/**
 * \file
 */

/**
 * \brief Type definition for a lexer state function.
 *
 * The lexer state machine functions take a wide character input and
 * return a lexer state function. The return value should
 * be cast to a scallop_lang_lex_fn* before use.
 *
 * The entry of the finite state machine is
 * scallop_lang_lex_begin().
 *
 * If the current state receives an unexpected input, it will
 * return scallop_lang_lex_unexpected().
 *
 * You _must_ check that the return value of the current
 * state is scallop_lang_lex_unexpected() and handle it
 * correctly. Calling scallop_lang_lex_unexpected() will
 * call abort().
 *
 * scallop_lang_lex_end() indicates that the state machine
 * has terminated and no further states apply. You
 * _must_ check for scallop_lang_lex_end() to indicate
 * that processing finished successfully.
 *
 * Example:
 * \include lex-example.c
 */
typedef void *scallop_lang_void_fn();
typedef void *(*scallop_lang_lex_fn(wint_t))();

scallop_lang_lex_fn *scallop_lang_lex_begin(wint_t input);

extern scallop_lang_lex_fn *const scallop_lang_lex_end;
extern scallop_lang_lex_fn *const scallop_lang_lex_unexpected;

/**
 * \brief Represents an unquoted word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_word(wint_t input);

/**
 * \brief Represents a word separator.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_word_separator(wint_t input);

/**
 * \brief Represents an escape character, '\'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_escape(wint_t input);

/**
 * \brief Represents an opening single quote.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_single_quote(wint_t input);

/**
 * \brief Represents a single quote, closing a single-quoted string.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_single_quote_end(wint_t input);

/**
 * \brief Represents characters in a single-quoted string
 * 	which contribute to a word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_single_quote_word(wint_t input);

/**
 * \brief Represents an opening double quote.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_double_quote(wint_t input);

/**
 * \brief Represents a double quote, closing a double-quoted string.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_double_quote_end(wint_t input);

/**
 * \brief Represents characters in a double-quoted string
 * 	which contribute to a word.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_double_quote_word(wint_t input);

/**
 * \brief Represents the opening of a curly bracket block '{'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_curly_block(wint_t input);

/**
 * \brief Represents the closing of a curly bracket block '}'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_curly_block_end(wint_t input);

/**
 * \brief Represents the opening of a square bracket block '['.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_square_block(wint_t input);

/**
 * \brief Represents the closing of a square bracket block ']'.
 *
 * \param input The next wide character input.
 * \returns A pointer to the next state function.
 */
scallop_lang_void_fn *scallop_lang_lex_square_block_end(wint_t input);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LANG_LEX
