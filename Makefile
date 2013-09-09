LIBMODS	:= vrep ffmpeg
MODULES	:= ftp data_structures util video navdata control controlcomm $(addprefix libs/,$(LIBMODS))
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
				libswscale                         \
				libavutil

vpath $(SRCDIR)

CC		= gcc
CFLAGS	= -Wall -pedantic -Werror -extra -std=gnu99 -g $(shell pkg-config --cflags $(FFMPEG_LIBS))
LIBS	= -lpthread $(shell pkg-config --libs-only-l $(FFMPEG_LIBS))
#$(addprefix -L,$(BINMODS))
LDFLAGS	= -pthread
DEFS	= -DMAX_EXT_API_CONNECTIONS=255 -DNON_MATLAB_PARSING
INCLUDES	= -Isrc

.PHONY: all clean libav ffmpeg

all: $(BINMODS) $(TARGET) ffmpeg

$(BINMODS):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

$(SRCS): $(HEADERS) ffmpeg

ffmpeg:
	@make -C FFMPEG

clean:
	-rm -f $(BINDIR)/*~ $(addsuffix /*.o,$(BINMODS)) $(BINDIR)/*.o $(TARGET)

distclean:: clean
