# hiptext - Image to Text Converter
# Copyright (c) 2013 Justine Tunney

# Example invocations:
#  - make                          # Bring hiptext to life.
#  - make check                    # Run unit tests.
#  - sudo make install             # Allow hiptext to stay forever :)
#  - sudo make uninstall           # Kick hiptext out of your house :(
#  - make clean                    # Delete all generated files.
#  - make lint                     # Check for C++ style errors.
#  - CXXFLAGS="-g -O3 -DNDEBUG" make  # Create a faster build.
#  - make -pn | less               # View implicit make rules and variables.

CXX          = clang++
LINK.o       = $(LINK.cc)
PREFIX      ?= /usr/local
TARGET_ARCH ?= -march=native
CXXFLAGS    ?= -g -O3
CXXFLAGS    += -std=c++11 -Wall -Wextra -Werror -pedantic
LDLIBS      += -lm -lglog -lgflags -lpng -ljpeg
LDLIBS      += -lavcodec -lavformat -lavutil -lswscale
LDLIBS      += $(shell freetype-config --libs)

ifeq ($(shell hostname),bean)
CXXFLAGS += -I/usr/include/x86_64-linux-gnu/c++/4.7
endif

SOURCES = \
	artiste.o \
	charquantizer.o \
	font.o \
	graphic.o \
	jpeg.o \
	macterm.o \
	movie.o \
	pixel.o \
	pixel_parse.o \
	png.o \
	termprinter.o \
	unicode.o \
	xterm256.o

all: hiptext
hiptext: hiptext.o $(SOURCES)

.PHONY: check clean install uninstall lint

check: test
	./test --alsologtostderr --gtest_color=yes

clean:
	$(RM) test hiptext $(wildcard *.o *.d *.S $(GTEST_DIR)/*.o)

install: hiptext
	install --mode=0755 hiptext $(PREFIX)/bin

uninstall:
	$(RM) $(PREFIX)/bin/hiptext

lint:
	cpplint.py $(wildcard *.cc hiptext/*.h) \
		2>&1 | grep -v 'termprinter\.cc:.*non-const' \
		     | grep -v 'readability/streams' \
		     | grep -v 'build/include' \
		     | grep -v 'build/header_guard' \
		     | grep -v 'Found C system header after'

%.cc: %.rl
	ragel -o $@ $<

%.S: %.cc
	clang++ $(CXXFLAGS) -g -S -o $@ $<
#	$(COMPILE.cc) -g -S -fverbose-asm $(OUTPUT_OPTION) $<

# Flag overrides for individual targets.
pixel_parse.o: CXXFLAGS := $(filter-out -MD,$(CXXFLAGS))
font.%:        CXXFLAGS += $(shell freetype-config --cflags)

# google-test integration magic.
GTEST_DIR ?= gtest-1.6.0
TESTS = $(GTEST_DIR)/src/gtest-all.o $(GTEST_DIR)/src/gtest_main.o \
        $(patsubst %.cc,%.o,$(wildcard *_test.cc))
$(TESTS): CXXFLAGS += -I$(GTEST_DIR)/include -I$(GTEST_DIR) -pthread
$(filter gtest%,$(TESTS)): CXXFLAGS := $(filter-out -MD -Wall,$(CXXFLAGS))
test: $(TESTS) $(SOURCES) ; $(LINK.cc) $^ $(LDLIBS) -lpthread -o $@

# Recompile sources when headers change.
CXXFLAGS += -MD
-include $(wildcard *.d)
