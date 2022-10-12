libtextwrap - Text-Wrapping Library with I18N


Introduction
------------

Word-wrapping (or line-folding) is one of frequently used text-processing.
It is implemented very easily only when it is based on several assumptions
such as:

 - One character consists from one byte.
 - One character occupies one column on the terminal.
 - A line can be folded between words which are separated by whitespaces.

All of these assumptions are *local* and valid only for some
part of languages in the world.  There are languages which don't meet
the assumptions and speakers of such languages cannot use softwares
which depend on these assumptions.

Since it is usually difficult to require developers to know various
languages in the world and their properties, text-processing
algorithms should be supplied as libraries.  Otherwise, their softwares
cannot support various languages in the world and people in the world
suffer from a problem that many softwares are useless for them.


Feature
-------

On calling a function with parameters of text and maximum columns,
one can get line-folded text.  The text is encoded in current LC_CTYPE
locale.  This means that libtextwrap uses UTF-8 for I/O in UTF-8 locales,
ISO-8859-1 in ISO-8859-1 locales, EUC-JP in EUC-JP locales, KOI8-R in
KOI8-R locales, and so on.


Download
--------

Please visit http://libtextwrap.sourceforge.net/ .


Compilation
-----------

When you are using released version, do like following:

$ ./configure
$ make
$ make install

When you are using CVS version, you will have to run

$ ./bootstrap

before invoking ./configure script.  You will need autoconf,
automake, and libtool in this case.


Usage
-----

See the manpage of textwrap(3).  Like following:

#include <stdio.h>
#include <locale.h>
#include <textwrap.h>
int main(int argc, char **argv)
{
    textwrap_t property;

    setlocale(LC_ALL, "");
    textwrap_init(&property);
    textwrap_columns(&property, 40);

    printf("Input string:\n%s\n", argv[1]);
    printf("Formatted string:\n%s\n",
        textwrap(&property, argv[1]));
}


Sample Program
--------------

dotextwrap.c is a test program to invoke libtextwrap.
It is compiled and installed at the same time as libtextwrap.
Read dotextwrap(1) for detail.

sample/testprog.pl is a test program for libtextwrap with
multilingual text.  Just invoke it.


License
-------

Copyright (c) 2003 by Tomohiro KUBOTA <kubota@debian.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of any author may not be used to endorse or promote
   products derived from this software without their specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.



The sample texts in sample/testprog.pl are citations from Debian website
(http://www.debian.org/).

Copyright (c) 1997-2003 Software in the Public Interest, Inc.
P.O. Box 502761
Indianapolis, IN 46250-2761
United States

This material may be distributed only subject to the terms and conditions
set forth in the Open Publication License, Draft v1.0 or later (the latest
version is presently available at http://www.opencontent.org/openpub/). 

"Debian" and the Debian Logo are trademarks of Software in the Public
Interest, Inc.
