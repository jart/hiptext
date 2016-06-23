AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I config

EXTRA_DIST = \
	DejaVuSansMono.ttf \
	README.md \
	autogen.sh \
	balls.png \
	obama.jpg \
	$(shell find gtest -type f | grep -v /.deps/ | grep -v \\.o\$)

bin_PROGRAMS = hiptext
noinst_LIBRARIES = libhiptext.a libgtest.a

distclean-local:
	rm -f src/pixel_parse.cc

################################################################################
## libhiptext.a

libhiptext_a_SOURCES = \
	src/artiste.cc \
	src/charquantizer.cc \
	src/css_color.rl \
	src/font.cc \
	src/graphic.cc \
	src/hiptext.cc \
	src/hiptext/artiste.h \
	src/hiptext/charquantizer.h \
	src/hiptext/font.h \
	src/hiptext/graphic.h \
	src/hiptext/jpeg.h \
	src/hiptext/macterm.h \
	src/hiptext/movie.h \
	src/hiptext/pixel.h \
	src/hiptext/png.h \
	src/hiptext/sixelprinter.h \
	src/hiptext/termprinter.h \
	src/hiptext/unicode.h \
	src/hiptext/unused.h \
	src/hiptext/xterm256.h \
	src/jpeg.cc \
	src/macterm.cc \
	src/movie.cc \
	src/pixel.cc \
	src/pixel_parse.cc \
	src/pixel_parse.rl \
	src/png.cc \
	src/sixelprinter.cc \
	src/termprinter.cc \
	src/unicode.cc \
	src/xterm256.cc

libhiptext_a_CPPFLAGS = \
	-Isrc \
	$(LIBAVCODEC_CFLAGS) \
	$(LIBAVFORMAT_CFLAGS) \
	$(LIBAVUTIL_CFLAGS) \
	$(LIBFREETYPE_CFLAGS) \
	$(LIBGFLAGS_CFLAGS) \
	$(LIBGLOG_CFLAGS) \
	$(LIBPNG_CFLAGS) \
	$(LIBSWSCALE_CFLAGS)

################################################################################
## hiptext

hiptext_SOURCES = src/hiptext.cc

hiptext_CPPFLAGS = $(libhiptext_a_CPPFLAGS)

hiptext_LDADD = \
	libhiptext.a \
	-ljpeg \
	$(LIBAVCODEC_LIBS) \
	$(LIBAVFORMAT_LIBS) \
	$(LIBAVUTIL_LIBS) \
	$(LIBFREETYPE_LIBS) \
	$(LIBGFLAGS_LIBS) \
	$(LIBGLOG_LIBS) \
	$(LIBPNG_LIBS) \
	$(LIBSWSCALE_LIBS)

################################################################################
## libgtest

libgtest_a_SOURCES = \
	gtest/src/gtest-all.cc

libgtest_a_CPPFLAGS = \
	$(PTHREAD_CFLAGS) \
	-Igtest \
	-Igtest/include

################################################################################
## test

check_PROGRAMS = hiptext_test
TESTS = $(check_PROGRAMS)

hiptext_test_SOURCES = \
	test/pixel_test.cc \
	test/xterm256_test.cc \
	test/test.cc

hiptext_test_CPPFLAGS = \
	$(hiptext_CPPFLAGS) \
	$(PTHREAD_CFLAGS) \
	-Igtest/include

hiptext_test_LDADD = \
	libgtest.a \
	$(hiptext_LDADD) \
	$(PTHREAD_LIBS) \
	$(PTHREAD_CFLAGS)  # XXX: Not sure why this is needed.

################################################################################
## miscellaneous

src/%.cc: src/%.rl
	@echo "  RL      " $@;
	@$(RAGEL) -G2 -o $@ $<
