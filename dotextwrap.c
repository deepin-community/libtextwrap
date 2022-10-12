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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <textwrap.h>

#define BUFFER 1000

int main(int argc, char *argv[])
{
  textwrap_t t;
  char *head1 = NULL, *head2 = NULL, *text, *p;
  int len, size;

  setlocale(LC_ALL, "");

  textwrap_init(&t);
  if (argc >= 2) textwrap_columns(&t, atoi(argv[1]));
  if (argc >= 3) textwrap_tab(&t, atoi(argv[2]));
  if (argc >= 4) head1 = argv[3];
  if (argc >= 5) head2 = argv[4];
  textwrap_head(&t, head1, head2);


  p = text = malloc(BUFFER); size = BUFFER;
  if (text == NULL) {perror("dotextwrap"); exit(1);}
  while(!feof(stdin)) {
    len = fread(p , 1, BUFFER, stdin);
    if (len == BUFFER) {
      text = realloc(text, size += BUFFER); p += BUFFER;
      if (text == NULL) {perror("dotextwrap"); exit(1);}
    } else {
      p[len] = 0; break;
    }
  }

  printf("%s",textwrap(&t, text));
}

