/*
 * This file generates parts of LibCSS.
 * Licensed under the MIT License,
 *		  http://www.opensource.org/licenses/mit-license.php
 * Copyright 2010 Vincent Sanders <vince@greyhound-browser.org>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* Descriptors are space separated key:value pairs brackets () are
 * used to quote in values.
 *
 * Examples:
 * list_style_image:CSS_PROP_LIST_STYLE_IMAGE IDENT:( INHERIT: NONE:0,LIST_STYLE_IMAGE_NONE IDENT:) URI:LIST_STYLE_IMAGE_URI
 *
 * list_style_position:CSS_PROP_LIST_STYLE_POSITION IDENT:( INHERIT: INSIDE:0,LIST_STYLE_POSITION_INSIDE OUTSIDE:0,LIST_STYLE_POSITION_OUTSIDE IDENT:)
*/

struct keyval {
	char *key;
	char *val;
};

struct keyval_list {
	struct keyval *item[100];
	int count;
};

struct keyval *get_keyval(char **pos)
{
	char *endpos;
	struct keyval *nkeyval;
	int kvlen;

	endpos = strchr(*pos, ' '); /* single space separated pairs */
	if (endpos == NULL) {
		/* no space, but might be the end of the input */
		kvlen = strlen(*pos);
		if (kvlen == 0)
			return NULL;
		endpos = *pos + kvlen;
	} else {
		kvlen = (endpos - *pos);
	}
	nkeyval = calloc(1, sizeof(struct keyval) + kvlen + 1);

	memcpy(nkeyval + 1, *pos, kvlen);

	nkeyval->key = (char *)nkeyval + sizeof(struct keyval);

	endpos = strchr(nkeyval->key, ':'); /* split key and value on : */
	if (endpos != NULL) {
		endpos[0] = 0; /* change : to null terminator */
		nkeyval->val = endpos + 1; /* skip : */
	}

	*pos += kvlen; /* update position */

	/* skip spaces */
	while ((*pos[0] != 0) &&
	       (*pos[0] == ' ')) {
		(*pos)++;
	}

	return nkeyval;
}

void output_header(FILE *outputf, const char *descriptor, struct keyval *parser_id, bool is_generic)
{
	fprintf(outputf,
		"/*\n"
		" * This file was generated by LibCSS gen_parser \n"
		" * \n"
		" * Generated from:\n"
		" *\n"
		" * %s\n"
		" * \n"
		" * Licensed under the MIT License,\n"
		" *		  http://www.opensource.org/licenses/mit-license.php\n"
		" * Copyright 2010 The Greyhound Browser Project.\n"
		" */\n"
		"\n"
		"#include <assert.h>\n"
		"#include <string.h>\n"
		"\n"
		"#include \"bytecode/bytecode.h\"\n"
		"#include \"bytecode/opcodes.h\"\n"
		"#include \"parse/properties/properties.h\"\n"
		"#include \"parse/properties/utils.h\"\n"
		"\n"
		"/**\n"
		" * Parse %s\n"
		" *\n"
		" * \\param c	  Parsing context\n"
		" * \\param vector  Vector of tokens to process\n"
		" * \\param ctx	  Pointer to vector iteration context\n"
		" * \\param result  resulting style\n"
		"%s"
		" * \\return CSS_OK on success,\n"
		" *	   CSS_NOMEM on memory exhaustion,\n"
		" *	   CSS_INVALID if the input is not valid\n"
		" *\n"
		" * Post condition: \\a *ctx is updated with the next token to process\n"
		" *		   If the input is invalid, then \\a *ctx remains unchanged.\n"
		" */\n"
		"css_error css__parse_%s(css_language *c,\n"
		"		const parserutils_vector *vector, int *ctx,\n"
		"		css_style *result%s)\n"
		"{\n",
		descriptor, 
		parser_id->key, 
		is_generic ? " * \\param op	 Bytecode OpCode for CSS property to encode\n" : "",
		parser_id->key,
		is_generic ? ", enum css_properties_e op" : "");
}


void output_token_type_check(FILE *outputf, bool do_token_check, struct keyval_list *IDENT, struct keyval_list *URI, struct keyval_list *NUMBER)
{
	fprintf(outputf,
		"	int orig_ctx = *ctx;\n"
		"	css_error error;\n"
		"	const css_token *token;\n"
		"	bool match;\n\n"
		"	token = parserutils_vector_iterate(vector, ctx);\n"
		"	if (%stoken == NULL%s",
		do_token_check ? "(" : "",
		do_token_check ? ")" : "");

	if (do_token_check) {
		bool prev = false; /* there was a previous check - add && */
		fprintf(outputf," || (");

		if (IDENT->count > 0) {
			fprintf(outputf,"(token->type != CSS_TOKEN_IDENT)");
			prev = true;
		}
		if (URI->count > 0) {
			if (prev) fprintf(outputf," && ");
			fprintf(outputf,"(token->type != CSS_TOKEN_URI)");
			prev = true;
		}
		if (NUMBER->count > 0) {
			if (prev) fprintf(outputf," && ");
			fprintf(outputf,"(token->type != CSS_TOKEN_NUMBER)");
			prev = true;
		}

		fprintf(outputf,")");
	}

	fprintf(outputf,
		") {\n"
		"\t\t*ctx = orig_ctx;\n"
		"\t\treturn CSS_INVALID;\n"
		"\t}\n\n\t");
}

void output_ident(FILE *outputf, bool only_ident, struct keyval *parseid, struct keyval_list *IDENT)
{
	int ident_count;

	for (ident_count = 0 ; ident_count < IDENT->count; ident_count++) {
		struct keyval *ckv = IDENT->item[ident_count];

		fprintf(outputf,
			"if (");
		if (!only_ident) {
			fprintf(outputf,
			"(token->type == CSS_TOKEN_IDENT) && ");
		}
		fprintf(outputf,
			"(lwc_string_caseless_isequal(token->idata, c->strings[%s], &match) == lwc_error_ok && match)) {\n",
			ckv->key);
		if (strcmp(ckv->key,"INHERIT") == 0) {
		fprintf(outputf,
			"\t\t\terror = css_stylesheet_style_inherit(result, %s);\n",
			parseid->val);
		} else {
		fprintf(outputf,
			"\t\t\terror = css__stylesheet_style_appendOPV(result, %s, %s);\n",
			parseid->val,
			ckv->val);
		}
		fprintf(outputf,
			"\t} else ");
	}
}

void output_uri(FILE *outputf, struct keyval *parseid, struct keyval_list *kvlist)
{
	struct keyval *ckv = kvlist->item[0];

	fprintf(outputf,
		"if (token->type == CSS_TOKEN_URI) {\n"
		"		lwc_string *uri = NULL;\n"
		"		uint32_t uri_snumber;\n"
		"\n"
		"		error = c->sheet->resolve(c->sheet->resolve_pw,\n"
		"				c->sheet->url,\n"
		"				token->idata, &uri);\n"
		"		if (error != CSS_OK) {\n"
		"			*ctx = orig_ctx;\n"
		"			return error;\n"
		"		}\n"
		"\n"
		"		error = css__stylesheet_string_add(c->sheet, uri, &uri_snumber);\n"
		"		if (error != CSS_OK) {\n"
		"			*ctx = orig_ctx;\n"
		"			return error;\n"
		"		}\n"
		"\n"
		"		error = css__stylesheet_style_appendOPV(result, %s, 0, %s);\n"
		"		if (error != CSS_OK) {\n"
		"			*ctx = orig_ctx;\n"
		"			return error;\n"
		"		}\n"
		"\n"
		"		error = css__stylesheet_style_append(result, uri_snumber);\n"
		"	} else ",
		parseid->val,
		ckv->val);
}

void output_number(FILE *outputf, struct keyval *parseid, struct keyval_list *kvlist)
{
	struct keyval *ckv = kvlist->item[0];
	int ident_count;

	fprintf(outputf,
		"if (token->type == CSS_TOKEN_NUMBER) {\n"
		"\t\tcss_fixed num = 0;\n"
		"\t\tsize_t consumed = 0;\n\n"
		"\t\tnum = css__number_from_lwc_string(token->idata, %s, &consumed);\n"
		"\t\t/* Invalid if there are trailing characters */\n"
		"\t\tif (consumed != lwc_string_length(token->idata)) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn CSS_INVALID;\n"
		"\t\t}\n",
		ckv->key);

	for (ident_count = 1 ; ident_count < kvlist->count; ident_count++) {
		struct keyval *ulkv = kvlist->item[ident_count];

		if (strcmp(ulkv->key, "RANGE") == 0) {
			fprintf(outputf,
				"\t\tif (%s) {\n"
				"\t\t\t*ctx = orig_ctx;\n"
				"\t\t\treturn CSS_INVALID;\n"
				"\t\t}\n\n", 
				ulkv->val);
		}

	}

	fprintf(outputf,
		"\t\terror = css__stylesheet_style_appendOPV(result, %s, 0, %s);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n\n"
		"\t\terror = css__stylesheet_style_append(result, num);\n"
		"\t} else ",
		parseid->val,
		ckv->val);	
}

void output_color(FILE *outputf, struct keyval *parseid, struct keyval_list *kvlist)
{
	fprintf(outputf,
		"{\n"
		"\t\tuint16_t value = 0;\n"
		"\t\tuint32_t color = 0;\n"
		"\t\t*ctx = orig_ctx;\n\n"
		"\t\terror = css__parse_colour_specifier(c, vector, ctx, &value, &color);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n\n"
		"\t\terror = css__stylesheet_style_appendOPV(result, %s, 0, value);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n"
		"\n"
		"\t\tif (value == COLOR_SET)\n"
		"\t\t\terror = css__stylesheet_style_append(result, color);\n"
		"\t}\n\n",
		parseid->val);
}

void output_length_unit(FILE *outputf, struct keyval *parseid, struct keyval_list *kvlist)
{
	struct keyval *ckv = kvlist->item[0];
	int ident_count;


	fprintf(outputf,
		"{\n"
		"\t\tcss_fixed length = 0;\n"
		"\t\tuint32_t unit = 0;\n"
		"\t\t*ctx = orig_ctx;\n\n"
		"\t\terror = css__parse_unit_specifier(c, vector, ctx, %s, &length, &unit);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n\n",
		ckv->key);

	for (ident_count = 1 ; ident_count < kvlist->count; ident_count++) {
		struct keyval *ulkv = kvlist->item[ident_count];

		if (strcmp(ulkv->key, "ALLOW") == 0) {
			fprintf(outputf,
				"\t\tif ((%s) == false) {\n"
				"\t\t\t*ctx = orig_ctx;\n"
				"\t\t\treturn CSS_INVALID;\n"
				"\t\t}\n\n", 
				ulkv->val);
		} else if (strcmp(ulkv->key, "DISALLOW") == 0) {
			fprintf(outputf,
				"\t\tif (%s) {\n"
				"\t\t\t*ctx = orig_ctx;\n"
				"\t\t\treturn CSS_INVALID;\n"
				"\t\t}\n\n", 
				ulkv->val);
		} else if (strcmp(ulkv->key, "RANGE") == 0) {
			fprintf(outputf,
				"\t\tif (length %s) {\n"
				"\t\t\t*ctx = orig_ctx;\n"
				"\t\t\treturn CSS_INVALID;\n"
				"\t\t}\n\n", 
				ulkv->val);
		}

	}

	fprintf(outputf,
		"\t\terror = css__stylesheet_style_appendOPV(result, %s, 0, %s);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n"
		"\n"
		"\t\terror = css__stylesheet_style_vappend(result, 2, length, unit);\n"
		"\t}\n\n",
		parseid->val,
		ckv->val);
}

void 
output_ident_list(FILE *outputf, 
		  struct keyval *parseid, 
		  struct keyval_list *kvlist)
{
	struct keyval *ckv = kvlist->item[0]; /* list type : opv value */
	struct keyval *ikv;

	if (strcmp(ckv->key, "STRING_OPTNUM") != 0) {
		fprintf(stderr, "unknown IDENT list type %s\n", ckv->key);
		exit(4);
	}

	if (kvlist->count < 2) {
		fprintf(stderr, "Not enough parameters to IDENT list type %s\n", ckv->key);
		exit(4);
	}

	/* list of IDENT and optional numbers */
	ikv = kvlist->item[1]; /* numeric default : end condition */

	fprintf(outputf,
		"{\n"
		"\t\terror = css__stylesheet_style_appendOPV(result, %s, 0, %s);\n"
		"\t\tif (error != CSS_OK) {\n"
		"\t\t\t*ctx = orig_ctx;\n"
		"\t\t\treturn error;\n"
		"\t\t}\n\n"
		"\t\twhile ((token != NULL) && (token->type == CSS_TOKEN_IDENT)) {\n"
		"\t\t\tuint32_t snumber;\n"
		"\t\t\tcss_fixed num;\n"
		"\t\t\tint pctx;\n\n"
		"\t\t\terror = css__stylesheet_string_add(c->sheet, lwc_string_ref(token->idata), &snumber);\n"
		"\t\t\tif (error != CSS_OK) {\n"
		"\t\t\t\t*ctx = orig_ctx;\n"
		"\t\t\t\treturn error;\n"
		"\t\t\t}\n\n"
		"\t\t\terror = css__stylesheet_style_append(result, snumber);\n"	
		"\t\t\tif (error != CSS_OK) {\n"
		"\t\t\t\t*ctx = orig_ctx;\n"
		"\t\t\t\treturn error;\n"
		"\t\t\t}\n\n"
		"\t\t\tconsumeWhitespace(vector, ctx);\n\n"
		"\t\t\tpctx = *ctx;\n"
		"\t\t\ttoken = parserutils_vector_iterate(vector, ctx);\n"
		"\t\t\tif ((token != NULL) && (token->type == CSS_TOKEN_NUMBER)) {\n"
		"\t\t\t\tsize_t consumed = 0;\n\n"
		"\t\t\t\tnum = css__number_from_lwc_string(token->idata, true, &consumed);\n"
		"\t\t\t\tif (consumed != lwc_string_length(token->idata)) {\n"
		"\t\t\t\t\t*ctx = orig_ctx;\n"
		"\t\t\t\t\treturn CSS_INVALID;\n"
		"\t\t\t\t}\n"
		"\t\t\t\tconsumeWhitespace(vector, ctx);\n\n"
		"\t\t\t\tpctx = *ctx;\n"
		"\t\t\t\ttoken = parserutils_vector_iterate(vector, ctx);\n"
		"\t\t\t} else {\n"
		"\t\t\t\tnum = INTTOFIX(%s);\n"
		"\t\t\t}\n\n"
		"\t\t\terror = css__stylesheet_style_append(result, num);\n"
		"\t\t\tif (error != CSS_OK) {\n"
		"\t\t\t\t*ctx = orig_ctx;\n"
		"\t\t\t\treturn error;\n"
		"\t\t\t}\n\n"
		"\t\t\tif (token == NULL)\n"
		"\t\t\t\tbreak;\n\n"
		"\t\t\tif (token->type == CSS_TOKEN_IDENT) {\n"
		"\t\t\t\terror = css__stylesheet_style_append(result, %s);\n"
		"\t\t\t\tif (error != CSS_OK) {\n"
		"\t\t\t\t\t*ctx = orig_ctx;\n"
		"\t\t\t\t\treturn error;\n"
		"\t\t\t\t}\n"
		"\t\t\t} else {\n"
		"\t\t\t\t*ctx = pctx; /* rewind one token back */\n"
		"\t\t\t}\n"
		"\t\t}\n\n"
		"\t\terror = css__stylesheet_style_append(result, %s);\n"
		"\t}\n\n",
		parseid->val,
		ckv->val,
		ikv->key,
		ckv->val,
		ikv->val);
}

void output_invalidcss(FILE *outputf)
{
	fprintf(outputf, "{\n\t\terror = CSS_INVALID;\n\t}\n\n");
}

void output_footer(FILE *outputf)
{
	fprintf(outputf,
		"	if (error != CSS_OK)\n"
		"		*ctx = orig_ctx;\n"
		"	\n"
		"	return error;\n"
		"}\n\n");
}

void output_wrap(FILE *outputf, struct keyval *parseid, struct keyval_list *WRAP)
{
	struct keyval *ckv = WRAP->item[0];
	fprintf(outputf,
		"	return %s(c, vector, ctx, result, %s);\n}\n",
		ckv->val,
		parseid->val);
}

char str_INHERIT[] = "INHERIT";

struct keyval ident_inherit = {
	.key = str_INHERIT,
};

int main(int argc, char **argv)
{
	char *descriptor;
	char *curpos; /* current position in input string */
	struct keyval *parser_id; /* the parser we are creating output for */
	FILE *outputf;
	struct keyval *rkv; /* current read key:val */
	struct keyval_list *curlist;
	bool do_token_check = true; /* if the check for valid tokens is done */
	bool only_ident = true; /* if the only token type is ident */
	bool is_generic = false;

	struct keyval_list base;
	struct keyval_list IDENT;
	struct keyval_list IDENT_LIST;
	struct keyval_list LENGTH_UNIT;
	struct keyval_list URI;
	struct keyval_list WRAP;
	struct keyval_list NUMBER;
	struct keyval_list COLOR;


	if (argc < 2) {
		fprintf(stderr,"Usage: %s [-o <filename>] <descriptor>\n", argv[0]);
		return 1;
	}

	if ((argv[1][0] == '-') && (argv[1][1] == 'o')) {
		if (argc != 4) {
			fprintf(stderr,"Usage: %s [-o <filename>] <descriptor>\n", argv[0]);
			return 1;
		}
		outputf = fopen(argv[2], "w");
		if (outputf == NULL) {
			perror("unable to open file");
			return 2; /* exit on output file output error */
		}
		descriptor = strdup(argv[3]);
	} else {
		outputf = stdout;
		descriptor = strdup(argv[1]);
	}
	curpos = descriptor;

	base.count = 0;
	IDENT.count = 0;
	URI.count = 0;
	WRAP.count = 0;
	NUMBER.count = 0;
	COLOR.count = 0;
	LENGTH_UNIT.count = 0;
	IDENT_LIST.count = 0;

	curlist = &base;

	while (*curpos != 0) {
		rkv = get_keyval(&curpos);
		if (rkv == NULL) {
			fprintf(stderr,"Token error at offset %ld\n",
					(long)(curpos - descriptor));
			fclose(outputf);
			return 2;
		}

		if (strcmp(rkv->key, "WRAP") == 0) {
			WRAP.item[WRAP.count++] = rkv;
			only_ident = false;
		} else if (strcmp(rkv->key, "NUMBER") == 0) {
			if (rkv->val[0] == '(') {
				curlist = &NUMBER;
			} else if (rkv->val[0] == ')') {
				curlist = &base;
			} else {
				NUMBER.item[NUMBER.count++] = rkv;
			}
			only_ident = false;
		} else if (strcmp(rkv->key, "IDENT") == 0) {
			if (rkv->val[0] == '(') {
				curlist = &IDENT;
			} else if (rkv->val[0] == ')') {
				curlist = &base;
			} else if (strcmp(rkv->val, str_INHERIT) == 0) {
				IDENT.item[IDENT.count++] = &ident_inherit;
			}
		} else if (strcmp(rkv->key, "IDENT_LIST") == 0) {
			if (rkv->val[0] == '(') {
				curlist = &IDENT_LIST;
			} else if (rkv->val[0] == ')') {
				curlist = &base;
			} 
		} else if (strcmp(rkv->key, "LENGTH_UNIT") == 0) {
			if (rkv->val[0] == '(') {
				curlist = &LENGTH_UNIT;
			} else if (rkv->val[0] == ')') {
				curlist = &base;
			} 
			only_ident = false;
			do_token_check = false;
		} else if (strcmp(rkv->key, "COLOR") == 0) {
			COLOR.item[COLOR.count++] = rkv;
			do_token_check = false;
			only_ident = false;
		} else if (strcmp(rkv->key, "URI") == 0) {
			URI.item[URI.count++] = rkv;
			only_ident = false;
		} else if (strcmp(rkv->key, "GENERIC") == 0) {
			is_generic = true;
		} else {
			/* just append to current list */
			curlist->item[curlist->count++] = rkv;
		}
	}

	if (base.count != 1) {
		fprintf(stderr,"Incorrect base element count (got %d expected 1)\n", base.count);
		fclose(outputf);
		return 3;
	}


	/* header */
output_header(outputf, descriptor, base.item[0], is_generic);

	if (WRAP.count > 0) {
		output_wrap(outputf, base.item[0], &WRAP);
	} else {
		/* check token type is correct */
		output_token_type_check(outputf, do_token_check,  &IDENT, &URI, &NUMBER);

		if (IDENT.count > 0)
			output_ident(outputf, only_ident, base.item[0], &IDENT);

		if (URI.count > 0)
			output_uri(outputf, base.item[0], &URI);

		if (NUMBER.count > 0)
			output_number(outputf, base.item[0], &NUMBER);

		/* terminal blocks, these end the ladder ie no trailing else */
		if (COLOR.count > 0) {
			output_color(outputf, base.item[0], &COLOR);
		} else if (LENGTH_UNIT.count > 0) {
			output_length_unit(outputf, base.item[0], &LENGTH_UNIT);
		} else if (IDENT_LIST.count > 0) {
			output_ident_list(outputf, base.item[0], &IDENT_LIST);
		} else {
			output_invalidcss(outputf);
		}

		output_footer(outputf);

	}

	fclose(outputf);

	return 0;
}
