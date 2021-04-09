#include <stdio.h>
#include <memory.h>
#include <ctype.h>

#define RJD_ENABLE_ASSERT 1
#define RJD_GFX_BACKEND_NONE 1
#include "rjd.h"

enum token_type
{
	TOKEN_TYPE_TEXT,
	TOKEN_TYPE_HEADER1,
	TOKEN_TYPE_LIST_ITEM,
	TOKEN_TYPE_LINK_BEGIN,
	TOKEN_TYPE_LINK_END,
	TOKEN_TYPE_LINK_ADDRESS_BEGIN,
	TOKEN_TYPE_LINK_ADDRESS_END,
	TOKEN_TYPE_COUNT,
};

struct token
{
	const char* text;
	uint32_t length;
	enum token_type type;
};

const char TOKEN_SYMBOLS[] =
{
	0,
	'#',
	'*',
	'[',
	']',
	'(',
	')',
};
RJD_STATIC_ASSERT(rjd_countof(TOKEN_SYMBOLS) == TOKEN_TYPE_COUNT);

int main(int argc, const char** argv)
{
	if (argc < 2) {
		printf("Usage: %s <markdown file>\n", argv[0]);
		return 0;
	}

	struct rjd_mem_allocator alloc = rjd_mem_allocator_init_default();

	size_t file_size = 0;
	struct rjd_result result = rjd_fio_size(argv[1], &file_size);
	if (!rjd_result_isok(result)) {
		printf("Failed to get file '%s' size.\n", argv[1]);
		return 1;
	}

	char* file_contents = NULL;
	result = rjd_fio_read(argv[1], &file_contents, &alloc);
	if (!rjd_result_isok(result)) {
		printf("Failed to open file '%s': %s\n", argv[1], result.error);
		return 1;
	}

	struct rjd_strpool strings = rjd_strpool_init(&alloc, 4096);
	struct token* tokens = rjd_array_alloc(struct token, 4096, &alloc);

	const char* end = file_contents + file_size;
	for (const char* next = file_contents; next < end; )
	{
		while (isspace(*next) && next < end) {
			++next;
		}

		if (next >= end) {
			break;
		}

		struct token t = {
			.type = TOKEN_TYPE_TEXT, 
			.text = next, 
			.length = 1
		};

		switch (*next)
		{
		case '#':
			t.type = TOKEN_TYPE_HEADER1;
			++next;
			break;
		case '*':
			t.type = TOKEN_TYPE_LIST_ITEM;
			++next;
			break;
		case '[':
			t.type = TOKEN_TYPE_LINK_BEGIN;
			++next;
			break;
		case ']':
			t.type = TOKEN_TYPE_LINK_END;
			++next;
			break;
		case '(':
			t.type = TOKEN_TYPE_LINK_ADDRESS_BEGIN;
			++next;
			break;
		case ')':
			t.type = TOKEN_TYPE_LINK_ADDRESS_END;
			++next;
			break;
		default:
			while (*next != '\n' && next < end) {
				bool end_token = false;
				for (size_t i = 0; i < rjd_countof(TOKEN_SYMBOLS); ++i) {
					if (*next == TOKEN_SYMBOLS[i]) {
						end_token = true;
						break;
					}
				}
				if (end_token) {
					break;
				}
				++next;
			}
			t.length = next - t.text;
		}

		rjd_array_push(tokens, t);
	}

	for (size_t i = 0; i < rjd_array_count(tokens); ++i)
	{
		char tmp[4096];
		size_t length = rjd_math_minu32(tokens[i].length, 4095);
		strncpy(tmp, tokens[i].text, length);
		tmp[length] = 0;
		printf("type: %d, %s\n", tokens[i].type, tmp);
	}

	rjd_strpool_free(&strings);

	return 0;
}

