#Writing a Static Site Generator
###2021-5-4

Up until recently, this site was 100% handcrafted HTML and CSS. With the exception of syntax highlighting taken from [highlightjs]([newtab] https://highlightjs.org/) and some analytics hooks, everything was written manually. It was mostly a learning exercise since as a game programmer I haven’t had much professional exposure to the web. However, after writing up the last post, I decided manually adding tags to each paragraph and inline code wasn't much fun anymore.

There are existing static site generators like [Hugo]([newtab] https://gohugo.io/) and [Jekyll]([newtab] https://jekyllrb.com/), but I didn't want to use an existing theme, and wasn’t interested in taking the time to learn how the template system worked to write my own. I only want to write Markdown with a couple of extra features. This also seemed like a good opportunity to use [rjd]([newtab] https://github.com/rdunnington/rjd) in a real app instead of some samples.

This site has a few elements the generator needed to handle:
* Header/footer
* Body text
* Links
* Lists
* Quotes
* Preformatted text: inline and block
* Custom header items for the resume page

The first attempt simply looked at the next character in the markdown file stream and attempted to generate corresponding HTML. That can work for single-character markup like a `<h1>` if the only other characters on the line belong to the header text. But it quickly fell flat on its face when starting to deal with multi-character tokens like the symbol for `<h3>`, `###`, or other unrelated tokens like links. And especially when you have optional tokens that come after the first, it gets a lot simpler to “peek” ahead in the stream than deal with things character by character.

Soon I realized that Markdown is itself a language and what I actually wanted was a Markdown to HTML compiler. Once my understanding of the problem changed, solving it became a lot more straightforward.

####Tokenizing
Writing a tokenizer, sometimes called a lexer, is very straightforward. Define a list of tokens that act as separators for the stream. Then split the stream into strings paired with the token type. For my purposes, I wound up with:

```
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
```

A couple notes here:
* The definitions are according to the name of the character, not the symbolic meaning of the token. That’s because the token means different things in different contexts. For example, you could name the `*` token `TOKEN_TYPE_LIST_ITEM`. But it’s also used to bold text when it’s not the first token on a line, which isn’t the same thing. This may not be the case for all languages - when there is a 1:1 mapping between the token and the context, it probably makes more sense to give it the symbolic name.
* `TOKEN_TYPE_TEXT` is a catchall for any group of characters delimited by any of the other tokens.

In most of these token cases, it turns out that the token itself is just one character, except for `TOKEN_TYPE_TEXT`. The text token needs to have a reference to the text it represents. You could allocate a string and copy the string out to it, but I opted to just store pointers to the file that was read into memory along with the length of the token:

```
struct token
{
	const char* text;
	uint32_t length;
	enum token_type type;
};
```

Since these are just pointers into the file content, they aren't null-terminated. Thankfully, the C stdlib does contain some tools to deal with this situation. 
* For printing non-null terminated strings, use the format `%.*s`. Usage: `printf("token text: %.*s\n", token.length, token.text);` The `*` character tells printf to look for a supplied length in the varargs. You could change it to a fixed length if you knew the length of the string at compile time, like so: `%128.s`.
* For comparing strings, use `strncmp()` with the minimum of the 2 strings length: `strncmp(token1.text, token2.text, min(token1.length, token2.length))`. Of course, if the length is different, they're probaby not the exact same string to begin with.

####Parsing
With the tokenizer done, the parser was next. I implemented a form of recursive descent parser, in which you have a root function that looks for tokens that can appear at the scope of the current context, then calls functions attempting to parse what they think the token represents. If an invalid token is encountered, an error is returned. 

Since most Markdown is a token followed by text, and then sometimes by an “end” token (.e.g headers or bolded text), most of the parsing functions consume the tokens in their context at the current stream cursor, then call the `parse_text()` function to handle the rest. For example, if we have the line:

```
###Wednesday, April 27, 2021
```

There are 5 tokens on this line:
* `TOKEN_TYPE_HASH`
* `TOKEN_TYPE_HASH`
* `TOKEN_TYPE_HASH`
* `TOKEN_TYPE_TEXT`
* `TOKEN_TYPE_NEWLINE`

The first token is `TOKEN_TYPE_HASH`, which is valid at root scope and denotes a header, so `parse_header()` is called. That will look for more `TOKEN_TYPE_HASH` tokens, to determine what kind of header it is (eg h1, h2, h3, etc). After consuming all the `TOKEN_TYPE_HASH` tokens, it calls `parse_text()`, expecting it to handle the rest of the line. `parse_text()` may look for other optional tokens, such as the start of a link or bold tokens.

This descending approach makes parsing the tokens very straightforward as there are a finite set of valid “next” tokens for a given context. Also, tokens that are present in multiple contexts are handled by the context function, removing the need for any special case code.

As mentioned earlier, the meat of the parser falls into `parse_text()`, which handles regular body text, links, and inline preformatted code. It consumes text tokens, and if it encounters a non-text token, it calls the appropriate parsing method. For example, if it sees a `[`, it will call `parse_link()`. It exits when a newline or end of stream is encountered. It doesn’t handle bolded or italics yet since it hasn't been needed yet, but that is simple to add; it would simply look for the appropriate token, and call the `parse_text_bold()` or `parse_text_italic()` functions.

The most complicated of the parsing functions is `parse_html()`. My [resume](/resume) uses custom div classes for the subsection headers to visually break up the page. I initially toyed with custom tags that would translate to div and class, but realized it was going to be about the same amount of Markdown as just embedding HTML directly into the document. Directly emitting HTML also has the virtue of being simpler than introducing an inevitably leaky or incomplete abstraction. To do this, `parse_html()` looks for `<` tokens, records the first HTML tag it encounters, and refcounts the same begin/end tags. Assuming correctly-formed HTML, that ensures it ends on the last tag; otherwise it would exit early for nested tags of the same type, such as nested divs.

Finally, owning the translation makes it easy to add custom features. Normally links in Markdown always open in the same tab. But I want links to external sites to open in a new tab. To do this, I added support for an optional `[newtab]` in the URL portion of the link. It's a series of 3 tokens, that is optional, so if the first `[` isn't present, the rest is skipped. If it's there, the link is flagged and the emitted HTML will contain a `target=\"_blank\"`. The markdown looks like this: `[MyLink]([newtab] https://url.com)`.

####Wrap-up

In a more demanding language translation, such as from C to machine code, you would typically parse tokens into an abstract syntax tree (AST), then have a final step that emits machine code. That’s necessary to do optimization passes, symbol resolution, and other compiler tasks. However, for a Markdown to HTML translator, creating an AST seemed like overkill, so I opted to just directly emit HTML from the parsing functions. For example, the `parse_header()` function, once it has consumed all `#` tokens, determines the appropriate header tag type, and writes it to the output stream. Then it calls `parse_text()` to handle the rest of the line. Once it returns, `pares_header()` writes the closing tag and returns.

The only HTML Markdown doesn't cover are the header and footer. Since they are largely the same for each page, it is hardcoded and has a few special cases that built paths to the scripts/css folders and the page title.

```
const char* header_title = NULL;
// build header_title
const char* header_lines[] =
{
	"<!DOCTYPE html>",
	"<html>",
	"<head>",
	header_title,
	"\t<meta charset=\"UTF-8\">",
	// etc...
};

const char* footer_lines[] =
{
	"</body>",
	"</html>",
};
```

That’s basically the whole Markdown to HTML translation process. The rest of the site generator recursively walks an input folder, translating each markdown file it finds to a corresponding HTML file in an output directory. Non-markdown files such as .css or .js get a regular file copy to the destination.

You can browse the source to the website generator here: [website-gen]([newtab] https://github.com/rdunnington/website-gen).

Finally, how long does it take to regenerate the whole site? I'm sure Hugo/Jekyll have me beat, but on my 2017 Macbook (1.3ghz dual core i5):
```
> time src/gen markdown rdunnington.github.io
0.16s user 0.13s system 94% cpu 0.301 total
```

No complaints there. :)

