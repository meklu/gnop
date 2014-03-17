CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Wstrict-prototypes -Os -g
LDFLAGS := $(shell pkg-config --libs sdl2)
INCLUDE := $(shell pkg-config --cflags-only-I sdl2)

TARGETS = gnop

.PHONY : all fall clean test

all : $(TARGETS)

fall : clean $(TARGETS)

clean :
	$(RM) $(TARGETS)

gnop : gnop.c
	$(CC) $(INCLUDE) $(CFLAGS) gnop.c -o gnop $(LDFLAGS)
