MODULES	:= ftp data_structures util video navdata
SRCDIR	:= src $(addprefix src/,$(MODULES))
BINDIR	:= bin
BINMODS	:= $(addprefix bin/,$(MODULES))
TARGET	:= $(BINDIR)/server.a
SRCS	:= $(shell find $(SRCDIR) -name '*.c')
HEADERS	:= $(shell find $(SRCDIR) -name '*.h')
OBJECTS := $(subst src,$(BINDIR),$(SRCS:%.c=%.o))

FFMPEG_LIBS=    libavdevice                        \
				libavformat                        \
				libavfilter                        \
				libavcodec                         \
				libswresample                      \
				libswscale                         \
				libavutil

vpath $(SRCDIR)

CC		= gcc
CFLAGS	= -Wall -pedantic -extra -Werror -O2 -std=gnu99 -g $(shell pkg-config --cflags $(FFMPEG_LIBS))
LIBS	= -lpthread $(shell pkg-config --libs $(FFMPEG_LIBS))
LDFLAGS	= 
INCLUDES	= -Isrc

.PHONY: all clean

all: $(BINMODS) $(TARGET)

$(BINMODS):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -c $< -o $@

$(SRCS): $(HEADERS)

clean:
	-rm -f $(BINDIR)/*~ $(addsuffix /*.o,$(BINMODS)) $(BINDIR)/*.o $(TARGET)

distclean:: clean
