MODULES	:= ftp data_structures util
SRCDIR	:= src $(addprefix src/,$(MODULES))
BINDIR	:= bin
TARGET	:= $(BINDIR)/server.a
SRCS	:= $(shell find $(SOURCEDIR) -name '*.c')
HEADERS	:= $(shell find $(SOURCEDIR) -name '*.h')
OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

vpath $(SRCDIR)

CC		= gcc
CFLAGS	= -Wall -pedantic -extra -Werror -O2 -std=c99
LIBS	= -lpthread
LDFLAGS	= 

#all:
#	gcc -o server.a main.c ftp_server.c error.c trie.c ftp_handlers.c -lpthread

${TARGET}: ${OBJS} 
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS} 

$(BINDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(dir $<) -c $< -o $@

${SRCS}: ${HEADERS}

clean:: 
	-rm -f *~ *.o ${TARGET} 

distclean:: clean
