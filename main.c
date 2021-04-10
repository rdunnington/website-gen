#include <stdio.h>
#include <memory.h>
#include <ctype.h>

#define RJD_ENABLE_LOGGING 1
#define RJD_ENABLE_ASSERT 1
#define RJD_GFX_BACKEND_NONE 1
#include "rjd.h"

enum token_type
{
	TOKEN_TYPE_TEXT,
	TOKEN_TYPE_HEADER,
	TOKEN_TYPE_LIST_ITEM,
	TOKEN_TYPE_LINK_BEGIN,
	TOKEN_TYPE_LINK_END,
	TOKEN_TYPE_LINK_ADDRESS_BEGIN,
	TOKEN_TYPE_LINK_ADDRESS_END,
	TOKEN_TYPE_COUNT,
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

struct token
{
	const char* text;
	uint32_t length;
	enum token_type type;
};

struct token_stream
{
	const struct token* tokens;
	uint32_t cursor;
	uint32_t indent;
};


void add_indent(struct rjd_strbuf* out, const struct token_stream* stream);
bool peek_token(const struct token_stream* stream, enum token_type type);
struct rjd_result advance_token(struct token_stream* stream);
struct rjd_result consume_token(struct token_stream* stream, enum token_type type);
struct rjd_result parse_text(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_paragraph(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_header(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_list(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_link(struct rjd_strbuf* out, struct token_stream* stream);

void add_indent(struct rjd_strbuf* out, const struct token_stream* stream)
{
	for (size_t i = 0; i < stream->indent; ++i)
	{
		rjd_strbuf_append(out, "\t");
	}
}

bool peek_token(const struct token_stream* stream, enum token_type type)
{
	uint32_t cursor = stream->cursor + 1;

	if (cursor >= rjd_array_count(stream->tokens)) {
		return false;
	}
	if (stream->tokens[cursor].type != type) {
		return false;
	}
	return true;
}

struct rjd_result advance_token(struct token_stream* stream)
{
	++stream->cursor;
	if (stream->cursor >= rjd_array_count(stream->tokens)) {
		return RJD_RESULT("end of token stream");
	}
	printf("advance_token: %c\n", *stream->tokens[stream->cursor].text);
	return RJD_RESULT_OK();
}

struct rjd_result consume_token(struct token_stream* stream, enum token_type type)
{
	advance_token(stream);
	if (stream->tokens[stream->cursor].type != type) {
		return RJD_RESULT("consume_token: unexpected token");
	}
	return RJD_RESULT_OK();
}

struct rjd_result parse_text(struct rjd_strbuf* out, struct token_stream* stream)
{
	bool consuming = true;
	while (consuming)
	{
		const struct token* t = stream->tokens + stream->cursor;
		switch (t->type)
		{
			case TOKEN_TYPE_TEXT:
				rjd_strbuf_appendl(out, t->text, t->length);
				consuming = false;
				break;
			case TOKEN_TYPE_LINK_BEGIN:
				printf("parse_text: begin parsing link\n");
				RJD_RESULT_PROMOTE(parse_link(out, stream));
				consuming = false;
				break;
			default:
				return RJD_RESULT("unexpected token in text stream");
		}
		advance_token(stream);
	}

	return RJD_RESULT_OK();
}

struct rjd_result parse_paragraph(struct rjd_strbuf* out, struct token_stream* stream)
{
	add_indent(out, stream);

	bool is_plain_text = stream->tokens[stream->cursor].type == TOKEN_TYPE_TEXT;

	if (is_plain_text) {
		rjd_strbuf_append(out, "<p>");
	}

	parse_text(out, stream);

	if (is_plain_text) {
		rjd_strbuf_append(out, "</p>");
	}
	rjd_strbuf_append(out, "\n");

	return RJD_RESULT_OK();
}

struct rjd_result parse_header(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;

	RJD_ASSERT(t->type == TOKEN_TYPE_HEADER);
	add_indent(out, stream);
	rjd_strbuf_append(out, "<h1>");

	RJD_RESULT_PROMOTE(advance_token(stream));
	RJD_RESULT_PROMOTE(parse_text(out, stream));
	
	rjd_strbuf_append(out, "</h1>\n");

	return RJD_RESULT_OK();
}

struct rjd_result parse_list(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;

	RJD_ASSERT(t->type == TOKEN_TYPE_LIST_ITEM);
	add_indent(out, stream);
	rjd_strbuf_append(out, "<ul>\n");

	stream->indent += 1;

	while (true)
	{
		add_indent(out, stream);
		rjd_strbuf_append(out, "<li>");
	
		RJD_RESULT_PROMOTE(advance_token(stream));
		parse_text(out, stream);
		rjd_strbuf_append(out, "</li>\n");
		stream->cursor -= 1;

		if (!rjd_result_isok(consume_token(stream, TOKEN_TYPE_LIST_ITEM))) {
			break;
		}
	}

	stream->indent -= 1;

	add_indent(out, stream);
	rjd_strbuf_append(out, "</ul>\n");

	return RJD_RESULT_OK();
}

struct rjd_result parse_link(struct rjd_strbuf* out, struct token_stream* stream)
{
	printf("parse_link\n");
	const struct token* t = stream->tokens + stream->cursor;
	RJD_ASSERT(t->type == TOKEN_TYPE_LINK_BEGIN);

	bool open_new_tab = false;
	const struct token* text = NULL;
	const struct token* address = NULL;

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));
	text = stream->tokens + stream->cursor;
	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_LINK_END));
	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_LINK_ADDRESS_BEGIN));

	if (peek_token(stream, TOKEN_TYPE_LINK_BEGIN)) {
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_LINK_BEGIN));
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));

		const struct token* attribute = stream->tokens + stream->cursor;

		if (!strncmp("newtab", attribute->text, attribute->length)) {
			open_new_tab = true;
		} else {
			return RJD_RESULT("unknown link attribute");
		}
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_LINK_END));
	}

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));
	address = stream->tokens + stream->cursor;
	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_LINK_ADDRESS_END));

	printf("got to end of lnk parse\n");

	rjd_strbuf_append(out, "<a href=\"");
	rjd_strbuf_appendl(out, address->text, address->length);
	rjd_strbuf_append(out, "\"");
	if (open_new_tab) {
		rjd_strbuf_append(out, " target=\"_blank\"");
	}
	rjd_strbuf_append(out, ">");
	rjd_strbuf_appendl(out, text->text, text->length);
	rjd_strbuf_append(out, "</a>");

	//advance_token(stream);

	return RJD_RESULT_OK();
}

int main(int argc, const char** argv)
{
	if (argc < 3) {
		printf("Usage: %s <markdown file> <output file>\n", argv[0]);
		return 0;
	}

	const char* mdFilepath = argv[1];
	const char* htmlFilepath = argv[2];

	struct rjd_mem_allocator alloc = rjd_mem_allocator_init_default();

	size_t file_size = 0;
	struct rjd_result result = rjd_fio_size(mdFilepath, &file_size);
	if (!rjd_result_isok(result)) {
		printf("Failed to get file '%s' size.\n", mdFilepath);
		return 1;
	}

	char* file_contents = NULL;
	result = rjd_fio_read(mdFilepath, &file_contents, &alloc);
	if (!rjd_result_isok(result)) {
		printf("Failed to open file '%s': %s\n", mdFilepath, result.error);
		return 1;
	}

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
			t.type = TOKEN_TYPE_HEADER;
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

	struct rjd_strpool strings = rjd_strpool_init(&alloc, 4096);
	const char** md_lines = rjd_array_alloc(const char*, rjd_array_count(tokens), &alloc);
	struct rjd_strbuf string = rjd_strbuf_init(&alloc);

	struct token_stream stream =
	{
		.tokens = tokens,
		.cursor = 0,
		.indent = 1,
	};

	while (stream.cursor < rjd_array_count(stream.tokens))
	{

		printf("current token: %c\n", *stream.tokens[stream.cursor].text);

		string.length = 0;
		switch (stream.tokens[stream.cursor].type)
		{
			case TOKEN_TYPE_TEXT:
			case TOKEN_TYPE_LINK_BEGIN:
				result = parse_paragraph(&string, &stream);
				break;
			case TOKEN_TYPE_HEADER:
				result = parse_header(&string, &stream);
				break;
			case TOKEN_TYPE_LIST_ITEM:
				result = parse_list(&string, &stream);
				break;
			//case TOKEN_TYPE_LINK_BEGIN:
			//	parse_text(&string, &stream);
			//	//add_indent(&string, &stream);
			//	//result = parse_link(&string, &stream);
			//	//rjd_strbuf_append(&string, "\n");
			//	break;
			default:
				printf("bad token %c\n", *stream.tokens[stream.cursor].text);
				result = RJD_RESULT("unexpected token at top level");
				break;
		}

		if (!rjd_result_isok(result)) {
			printf("Error (%s): %s\n", mdFilepath, result.error);
			break;
		}

		struct rjd_strref* ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		rjd_array_push(md_lines, rjd_strref_str(ref));
	}

	rjd_strbuf_free(&string);

//	for (size_t i = 0; i < rjd_array_count(tokens); ++i)
//	{
//		char tmp[4096];
//		size_t length = rjd_math_minu32(tokens[i].length, 4095);
//		strncpy(tmp, tokens[i].text, length);
//		tmp[length] = 0;
//		printf("type: %d, %s\n", tokens[i].type, tmp);
//	}

	const char* titleString = "";

	const char* header_lines[] = 
	{
		"<!DOCTYPE html>",
		"<html>",
		"<head>",
		titleString,
		"\t<meta charset=\"UTF-8\">",
		"\t<meta name=\"description\" content=\"Personal website with a blog and resume.\">",
		"\t<meta name=\"keywords\" content=\"programming, blog\">",
		"\t<meta name=\"author\" content=\"Reuben Dunnington\">",
		"\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">",
		"\t<link rel=\"stylesheet\" type=\"text/css\" href=\"styles/global.css\">",
		"\t<script async src=\"https://www.googletagmanager.com/gtag/js?id=UA-167453445-1\"></script>",
		"\t<script src=\"script/gtag.js\"></script>",
		"</head>",
		"<body>",
		"\t<nav>",
		"\t\t<a href=\"/\">Home</a>",
		"\t\t<a href=\"/resume\">Resume</a>",
		"\t\t<a href=\"/projects\">Projects</a>",
		"\t\t<a href=\"/blog\">Blog</a>",
		"\t</nav>",
	};

	const char* footer_lines[] = 
	{
		"</body>",
		"</html>",
	};

	FILE* htmlFile = fopen(htmlFilepath, "wt");

	for (size_t i = 0; i < rjd_countof(header_lines); ++i)
	{
		printf("%s\n", header_lines[i]);
		fprintf(htmlFile, "%s\n", header_lines[i]);
	}

	for (size_t i = 0; i < rjd_array_count(md_lines); ++i)
	{
		printf("%s", md_lines[i]);
		fprintf(htmlFile, "%s", md_lines[i]);
	}

	for (size_t i = 0; i < rjd_countof(footer_lines); ++i)
	{
		printf("%s\n", footer_lines[i]);
		fprintf(htmlFile, "%s\n", footer_lines[i]);
	}

	fclose(htmlFile);
	rjd_array_free(md_lines);
	rjd_strpool_free(&strings);

	return 0;
}

