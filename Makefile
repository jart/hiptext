CXX          = clang++
LINK.o       = $(LINK.cc)
TARGET_ARCH ?= -march=native
GTEST_DIR   ?= gtest-1.6.0
CXXFLAGS    += -std=c++11 -Wall -pedantic -g -O2 -pipe -MD
CXXFLAGS    += -I$(GTEST_DIR)/include
CXXFLAGS    += $(shell freetype-config --cflags)
LDFLAGS     += -lm -lpthread -lglog -lgflags -lpng -ljpeg
LDFLAGS     += $(shell freetype-config --libs)

HIPS = font.o utf8.o png.o jpeg.o pixel.o graphic.o xterm256.o \
	charquantizer.o pixel_parse.o xtermprinter.o

all: hiptext

hiptext: hiptext.o $(HIPS)

test: test.o gtest.o $(HIPS) $(patsubst %.cc,%.o,$(wildcard *_test.cc))

gtest.o: $(GTEST_DIR)/src/gtest-all.cc
	$(COMPILE.cc) -I$(GTEST_DIR) $(OUTPUT_OPTION) $<

%.cc: %.rl
	ragel -o $@ $<

check: test
	./test --alsologtostderr --gtest_color=yes --gtest_print_time=1

clean:
	$(RM) hiptext test gtest.o \
	  $(wildcard *.o) \
	  $(wildcard *.d) \
	  $(patsubst %.rl,%.cc,$(wildcard *.rl))

-include $(patsubst %.cc,%.d,$(wildcard *.cc))
.PHONY : clean check
