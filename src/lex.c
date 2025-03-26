#include "scallop-lang/lex.h"

size_t _scallop_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
bool _scallop_read_error(_scallop_read_t read);
_scallop_read_t _scallop_read(
	struct libadt_const_lptr script,
	scallop_lang_classifier_fn *const previous
);
scallop_lang_classifier_fn *_scallop_type(
	scallop_lang_classifier_fn *type
);
struct scallop_lang_lex scallop_lang_lex_init(
	struct libadt_const_lptr script
);
struct scallop_lang_lex scallop_lang_lex_next_raw(
	struct scallop_lang_lex previous_lex
);
struct scallop_lang_lex scallop_lang_lex_next(
	struct scallop_lang_lex previous_lex
);
size_t _scallop_mbrtowc(
	wchar_t *result,
	struct libadt_const_lptr string,
	mbstate_t *_mbstate
);
ssize_t scallop_lang_lex_normalize_word(
	struct libadt_const_lptr word,
	struct libadt_lptr out
);
