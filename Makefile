LIBMODS	:= vrep libav
MODULES	:= ftp data_structures util video navdata control $(addprefix libs/,$(LIBMODS))
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
LIBS	= -lpthread $(addprefix -L,$(BINMODS)) $(shell pkg-config --libs-only-l $(FFMPEG_LIBS))
LDFLAGS	= 
DEFS	= -DMAX_EXT_API_CONNECTIONS=255 -DNON_MATLAB_PARSING
INCLUDES	= -Isrc

.PHONY: all clean libav

all: $(BINMODS) $(TARGET)

$(BINMODS):
	mkdir -p $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFS) -c $< -o $@

$(SRCS): libav $(HEADERS)

libav: bin/libs/libav/
	@echo "Building libav..."
	@#@cd libav-9.8 && ./configure --enable-gpl --enable-libx264 --enable-libxvid --enable-version3 --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libmp3lame --enable-libspeex --enable-libvorbis --enable-libtheora --enable-libvpx --enable-libopenjpeg --enable-libfreetype --enable-doc --enable-shared --disable-libopenjpeg --disable-libspeex --disable-programs --disable-doc  --incdir=$(CURDIR)/src/libs/libav/ --libdir=$(CURDIR)/bin/libs/libav/ --shlibdir=$(CURDIR)/bin/libs/libav/ --bindir=$(CURDIR)/bin/libs/libav/presets && 
	make -C libav-9.8 && make -C libav-9.8 install

clean:
	-rm -f $(BINDIR)/*~ $(addsuffix /*.o,$(BINMODS)) $(BINDIR)/*.o $(TARGET)

distclean:: clean
