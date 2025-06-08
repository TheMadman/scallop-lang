#include "scallop-lang/parse.h"
#include "scallop-lang/classifier.h"


struct scallop_lang_parse_parser scallop_lang_parse_parser(
	struct libadt_const_lptr script
);
struct scallop_lang_parse_parser scallop_lang_parse_next(
	struct scallop_lang_parse_parser parser
);

