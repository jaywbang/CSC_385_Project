#ifndef SCANNER_H
#define SCANNER_H

struct token1
{
	struct source1 *src;
	int text_len;
	char *text;
};

extern struct token1 endoffile_token;

struct token1 *tokenize(struct source1 *src);
void freetoken(struct token1 *tok);

#endif
