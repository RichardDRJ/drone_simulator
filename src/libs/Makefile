MODULES	:= vrep
SRCDIR	:= . $(addprefix ./,$(MODULES))
BINDIR	:= ../../bin
BINMODS	:= $(addprefix $(BINDIR)/,$(MODULES))
SRCS	:= $(shell find $(SRCDIR) -maxdepth 0 -name '*.c')
HEADERS	:= $(shell find $(SRCDIR) -maxdepth 0 -name '*.h')
OBJECTS := $(subst ./,$(BINDIR)/,$(SRCS:%.c=%.o))

vpath $(SRCDIR)

CC		= gcc
CFLAGS	= -Wall -pedantic -extra -O2 -std=gnu99 -g
LIBS	= -lpthread

.PHONY: all clean

all: $(BINMODS) $(TARGET)

$(BINMODS):
	mkdir -p $@

$(BINDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) -c $< -o $@

$(SRCS): $(HEADERS)

clean:
	-rm -f $(BINDIR)/*~ $(addsuffix /*.o,$(BINMODS)) $(BINDIR)/*.o $(TARGET)
	make -C clean $(LIBDIR)

distclean:: clean
