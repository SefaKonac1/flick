/* @(#)rpc_scan.c	2.1 88/08/01 4.0 RPCSRC */
/*
 * Copyright (c) 1995, 1996, 1998, 1999 The University of Utah and
 * the Computer Systems Laboratory at the University of Utah (CSL).
 *
 * This file is part of Flick, the Flexible IDL Compiler Kit.
 *
 * Flick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Flick is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Flick; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place #330, Boston, MA 02111, USA.
 */

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
#if 0
#ifndef lint
static char sccsid[] = "@(#)rpc_scan.c 1.6 87/06/24 (C) 1987 SMI";
#endif
#endif

/*
 * rpc_scan.c, Scanner for the RPC protocol compiler 
 * Copyright (C) 1987, Sun Microsystems, Inc. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <mom/aoi.h>
#include <mom/libmeta.h>
#include <mom/compiler.h>
#include "rpc_scan.h"
#include "rpc_util.h"

#define startcomment(where) (where[0] == '/' && where[1] == '*')
#define endcomment(where) (where[-1] == '*' && where[0] == '/')

static void unget_token(token *tokp);

static void findstrconst(char **str, char **val);
static void findconst(char **str, char **val);
static void findkind(char **mark, token *tokp);

static int cppline(char *line);
static int directive(char *line);

static void printdirective(char *line);
static void docppline(char *line, int *lineno, char **fname);

extern io_file_index root_file;
extern io_file_index current_i_file;
extern const char *root_filename;
extern aoi outaoi;

static int pushed = 0;	/* is a token pushed */
static token lasttok;	/* last token, if pushed */

/*
 * scan expecting 1 given token 
 */
void
scan(
	tok_kind expect,
	token *tokp)
{
	get_token(tokp);
	if (tokp->kind != expect) {
		expected1(expect);
	}
}

/*
 * scan expecting 2 given tokens 
 */
void
scan2(
	tok_kind expect1,
	tok_kind expect2,
	token *tokp)
{
	get_token(tokp);
	if (tokp->kind != expect1 && tokp->kind != expect2) {
		expected2(expect1, expect2);
	}
}

/*
 * scan expecting 3 given token 
 */
void
scan3(
	tok_kind expect1,
	tok_kind expect2,
	tok_kind expect3,
	token *tokp)
{
	get_token(tokp);
	if (tokp->kind != expect1 && tokp->kind != expect2
	    && tokp->kind != expect3) {
		expected3(expect1, expect2, expect3);
	}
}


/*
 * scan expecting a constant, possibly symbolic 
 */
void
scan_num(
	token *tokp)
{
	get_token(tokp);
	switch (tokp->kind) {
	case TOK_IDENT:
		break;
	default:
		error("constant or identifier expected");
	}
}


/*
 * Peek at the next token 
 */
void
peek(
	token *tokp)
{
	get_token(tokp);
	unget_token(tokp);
}


/*
 * Peek at the next token and scan it if it matches what you expect 
 */
int
peekscan(
	tok_kind expect,
	token *tokp)
{
	peek(tokp);
	if (tokp->kind == expect) {
		get_token(tokp);
		return (1);
	}
	return (0);
}



/*
 * Get the next token, printing out any directive that are encountered. 
 */
void
get_token(
	token *tokp)
{
	int commenting;

	if (pushed) {
		pushed = 0;
		*tokp = lasttok;
		return;
	}
	commenting = 0;
	for (;;) {
		if (*where == 0) {
			for (;;) {
				if (!fgets(curline, MAXLINESIZE, fin)) {
					tokp->kind = TOK_EOF;
					*where = 0;
					return;
				}
				linenum++;
				if (commenting) {
					break;
				} else if (cppline(curline)) {
					docppline(curline, &linenum, 
						  &infilename);
				} else if (directive(curline)) {
					printdirective(curline);
				} else {
					break;
				}
			}
			where = curline;
		} else if (isspace(((int) *where))) {
			while (isspace(((int) *where))) {
				where++;	/* eat */
			}
		} else if (commenting) {
			where++;
			if (endcomment(where)) {
				where++;
				commenting--;
			}
		} else if (startcomment(where)) {
			where += 2;
			commenting++;
		} else {
			break;
		}
	}

	/*
	 * 'where' is not whitespace, comment or directive Must be a token! 
	 */
	switch (*where) {
	case ':':
		tokp->kind = TOK_COLON;
		where++;
		break;
	case ';':
		tokp->kind = TOK_SEMICOLON;
		where++;
		break;
	case ',':
		tokp->kind = TOK_COMMA;
		where++;
		break;
	case '=':
		tokp->kind = TOK_EQUAL;
		where++;
		break;
	case '*':
		tokp->kind = TOK_STAR;
		where++;
		break;
	case '[':
		tokp->kind = TOK_LBRACKET;
		where++;
		break;
	case ']':
		tokp->kind = TOK_RBRACKET;
		where++;
		break;
	case '{':
		tokp->kind = TOK_LBRACE;
		where++;
		break;
	case '}':
		tokp->kind = TOK_RBRACE;
		where++;
		break;
	case '(':
		tokp->kind = TOK_LPAREN;
		where++;
		break;
	case ')':
		tokp->kind = TOK_RPAREN;
		where++;
		break;
	case '<':
		tokp->kind = TOK_LANGLE;
		where++;
		break;
	case '>':
		tokp->kind = TOK_RANGLE;
		where++;
		break;

	case '"':
		tokp->kind = TOK_STRCONST;
		findstrconst(&where, &tokp->str);
		break;

	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		tokp->kind = TOK_IDENT;
		findconst(&where, &tokp->str);
		break;


	default:
		if (!(isalpha(((int) *where)) || *where == '_')) {
			char buf[100];
			char *p;

			s_print(buf, "illegal character in file: ");
			p = buf + strlen(buf);
			if (isprint(((int) *where))) {
				s_print(p, "%c", *where);
			} else {
				s_print(p, "%d", *where);
			}
			error(buf);
		}
		findkind(&where, tokp);
		break;
	}
}



static void
unget_token(
	token *tokp)
{
	lasttok = *tokp;
	pushed = 1;
}


static void
findstrconst(
	char **str,
	char **val)
{
	char *p;
	int size;

	p = *str;
	do {
		(void) *p++;
	} while (*p && *p != '"');
	if (*p == 0) {
		error("unterminated string constant");
	}
	p++;
	size = p - *str;
	*val = alloc(size + 1);
	(void) strncpy(*val, *str, size);
	(*val)[size] = 0;
	*str = p;
}

static void
findconst(
	char **str,
	char **val)
{
	char *p;
	int size;

	p = *str;
	if (*p == '0' && *(p + 1) == 'x') {
		p++;
		do {
			p++;
		} while (isxdigit(((int) *p)));
	} else {
		do {
			p++;
		} while (isdigit(((int) *p)));
	}
	size = p - *str;
	*val = alloc(size + 1);
	(void) strncpy(*val, *str, size);
	(*val)[size] = 0;
	*str = p;
}



static token symbols[] = {
			  {TOK_CONST, "const"},
			  {TOK_UNION, "union"},
			  {TOK_SWITCH, "switch"},
			  {TOK_CASE, "case"},
			  {TOK_DEFAULT, "default"},
			  {TOK_STRUCT, "struct"},
			  {TOK_TYPEDEF, "typedef"},
			  {TOK_ENUM, "enum"},
			  {TOK_OPAQUE, "opaque"},
			  {TOK_BOOL, "bool"},
			  {TOK_VOID, "void"},
			  {TOK_CHAR, "char"},
			  {TOK_INT, "int"},
			  {TOK_UNSIGNED, "unsigned"},
			  {TOK_SHORT, "short"},
			  {TOK_LONG, "long"},
			  {TOK_FLOAT, "float"},
			  {TOK_DOUBLE, "double"},
			  {TOK_STRING, "string"},
			  {TOK_PROGRAM, "program"},
			  {TOK_VERSION, "version"},
			  {TOK_EOF, "??????"},
};


static void
findkind(
	char **mark,
	token *tokp)
{

	int len;
	token *s;
	char *str;

	str = *mark;
	for (s = symbols; s->kind != TOK_EOF; s++) {
		len = strlen(s->str);
		if (strncmp(str, s->str, len) == 0) {
			if (!isalnum(((int) str[len])) && str[len] != '_') {
				tokp->kind = s->kind;
				tokp->str = s->str;
				*mark = str + len;
				return;
			}
		}
	}
	tokp->kind = TOK_IDENT;
	for (len = 0; isalnum(((int) str[len])) || str[len] == '_'; len++);
	tokp->str = alloc(len + 1);
	(void) strncpy(tokp->str, str, len);
	tokp->str[len] = 0;
	*mark = str + len;
}

static int
cppline(
	char *line)
{
	return (line == curline && *line == '#');
}

static int
directive(
	char *line)
{
	return (line == curline && *line == '%');
}

static void
printdirective(
	char *line)
{
	f_print(fout, "%s", line + 1);
}

static void
docppline(
	char *line,
	int *lineno,
	char **fname)
{
	io_file_index last_i_file = current_i_file;
	int num;
	
	char *filename_start;
	char *filename_end;
	char *file;
	
	/* Skip over the the initial `#' and subsequent whitespace. */
	line++;
	while (isspace(((int) *line))) {
		line++;
	}
	
	/* Skip over the (optional) token `line' and subsequent whitespace. */
	if (!strncmp(line, "line", (sizeof("line") - 1))) {
		line += sizeof("line") - 1; /* `sizeof' counts final NUL. */
		while (isspace(((int) *line))) {
			line++;
		}
	}
	
	/* If we're not at a number (e.g., this is a `pragma'), bail. */
	if (!isdigit(((int) *line)))
		return;
	
	/* Parse the line number and subsequent filename. */
	num = atoi(line);
	while (isdigit(((int) *line))) {
		line++;
	}
	while (isspace(((int) *line))) {
		line++;
	}
	
	for (filename_start = 0; *line; ++line) {
		if (*line == '\"') {
			filename_start = line + 1;
			break;
		}
	}
	if (filename_start) {
		line = filename_start;
		for (filename_end = 0; *line; ++line) {
			if (*line == '\"') {
				filename_end = line;
				break;
			}
		}
	} else
		/* Avoid (bogus) warning from gcc 2.8.0 about uninit'ed use. */
		filename_end = 0;
	
	if (filename_start && filename_end) {
		file = alloc((filename_end - filename_start + 1)
			     * sizeof(char));
		strncpy(file,
			filename_start,
			(filename_end - filename_start));
		file[filename_end - filename_start] = 0;
	} else
		file = 0;
	
	/* Update our scanner state. */
	if (!file) {
		/* No filename found?  Then don't update `fname'. */
	} else if (*file == 0) {
		*fname = "<stdin>";
		/*
		 * *fname = NULL;
		 *
		 * The original code NULL'ed the filename, but doing so can
		 * cause error() in `rpc_util.c' to crash when it sends the
		 * NULL pointer to f_print().
		 */
	} else {
		*fname = file;
	}
	
	/*
	 * Since the root filename was purged of its path, we have to make sure
	 * that we don't try to add the full-path version.  Also, if the root
	 * file includes another file in the same directory or a subdirectory,
	 * then we strip off the pathname of that file, too.
	 *
	 * XXX - This isn't the whole solution.  If the root file includes a
	 * file relative to the parent directory (".."), then the preprocessor
	 * may give us only the exact path of the file, not the relative path.
	 * What we really want is the text inside the #include directive,
	 * rather than exact paths, perhaps with additional information as to
	 * which are system files and user files (e.g. whether included with <>
	 * or "").  Unfortunately, the preprocessor does away with the #include
	 * lines, and only gives us a relative or full pathname, although gcc
	 * is at least gracious enough to also indicate system include files.
	 */
	if (strcmp(*fname, root_filename) != 0) {
		const char *root_dir = dir_part(root_filename);
		int dir_len = strlen(root_dir);
		
		if ((dir_len > 0)
		    && (strncmp(*fname, root_dir, dir_len) == 0)) {
			current_i_file = meta_add_file(&outaoi.meta_data,
						       *fname + dir_len + 1,
						       IO_FILE_INPUT);
		} else {
			current_i_file = meta_add_file(&outaoi.meta_data,
						       *fname,
						       IO_FILE_INPUT);
		}
	} else {
		current_i_file = root_file;
	}
	
	line++;
	for( ; *line && ((*line == ' ') || (*line == '\t')); ++line );
	if( *line && isdigit((int)*line) ) {
		int flag;
		
		flag = atoi(line);
		switch( flag ) {
		case 1:
			break;
		case 2:
			meta_include_file(&outaoi.meta_data,
					  current_i_file,
					  last_i_file);
			break;
		default:
			break;
		}
	}
	*lineno = num - 1;
}

/* End of file. */
