/*
 * libtextwrap - Text-Wrapping Library with I18N
 * Copyright (C) 2003 by Tomohiro KUBOTA <kubota@debian.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of any author may not be used to endorse or promote
 *    products derived from this software without their specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <langinfo.h>
#include <ctype.h>
#include <wchar.h>

#include "textwrap.h"

typedef struct {char *buf; int size, len;} string_t;

#define ENCODING_TYPE_SIMPLE	0
#define ENCODING_TYPE_UTF8	1
#define ENCODING_TYPE_CJ	2

/* ---------- internal functions ---------- */

static string_t *
stringt_new(int size)
{
  string_t *p;
  p = (string_t *)malloc(sizeof(string_t)); if (!p) return NULL;
  p->buf = (char *)malloc(size); if (!p->buf) { free(p); return NULL; }
  p->size = size;
  p->len = 0;
  p->buf[0] = 0;
  return p;
}

static void
stringt_destroy(string_t *p)
{
  free(p->buf);
  free(p);
  return;
}

static char *
stringt_destroy_extract(string_t *p)
{
  char *s = p->buf;
  free(p);
  return s;
}

static char *
stringt_extract(string_t *p)
{
  return p->buf;
}

static void
stringt_zero(string_t *p)
{
  p->len = 0;
  p->buf[0] = 0;
}

static string_t *
stringt_addstrn(string_t *p, const char *c, int len)
{
  if (c == NULL) return p;
  if (p->len + len + 1 > p->size) {
    p->buf = (char *)realloc(p->buf, (p->size + len + 1) * 2);
    if (!p->buf) { free(p); return NULL;}
    p->size = (p->size + len + 1) * 2;
  }
  strncpy(p->buf + p->len, c, len);
  p->buf[p->len + len] = 0; p->len += len;
  return p;
}

static string_t *
stringt_addstr(string_t *p, const char *c)
{
  if (!c) return p;
  return stringt_addstrn(p, c, strlen(c));
}

static string_t *
stringt_addstringt(string_t *p, string_t *q)
{
  stringt_addstrn(p, q->buf, q->len);
}

/*
 * Since the return value of nl_langinfo() may differ depending on
 * systems (such as "EUC-JP" vs "eucJP"), the encoding names are
 * normalized (all-uppercase, eliminate non-alphanumerics).
 */

static char *
normalize(char *str)
{
  char *out, *p, *q;

  p = str;
  q = out = (char *)malloc(strlen(str) + 1);

  while(*p) {
    *q = toupper(*p++);
    if (isalnum(*q)) q++;
  }
  *q = 0;
  return out;
}

/*
 * "Encoding type" affects breakable().  It doesn't affect number of
 * bytes of one character nor number of columns of one character.
 */

int
set_encoding_type(const char *encoding)
{
  if (!strcmp(encoding, "UTF8")) return ENCODING_TYPE_UTF8;
  else if (strstr(encoding, "EUCKR")) return ENCODING_TYPE_SIMPLE;
  else if (strstr(encoding, "BIG5")) return ENCODING_TYPE_CJ;
  else if (strstr(encoding, "EUC")) return ENCODING_TYPE_CJ;
  else if (strstr(encoding, "GB2312")) return ENCODING_TYPE_CJ;
  return ENCODING_TYPE_SIMPLE;
}

/*
 * "multibyte character" version of wcwidth(3).
 */
static int
mbwidth(const char *ch, int ml)
{
  wchar_t c[2];

  /* 0x00 - 0x7f is always ASCII */
  if ((*ch >= 0 && *ch < 32) || *ch == 127) return -1;
  if (*ch >= 0 && *ch < 127) return 1;
  if (!mbtowc(c, ch, ml)) return -1;
  return wcwidth(c[0]);
}

/*
 * returns 1 if the character allows line-breaking after it.
 * the character is given in "multibyte character" in C meaning,
 * i.e., the encoding specified by LC_CTYPE locale.
 */

static int
breakable(const char *ch, int ml, int encoding_type)
{
  unsigned int u;

  if (*ch == ' ' || *ch == '\t') return 1;
  switch(encoding_type){
  case ENCODING_TYPE_SIMPLE: return 0;
  case ENCODING_TYPE_CJ: return (ml >= 2);
  case ENCODING_TYPE_UTF8:
    switch (ml){
    case 3: /* U+0800 - U+FFFF */
      u = (*ch&0x0f)*0x1000 + (*(ch+1)&0x3f)*0x40 + (*(ch+2)&0x3f);
      if (u >= 0x3000 && u <= 0x312f) {
	if (u == 0x300a || u == 0x300c || u == 0x300e || u == 0x3010
	    || u == 0x3014 || u == 0x3016 || u == 0x3018 || u == 0x301a)
	  return 0;
	return 1;
      }  /* CJK punctuations, Hiragana, Katakana, Bopomofo */
      if (u >= 0x31a0 && u <= 0x31bf) return 1;  /* Bopomofo */
      if (u >= 0x31f0 && u <= 0x31ff) return 1;  /* Katakana extension */
      if (u >= 0x3400 && u <= 0x9fff) return 1;  /* Han Ideogram */
      if (u >= 0xf900 && u <= 0xfaff) return 1;  /* Han Ideogram */
      return 0;
    case 4: /* U+10000 - U+1FFFFF */
      u = (*ch&7)*0x40000 + (*(ch+1)&0x3f)*0x1000 + (*(ch+2)&0x3f)*0x40
	+ (*(ch+3)&0x3f);
      if (u >= 0x20000 && u <= 0x2ffff) return 1;  /* Han Ideogram */
      return 0;
    default: return 0;
    }
  }    
}

/* ---------- public functions ---------- */

void
textwrap_init(textwrap_t *prop)
{
  prop->columns = 76;
  prop->tab = 8;
  prop->head1 = NULL;
  prop->head2 = NULL;
}

void
textwrap_columns(textwrap_t *prop, int columns)
{
  if (columns > 0) prop->columns = columns;
}

void
textwrap_tab(textwrap_t *prop, int tab)
{
  if (tab > 0) prop->tab = tab;
}

void
textwrap_head(textwrap_t *prop, const char *head1, const char *head2)
{
  prop->head1 = head1;
  prop->head2 = head2;
}

char *
textwrap(const textwrap_t *prop, const char *text)
{
  int len;
  string_t *out, *word;
  int additional;
  const char *p;
  int line_width, word_width;
  int head1_width, head2_width;
  char *encoding;
  int encoding_type;
  int columns, tab;
  const char *head1, *head2;

  columns = prop->columns; tab = prop->tab;
  head1 = prop->head1; head2 = prop->head2;

  /* locale initialization */
  encoding = normalize(nl_langinfo(CODESET));
  encoding_type = set_encoding_type(encoding);

  /* buffer initialization */
  len = strlen(text);
  additional = len / columns + columns;
  out = stringt_new(len + additional);
  word = stringt_new(len);

  /* header initialization */
  if (head1 != NULL) {
    for (p = head1, head1_width = 0; *p;) {
      int l = mblen(p, MB_CUR_MAX);
      if (l < 0) {head1 = NULL; head1_width = 0; break;}
      head1_width += mbwidth(p, l); p += l;
    }
    if (head1_width >= columns) {head1 = NULL; head1_width = 0;}
  } else head1_width = 0;

  if (head2 != NULL) {
    for (p = head2, head2_width = 0; *p;) {
      int l = mblen(p, MB_CUR_MAX);
      if (l < 0) {head2 = NULL; head2_width = 0; break;}
      head2_width += mbwidth(p, l); p += l;
    }
    if (head2_width >= columns) {head2 = NULL; head2_width = 0;}
  } else head2_width = 0;

  /* preparation for the main loop */
  p = text; /* next input character */
  stringt_addstr(out, head1);
  line_width = head1_width;
  word_width = 0;

  /* main loop */
  while(1){
    int ml, w, b;
    const char *now;

    if (!*p) {
      stringt_addstringt(out, word);
      return stringt_destroy_extract(out);
    }

    now = p; /* current character */
    ml = mblen(p, MB_CUR_MAX);
    w = mbwidth(p, ml);
    b = breakable(p, ml, encoding_type);
    p += ml;

    /*
     * newline code
     */

    if (*now == '\n') {
      stringt_addstringt(out, word);
      stringt_addstr(out, "\n");
      if (!*p) return stringt_destroy_extract(out);
      stringt_addstr(out, head2); line_width = head2_width;
      stringt_zero(word); word_width = 0;
      continue;
    }

    /*
     * tab code
     */

    if (*now == '\t') {
      if (line_width + word_width >= (columns / tab) * tab) {
	/* new line */
	stringt_addstringt(out, word);
	stringt_addstr(out, "\n");
	if (!*p) return stringt_destroy_extract(out);
	stringt_addstr(out, head2); line_width = head2_width;
	stringt_zero(word); word_width = 0;
      } else {
	/* not new line */
	int j;
	stringt_addstringt(out, word); line_width += word_width;
	stringt_zero(word); word_width = 0;
	j = tab - (line_width + word_width) % tab;
	for( ; j; j--) { stringt_addstr(out, " "); line_width ++; }
      }
      continue;
    }

    if (w < 0 || (*now >= 0 && *now < 0x20)) continue;

    /*
     * when the current line have enough room for the current character
     */

    if (line_width + word_width + w <= columns) {
      if (*now == ' ' || b) {
	stringt_addstringt(out, word);
	stringt_addstrn(out, now, ml);
	line_width += word_width + w;
	stringt_zero(word); word_width = 0;
      } else {
	stringt_addstrn(word, now, ml); word_width += w;
      }
      continue;
    }

    /*
     * when the current line overflows with the current character
     */

    if (*now == ' ') {
      stringt_addstringt(out, word);
      stringt_addstr(out, "\n");
      stringt_addstr(out, head2); line_width = head2_width;
      stringt_zero(word); word_width = 0;
    } else if (word_width + w <= columns) {
      stringt_addstr(out, "\n");
      stringt_addstr(out, head2); line_width = head2_width;
      p = now;
    } else {
      stringt_addstringt(out, word);
      stringt_addstr(out, "\n");
      stringt_addstr(out, head2); line_width = head2_width;
      stringt_zero(word);
      stringt_addstrn(word, now, ml); word_width = w;
    }
  }
}
