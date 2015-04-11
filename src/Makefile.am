
lib_LTLIBRARIES = msg2clip.la

msg2clip_la_SOURCES = msg2clip.c

msg2clip_la_LDFLAGS = -module -no-undefined -avoid-version

AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	$(SYLPHEED_CFLAGS)

msg2clip_la_LIBADD = \
	$(GTK_LIBS) \
	$(SYLPHEED_LIBS)

install:
install-plugin: install-am
