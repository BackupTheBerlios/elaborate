MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I$(top_srcdir)/src/lib \
@EET_CFLAGS@ \
@ETK_CFLAGS@ \
-g -Werror -Wunused

bin_PROGRAMS = elaborate

elaborate_SOURCES = \
el.h \
main.c \
el_gui.h \
el_gui.c \
el_config.h \
el_config.c \
el_config_gui.h \
el_config_gui.c \
el_about_gui.h \
el_about_gui.c

elaborate_LDADD = \
	    $(top_srcdir)/src/lib/libeirc.la \
	    @EET_LIBS@ \
	    @ETK_LIBS@
