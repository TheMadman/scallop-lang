/*
 * Scallop - a shell for executing tasks concurrently
 * Copyright (C) 2022  Marcus Harrison
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
#ifndef SCALLOP_LEXER_H
#define SCALLOP_LEXER_H

#include <csalt/stores.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 *
 * \brief This file defines the data structures and
 *	functions used for lexing.
 */

/**
 * \brief Indicates the kind of token that has been
 * 	read.
 */
enum SCALLOP_TOKEN {
	SCALLOP_TOKEN_EOF,
	SCALLOP_TOKEN_WORD,
	SCALLOP_TOKEN_WORD_SEPARATOR,
	SCALLOP_TOKEN_STATEMENT_SEPARATOR,
	SCALLOP_TOKEN_OPEN_CURLY_BRACKET,
	SCALLOP_TOKEN_CLOSE_CURLY_BRACKET,
	SCALLOP_TOKEN_OPEN_SQUARE_BRACKET,
	SCALLOP_TOKEN_CLOSE_SQUARE_BRACKET,
	SCALLOP_TOKEN_ASSIGNMENT_OPERATOR,
	SCALLOP_TOKEN_PIPE,
	SCALLOP_TOKEN_BINARY_PIPE,
};

/**
 * \brief Represents a single token.
 *
 * If the store would have blocked on a read
 * before the token was fully lexed, the
 * scallop_parse_token::read_finished property will be 0. The
 * resulting scallop_parse_token can be used
 * in a new call to scallop_lex() to continue
 * lexing until the token was lexed completely.
 * Example:
 *
 * \code
 * 	struct scallop_parse_token token = { 0 };
 * 	while (!token.read_finished)
 * 		token = scallop_lex(source, token);
 * 	// use token
 * \endcode
 *
 * The scallop_parse_token::start_offset and
 * scallop_parse_token::end_offset are offsets
 * into the original string, allowing fetching
 * of the original value from the source.
 *
 * Do not pass read-once stores - such as stdin
 * or network-sockets - to scallop_lex(), as
 * the token does not store raw data values. If
 * you want to be able to parse read-once stores,
 * you should use csalt_store_fallback with a
 * store that can persist the original source,
 * such as an initialized csalt_resource_vector.
 *
 * You are expected to use
 * scallop_parse_token::start_offset and
 * scallop_parse_token::end_offset to read out
 * the raw values from the original store.
 */
struct scallop_parse_token {
	/**
	 * \brief The SCALLOP_TOKEN value that the lexer
	 * 	lexed.
	 */
	int32_t token;

	/**
	 * \brief Single character of push-back. Used
	 * 	internally.
	 */
	int32_t pushback_char;

	/**
	 * \brief The byte offset that the value for
	 *	this token starts at.
	 */
	int64_t start_offset;

	/**
	 * \brief The byte offset that the value for
	 *	this token ends at.
	 *
	 * Note that this value represents the character
	 * _after_ the last character of the token.
	 */
	int64_t end_offset;

	/**
	 * \brief The line number that the token
	 * 	begins at.
	 */
	int64_t row;

	/**
	 * \brief The character number on the given line
	 * 	that the token begins at.
	 */
	int64_t col;

	/**
	 * \brief Represents whether the full token was
	 * 	lexed.
	 */
	int8_t read_finished;
};

/**
 * \brief Takes a store representing the UTF-8 source
 * 	code and returns the next token.
 *
 * Pass a zero-initialized token for the beginning of
 * lexing; to continue lexing, pass the previously-returned token.
 *
 * Example:
 * \code
 *	struct scallop_parse_token token = { 0 };
 *	while (!token.read_finished)
 *		token = scallop_lex(source, token);
 *	// use token
 * \endcode
 */
struct scallop_parse_token scallop_lex(
	csalt_store *source,
	struct scallop_parse_token token
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCALLOP_LEXER_H
