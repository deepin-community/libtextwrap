lib_LTLIBRARIES = libtextwrap.la

libtextwrap_la_SOURCES = textwrap.c
libtextwrap_la_LDFLAGS = -version-info 1:0:0

include_HEADERS = textwrap.h
man_MANS = textwrap.3 dotextwrap.1

bin_PROGRAMS = dotextwrap
dotextwrap_SOURCES = dotextwrap.c
dotextwrap_LDADD = -ltextwrap
