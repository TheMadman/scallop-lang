#include "scallop-lang/token.h"

size_t _scallop_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _scallop_read_error(size_t read);
_scallop_read_t _scallop_read(
	struct libadt_const_lptr script,
	scallop_lang_lex_fn *const previous
);
scallop_lang_lex_fn *_scallop_type(
	scallop_lang_lex_fn *type
);
struct scallop_lang_token scallop_lang_token_init(
	struct libadt_const_lptr script
);
struct scallop_lang_token scallop_lang_token_next(
	struct scallop_lang_token previous_token
);
size_t _scallop_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _scallop_read_error(size_t read);
