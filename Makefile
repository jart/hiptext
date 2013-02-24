CXX=clang++
LINK.o=$(LINK.cc)
TARGET_ARCH=-march=native
CXXFLAGS=-std=c++11 -Wall -pedantic -g -O2 -pipe -MD \
	$(shell freetype-config --cflags)
LDFLAGS=-lm -lglog -lgflags -lpng -ljpeg \
	$(shell freetype-config --libs)
SRCS=hiptext.cc utf8.cc png.cc pixel.cc graphic.cc xterm256.cc
OBJS=$(SRCS:.cc=.o)
DEPS=$(OBJS:.o=.d)

all: hiptext
hiptext: $(OBJS)

clean:
	$(RM) hiptext $(OBJS) $(DEPS)

-include $(DEPS)
