
lib_LTLIBRARIES = msg2clip.la

msg2clip_la_SOURCES = msg2clip.c \
	$(sylplugin_factory_source_dir)/src/sylplugin_factory.c

msg2clip_la_LDFLAGS = -module -no-undefined -avoid-version

INCLUDES = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	$(SYLPHEED_CFLAGS)

msg2clip_la_LIBADD = \
	$(GTK_LIBS) \
	$(SYLPHEED_LIBS)

install:
install-plugin: install-am
