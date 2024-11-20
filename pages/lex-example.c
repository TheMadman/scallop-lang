#include <stdlib.h>
#include <stdio.h>

#include <scallop-lang/lex.h>

#define lex_fn scallop_lang_lex_fn
#define begin scallop_lang_lex_begin
#define end scallop_lang_lex_end
#define unexpected scallop_lang_lex_unexpected

int main()
{
	lex_fn *old_current = NULL;
	int current_character = 0;
	int current_state_character = 0;
	for (
		lex_fn *current = begin(getwchar());
		current != end;
		current = (lex_fn*)current(getwchar())
	) {
		if (current == unexpected) {
			// Handle error
			return 1;
		}

		// do processing
		// In this example, we're just reporting
		// new state transitions, nothing more complex
		if (current != old_current) {
			printf(
				"State change at: %d\n"
				"Previous state length: %d\n",
				current_character,
				current_state_character
			);
			current_state_character = 0;
			old_current = current;
		}
		current_character++;
		current_state_character++;
	}

	return 0;
}
 
