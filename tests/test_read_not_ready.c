#include "test_macros.h"

#include <csalt/stores.h>
#include <string.h>

ssize_t not_ready_read(csalt_store *store, void *buffer, ssize_t size)
{
	// first read reports 0 bytes, subsequent reads report size bytes
	static int ready = 0;
	if (!ready++) {
		return 0;
	} else {
		memset(buffer, 0, (size_t)size);
		return size;
	}
}

ssize_t not_ready_write(csalt_store *store, const void *buffer, ssize_t size)
{
	return size;
}

ssize_t not_ready_size(csalt_store *store)
{
	return 0;
}

int not_ready_split(
	csalt_store *store,
	ssize_t begin,
	ssize_t end,
	csalt_store_block_fn *block,
	void *param
)
{
	return block(store, param);
}

const struct csalt_store_interface not_ready_interface = {
	not_ready_read,
	not_ready_write,
	not_ready_size,
	not_ready_split,
};

csalt_store not_ready = &not_ready_interface;

int main()
{
	csalt_store *store = &not_ready;
	struct scallop_parse_token token = { 0 };

	token = scallop_lex(store, token);
	assert(!token.read_finished);

	token = scallop_lex(store, token);
	assert(token.read_finished);
	assert(token.token == SCALLOP_TOKEN_EOF);
}
