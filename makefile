################################################################################
#                           Makefile for cue                                   #
#                                                                              #
#   Targets:                                                                   #
#       all                - compile everything                                #
#       cue                - compile cue                                       #
#       debug              - compile everything with debug printing            #
#       fulldebug          - compile everything with more debug printing       #
#       clean              - remove object files                               #
#                                                                              #
################################################################################

SHELL = /bin/sh

#directories
OBJDIR = obj
BINDIR = bin

#compiler
CC = gcc
#compiler options
CFLAGS = -g -Wall

#sources
VPATH = %.c src/
VPATH = %.h src/
SOURCES = cue.c pocket.c network.c storage.c

#object specification
#OBJECTS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))
CUEOBJ = 		$(addprefix $(OBJDIR)/, \
			 	cue.o pocket.o network.o storage.o)

all: cue

clean:
	rm -f $(CUEOBJ)
	rm -f $(BINDIR)/cue

cue: $(CUEOBJ) -lcurl
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

debug: CFLAGS += -DDEBUG=1
debug: all

fulldebug: CFLAGS += -DDEBUG=2
fulldebug: all
