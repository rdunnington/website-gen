#include <stdio.h>
#include <memory.h>
#include <ctype.h>

#define RJD_ENABLE_LOGGING 1
#define RJD_ENABLE_ASSERT 1
#define RJD_GFX_BACKEND_NONE 1
#include "rjd/rjd_all.h"

enum token_type
{
	TOKEN_TYPE_TEXT,
	TOKEN_TYPE_NEWLINE,
	TOKEN_TYPE_HASH,
	TOKEN_TYPE_ASTERISK,
	TOKEN_TYPE_SQUARE_BRACKET_OPEN,
	TOKEN_TYPE_SQUARE_BRACKET_CLOSE,
	TOKEN_TYPE_PAREN_OPEN,
	TOKEN_TYPE_PAREN_CLOSE,
	TOKEN_TYPE_ANGLE_BRACKET_OPEN,
	TOKEN_TYPE_ANGLE_BRACKET_CLOSE,
	TOKEN_TYPE_SLASH_FORWARD,
	TOKEN_TYPE_BACKTICK,
	TOKEN_TYPE_COUNT,
};

const char TOKEN_SYMBOLS[] =
{
	0,
	'\n',
	'#',
	'*',
	'[',
	']',
	'(',
	')',
	'<',
	'>',
	'/',
	'`',
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
	const struct token* first_header_text;
	uint32_t cursor;
	int32_t indent;
};


void append_indent(struct rjd_strbuf* out, const struct token_stream* stream);
bool peek_token(const struct token_stream* stream, enum token_type type);
struct rjd_result advance_token(struct token_stream* stream);
struct rjd_result consume_token(struct token_stream* stream, enum token_type type);
struct rjd_result parse_text(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_paragraph(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_header(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_list(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_link(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_html(struct rjd_strbuf* out, struct token_stream* stream);
struct rjd_result parse_quote(struct rjd_strbuf* out, struct token_stream* stream); 
struct rjd_result parse_code(struct rjd_strbuf* out, struct token_stream* stream); 

void append_indent(struct rjd_strbuf* out, const struct token_stream* stream)
{
	if (stream->indent < 0) {
		printf("stream indent went negative! value: %d", stream->indent);
	}
	for (int i = 0; i < stream->indent; ++i)
	{
		rjd_strbuf_append(out, "\t");
	}
}

void append_token_text(struct rjd_strbuf* out, const struct token* t)
{
	for (uint32_t i = 0; i < t->length; ++i) {
		switch (t->text[i]) {
			case '<':
				rjd_strbuf_append(out, "&lt;");
				break;
			case '>':
				rjd_strbuf_append(out, "&gt;");
				break;
			default:
				rjd_strbuf_appendl(out, t->text + i, 1);
				break;
		}
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

	const struct token* t = stream->tokens + stream->cursor;
	if (t->type == TOKEN_TYPE_NEWLINE) {
		printf("advance_token: {newline}\n");
	} else {
		printf("advance_token: %.*s\n", t->length, t->text);
	}
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
				printf("text_parse: text: %.*s\n", t->length, t->text);
				rjd_strbuf_appendl(out, t->text, t->length);
				break;
			case TOKEN_TYPE_SLASH_FORWARD:
			case TOKEN_TYPE_PAREN_OPEN:
			case TOKEN_TYPE_PAREN_CLOSE:
				rjd_strbuf_appendl(out, t->text, t->length);
				break;
			case TOKEN_TYPE_SQUARE_BRACKET_OPEN:
				//printf("parse_text: begin parsing link\n");
				RJD_RESULT_PROMOTE(parse_link(out, stream));
				break;
			case TOKEN_TYPE_BACKTICK:
				RJD_RESULT_PROMOTE(parse_code(out, stream));
				break;
			default:
				printf("parse_text: got non-text token (%u) '%c'\n", t->type, *t->text);
				consuming = false;
				break;
		}
		advance_token(stream);
	}

	return RJD_RESULT_OK();
}

struct rjd_result parse_paragraph(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;
	printf("parse paragraph beginning with: '%*.s'\n", t->length, t->text);
	append_indent(out, stream);

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

	RJD_ASSERT(t->type == TOKEN_TYPE_HASH);

	int32_t header_type = 0;

	while (t->type == TOKEN_TYPE_HASH) {
		++header_type;
		RJD_RESULT_PROMOTE(advance_token(stream));
		t = stream->tokens + stream->cursor;
	}

	if (stream->first_header_text == NULL) {
		stream->first_header_text = t;
	}

	append_indent(out, stream);
	rjd_strbuf_append(out, "<h%d>", header_type);
	RJD_RESULT_PROMOTE(parse_text(out, stream));
	rjd_strbuf_append(out, "</h%d>\n", header_type);

	return RJD_RESULT_OK();
}

struct rjd_result parse_list(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;

	RJD_ASSERT(t->type == TOKEN_TYPE_ASTERISK);
	append_indent(out, stream);
	rjd_strbuf_append(out, "<ul>\n");

	stream->indent += 1;

	while (true)
	{
		append_indent(out, stream);
		rjd_strbuf_append(out, "<li>");
	
		RJD_RESULT_PROMOTE(advance_token(stream));
		parse_text(out, stream);
		rjd_strbuf_append(out, "</li>\n");
		stream->cursor -= 1;

		if (!rjd_result_isok(consume_token(stream, TOKEN_TYPE_ASTERISK))) {
			break;
		}
	}

	stream->indent -= 1;

	append_indent(out, stream);
	rjd_strbuf_append(out, "</ul>\n");

	return RJD_RESULT_OK();
}

struct rjd_result parse_link(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;
	RJD_ASSERT(t->type == TOKEN_TYPE_SQUARE_BRACKET_OPEN);

	bool open_new_tab = false;
	const struct token* text = NULL;

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));
	text = stream->tokens + stream->cursor;
	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_SQUARE_BRACKET_CLOSE));
	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_PAREN_OPEN));

	if (peek_token(stream, TOKEN_TYPE_SQUARE_BRACKET_OPEN)) {
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_SQUARE_BRACKET_OPEN));
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));

		const struct token* attribute = stream->tokens + stream->cursor;

		if (!strncmp("newtab", attribute->text, attribute->length)) {
			open_new_tab = true;
		} else {
			return RJD_RESULT("unknown link attribute");
		}
		RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_SQUARE_BRACKET_CLOSE));
	}

	RJD_RESULT_PROMOTE(advance_token(stream));
	const struct token* text_begin = stream->tokens + stream->cursor;
	const struct token* text_end = text_begin;

	rjd_strbuf_append(out, "<a href=\"");
	while (text_end->type != TOKEN_TYPE_PAREN_CLOSE)
	{
		rjd_strbuf_appendl(out, text_end->text, text_end->length);
		RJD_RESULT_PROMOTE(advance_token(stream));
		text_end = stream->tokens + stream->cursor;
	}
	rjd_strbuf_append(out, "\"");
	if (open_new_tab) {
		rjd_strbuf_append(out, " target=\"_blank\"");
	}
	rjd_strbuf_append(out, ">");
	rjd_strbuf_appendl(out, text->text, text->length);
	rjd_strbuf_append(out, "</a>");

	return RJD_RESULT_OK();
}

uint32_t find_html_tag_length(const struct token* t)
{
	RJD_ASSERT(t->type == TOKEN_TYPE_TEXT);

	for (uint32_t i = 0; i < t->length; ++i) {
		if (!isspace(t->text[i])) {
			return i;
		}
	}
	return t->length;
}

struct rjd_result parse_html(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;
	RJD_ASSERT(t->type == TOKEN_TYPE_ANGLE_BRACKET_OPEN);

	append_indent(out, stream);
	rjd_strbuf_appendl(out, t->text, t->length);

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));
	t = stream->tokens + stream->cursor;
	const struct token* html_tag = t;
	const uint32_t html_tag_length = find_html_tag_length(t);

	rjd_strbuf_appendl(out, t->text, t->length);
	++stream->indent;

	int32_t tag_count = 1;
	while (tag_count > 0)
	{
		//if (t->type == TOKEN_TYPE_ANGLE_BRACKET_CLOSE) {
		//	rjd_strbuf_append(out, "\n");
		//	append_indent(out, stream);
		//}

		RJD_RESULT_PROMOTE(advance_token(stream));
		t = stream->tokens + stream->cursor;

		//if (t->type == TOKEN_TYPE_ANGLE_BRACKET_OPEN) {
		//	++stream->indent;
		//} else if (t->type == TOKEN_TYPE_ANGLE_BRACKET_CLOSE) {
		//	--stream->indent;
		//}

		const struct token* next = NULL;
		const struct token* next2 = NULL;
		if (stream->cursor + 2 < rjd_array_count(stream->tokens)) {
			next = stream->tokens + stream->cursor + 1;
			next2 = stream->tokens + stream->cursor + 2;
		}

		if (t->type == TOKEN_TYPE_ANGLE_BRACKET_OPEN &&
			next && next->type == TOKEN_TYPE_TEXT)
		{
			uint32_t next_length = find_html_tag_length(next);
			uint32_t length = rjd_math_minu32(html_tag_length, next_length);

			//printf("comparing open:\n\t%.*s\n\t%.*s\n\tmin length: %u\n",
			//		html_tag_length, html_tag->text,
			//		next_length, next->text,
			//		length);
			if (!strncmp(html_tag->text, next->text, length)) {
				//printf("tag_count: %u\n", tag_count);
				++tag_count;
				++stream->indent;
			}
		}

		if (t->type == TOKEN_TYPE_ANGLE_BRACKET_OPEN &&
			next && next->type == TOKEN_TYPE_SLASH_FORWARD &&
			next2 && next2->type == TOKEN_TYPE_TEXT)
		{
			uint32_t next2_length = find_html_tag_length(next2);
			uint32_t length = rjd_math_minu32(html_tag->length, next2_length);

			//printf("comparing close:\n\t%.*s\n\t%.*s\n\tmin length: %u\n",
			//		html_tag_length, html_tag->text,
			//		next2_length, next2->text,
			//		length);

			if (!strncmp(html_tag->text, next2->text, length)) {
				--tag_count;
				--stream->indent;

				//printf("tag_count: %u\n", tag_count);
				if (tag_count == 0) {
					// a bit of a hack, but we didn't know this would be 
					// the end tag when we were adding the previous indent
					if (rjd_strbuf_str(out)[out->length - 1] == '\t') {
						out->length -= 1;
					}
					//printf("found end tag: %.*s\n", next2->length, next2->text);
				}
			}
		}
		//printf("parse_html loop\n");

		rjd_strbuf_appendl(out, t->text, t->length);

		if (t->type == TOKEN_TYPE_NEWLINE) {
			printf("indent: %d", stream->indent);
			append_indent(out, stream);
		}
	}

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_SLASH_FORWARD));
	t = stream->tokens + stream->cursor;
	rjd_strbuf_appendl(out, t->text, t->length);

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_TEXT));
	t = stream->tokens + stream->cursor;
	rjd_strbuf_appendl(out, t->text, t->length);

	RJD_RESULT_PROMOTE(consume_token(stream, TOKEN_TYPE_ANGLE_BRACKET_CLOSE));
	t = stream->tokens + stream->cursor;
	rjd_strbuf_appendl(out, t->text, t->length);

	rjd_strbuf_append(out, "\n");

	advance_token(stream);

	return RJD_RESULT_OK();
}

struct rjd_result parse_quote(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;
	RJD_ASSERT(t->type == TOKEN_TYPE_ANGLE_BRACKET_CLOSE);

	append_indent(out, stream);
	rjd_strbuf_append(out, "<p class=\"quote\">");

	while (t->type == TOKEN_TYPE_ANGLE_BRACKET_CLOSE)
	{
		advance_token(stream);
		RJD_RESULT_PROMOTE(parse_text(out, stream));

		if (peek_token(stream, TOKEN_TYPE_NEWLINE)) {
			advance_token(stream);
		}

		t = stream->tokens + stream->cursor;
		if (t->type == TOKEN_TYPE_ANGLE_BRACKET_CLOSE) {
			rjd_strbuf_append(out, "<br>");
		}
	}

	rjd_strbuf_append(out, "</p>\n");

	return RJD_RESULT_OK();
}

struct rjd_result parse_code(struct rjd_strbuf* out, struct token_stream* stream)
{
	const struct token* t = stream->tokens + stream->cursor;
	RJD_ASSERT(t->type == TOKEN_TYPE_BACKTICK);

	const struct rjd_result result_invalid_multiline_token = RJD_RESULT("Multiline token is 3 backticks on its own line.");

	bool multiline = false;
	if (peek_token(stream, TOKEN_TYPE_BACKTICK)) {
		advance_token(stream);
		if (peek_token(stream, TOKEN_TYPE_BACKTICK)) {
			advance_token(stream);
			if (peek_token(stream, TOKEN_TYPE_NEWLINE)) {
				advance_token(stream);
				multiline = true;
			} else {
				return result_invalid_multiline_token;
			}
		} else {
			return result_invalid_multiline_token;
		}
	}

	if (multiline) {
		append_indent(out, stream);
		rjd_strbuf_append(out, "<pre><code>");
	} else {
		rjd_strbuf_append(out, "<span class=\"inline-code\">");
	}

	RJD_RESULT_PROMOTE(advance_token(stream));
	t = stream->tokens + stream->cursor;

	while (t->type != TOKEN_TYPE_BACKTICK)
	{
		append_token_text(out, t);
		RJD_RESULT_PROMOTE(advance_token(stream));
		t = stream->tokens + stream->cursor;
	}

	if (multiline) {
		if (peek_token(stream, TOKEN_TYPE_BACKTICK)) {
			advance_token(stream);
			if (peek_token(stream, TOKEN_TYPE_BACKTICK)) {
				advance_token(stream);
				if (peek_token(stream, TOKEN_TYPE_NEWLINE)) {
					advance_token(stream);
				} else {
					return result_invalid_multiline_token;
				}
			} else {
				return result_invalid_multiline_token;
			}
		}

		rjd_strbuf_append(out, "</code></pre>\n");
	} else {
		rjd_strbuf_append(out, "</span>");
	}

	return RJD_RESULT_OK();
}

struct rjd_result transform_markdown_file(const char* path_md, const char* path_html, const char* path_root, struct rjd_mem_allocator* alloc)
{
	size_t file_size = 0;
	RJD_RESULT_PROMOTE(rjd_fio_size(path_md, &file_size));

	char* file_contents = NULL;
	RJD_RESULT_PROMOTE(rjd_fio_read(path_md, &file_contents, alloc));

	struct token* tokens = rjd_array_alloc(struct token, 4096, alloc);

	const char* end = file_contents + file_size;
	for (const char* next = file_contents; next < end; )
	{
		//while (isspace(*next) && *next != '\n' && next < end) {
		//while (*next != '\n' && next < end) {
		//	++next;
		//}

		//if (next >= end) {
		//	break;
		//}

		struct token t = {
			.type = TOKEN_TYPE_TEXT, 
			.text = next, 
			.length = 1
		};

		switch (*next)
		{
		case '\n':
			t.type = TOKEN_TYPE_NEWLINE;
			++next;
			break;
		case '#':
			t.type = TOKEN_TYPE_HASH;
			++next;
			break;
		case '*':
			t.type = TOKEN_TYPE_ASTERISK;
			++next;
			break;
		case '[':
			t.type = TOKEN_TYPE_SQUARE_BRACKET_OPEN;
			++next;
			break;
		case ']':
			t.type = TOKEN_TYPE_SQUARE_BRACKET_CLOSE;
			++next;
			break;
		case '(':
			t.type = TOKEN_TYPE_PAREN_OPEN;
			++next;
			break;
		case ')':
			t.type = TOKEN_TYPE_PAREN_CLOSE;
			++next;
			break;
		case '<':
			t.type = TOKEN_TYPE_ANGLE_BRACKET_OPEN;
			++next;
			break;
		case '>':
			t.type = TOKEN_TYPE_ANGLE_BRACKET_CLOSE;
			++next;
			break;
		case '/':
			t.type = TOKEN_TYPE_SLASH_FORWARD;
			++next;
			break;
		case '`':
			t.type = TOKEN_TYPE_BACKTICK;
			++next;
			break;
		default:
			while (next < end) {
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
			break;
		}

		rjd_array_push(tokens, t);
	}

	struct rjd_strpool strings = rjd_strpool_init(alloc, 4096);
	const char** md_lines = rjd_array_alloc(const char*, rjd_array_count(tokens), alloc);
	struct rjd_strbuf string = rjd_strbuf_init(alloc);

	struct token_stream stream =
	{
		.tokens = tokens,
		.first_header_text = NULL,
		.cursor = 0,
		.indent = 1,
	};

	while (stream.cursor < rjd_array_count(stream.tokens))
	{
		const struct token* t = stream.tokens + stream.cursor;
		printf("top-level token (type %d): %.*s\n", t->type, t->length, t->text);

		struct rjd_result result = {0};

		string.length = 0;
		switch (stream.tokens[stream.cursor].type)
		{
			case TOKEN_TYPE_NEWLINE:
				result = advance_token(&stream);
				break;
			case TOKEN_TYPE_TEXT:
			case TOKEN_TYPE_SQUARE_BRACKET_OPEN:
				result = parse_paragraph(&string, &stream);
				break;
			case TOKEN_TYPE_HASH:
				result = parse_header(&string, &stream);
				break;
			case TOKEN_TYPE_ASTERISK:
				result = parse_list(&string, &stream);
				break;
			case TOKEN_TYPE_ANGLE_BRACKET_OPEN:
				result = parse_html(&string, &stream);
				break;
			case TOKEN_TYPE_ANGLE_BRACKET_CLOSE:
				result = parse_quote(&string, &stream);
				break;
			case TOKEN_TYPE_BACKTICK:
				result = parse_code(&string, &stream);
				break;
			default:
				printf("bad token %c\n", *stream.tokens[stream.cursor].text);
				result = RJD_RESULT("unexpected token at top level");
				break;
		}

		if (!rjd_result_isok(result)) {
			printf("Error (%s): %s\n", path_md, result.error);
			break;
		}

		if (string.length > 0) {
			struct rjd_strref* ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
			rjd_array_push(md_lines, rjd_strref_str(ref));
		}
	}

	const char* header_title = "";
	if (stream.first_header_text) {
		const struct token* t = stream.first_header_text;

		rjd_strbuf_clear(&string);
		rjd_strbuf_append(&string, "\t<title>");
		rjd_strbuf_appendl(&string, t->text, t->length);
		rjd_strbuf_append(&string, " | Reuben Dunnington</title>");
		struct rjd_strref* ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		header_title = rjd_strref_str(ref);
	}

	const char* header_css = "";
	const char* header_monokai = "";
	const char* header_gtag = "";
	const char* header_highlight = "";
	{
		struct rjd_strref* ref = NULL;

		rjd_strbuf_clear(&string);
		rjd_strbuf_append(&string, "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"");
		rjd_strbuf_append(&string, path_root);
		rjd_strbuf_append(&string, "styles/global.css\">");
		ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		header_css = rjd_strref_str(ref);

		rjd_strbuf_clear(&string);
		rjd_strbuf_append(&string, "\t<link rel=\"stylesheet\" type=\"text/css\" href=\">");
		rjd_strbuf_append(&string, path_root);
		rjd_strbuf_append(&string, "script/highlight/monokai.css\">");
		ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		header_monokai = rjd_strref_str(ref);

		rjd_strbuf_clear(&string);
		rjd_strbuf_append(&string, "\t<script src=\"");
		rjd_strbuf_append(&string, path_root);
		rjd_strbuf_append(&string, "script/gtag.js\"></script>");
		ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		header_gtag = rjd_strref_str(ref);

		rjd_strbuf_clear(&string);
		rjd_strbuf_append(&string, "\t<script src=\"");
		rjd_strbuf_append(&string, path_root);
		rjd_strbuf_append(&string, "script/script/highlight/highlight.pack.js\"></script>");
		ref = rjd_strpool_add(&strings, rjd_strbuf_str(&string));
		header_highlight = rjd_strref_str(ref);
	}

	rjd_strbuf_free(&string);

	const char* header_lines[] = 
	{
		"<!DOCTYPE html>",
		"<html>",
		"<head>",
		header_title,
		"\t<meta charset=\"UTF-8\">",
		"\t<meta name=\"description\" content=\"Personal website with a blog and resume.\">",
		"\t<meta name=\"keywords\" content=\"programming, blog\">",
		"\t<meta name=\"author\" content=\"Reuben Dunnington\">",
		"\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">",
		header_css,
		header_monokai,
		"\t<script async src=\"https://www.googletagmanager.com/gtag/js?id=UA-167453445-1\"></script>",
		header_gtag,
		header_highlight,
		"\t<script>hljs.initHighlightingOnLoad();</script>",
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

	FILE* htmlFile = fopen(path_html, "wt");

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
	rjd_array_free(tokens);
	rjd_array_free(md_lines);
	rjd_strpool_free(&strings);

	return RJD_RESULT_OK();
}

int main(int argc, const char** argv)
{
	if (argc < 3) {
		printf("Usage: %s <input folder> <output folder>\n", argv[0]);
		return 0;
	}

	struct rjd_mem_allocator alloc = rjd_mem_allocator_init_default();

	const char* path_source = argv[1];
	const char* path_destination = argv[2];

	struct rjd_strbuf system_command = rjd_strbuf_init(&alloc);

	struct rjd_path_enumerator_state path_walker = rjd_path_enumerate_create(path_source, &alloc, RJD_PATH_ENUMERATE_MODE_RECURSIVE);
	for(const char* path_input = rjd_path_enumerate_next(&path_walker);
		path_input != NULL;
		path_input = rjd_path_enumerate_next(&path_walker)) 
	{

		struct rjd_path path_output = rjd_path_init_with(path_input);
		rjd_path_pop_front_path_str(&path_output, path_source);
		rjd_path_join_front(&path_output, path_destination);

		const bool is_markdown = rjd_path_str_endswith(path_input, ".md");
		if (is_markdown) {
			rjd_path_pop_extension(&path_output);
			rjd_path_append(&path_output, ".html");

			struct rjd_path to_root = rjd_path_init();
			struct rjd_path output_copy = path_output;
			rjd_path_pop(&output_copy);
			printf("\n");
			while (output_copy.length != 0) {
				printf("output_copy: %s\n", output_copy.str);
				rjd_path_pop(&output_copy);
				rjd_path_join_str(&to_root, "..");
			}

			const char* path_output_str = rjd_path_get(&path_output);
			struct rjd_result r = transform_markdown_file(path_input, path_output_str, rjd_path_get(&to_root), &alloc);
			if (rjd_result_isok(r)) {
				printf("transform %s -> %s\n", path_input, path_output_str);
			} else {
				printf("Markdown error in file '%s': %s", path_input, r.error);
			}
		} else {
			if (RJD_PLATFORM_WINDOWS) {
				rjd_strbuf_append(&system_command, "copy /y %s %s", path_input, rjd_path_get(&path_output));
			} else {
				rjd_strbuf_append(&system_command, "cp %s %s", path_input, rjd_path_get(&path_output));
			}

			printf("%s\n", rjd_strbuf_str(&system_command));
			system(rjd_strbuf_str(&system_command));
			rjd_strbuf_clear(&system_command);
		}
	}

	rjd_path_enumerate_destroy(&path_walker);

	return 0;
}
