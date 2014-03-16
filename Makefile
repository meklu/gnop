CC := gcc
CFLAGS := -Wall -Wextra -pedantic -Wstrict-prototypes -Os -g
LDFLAGS := -lSDL2

TARGETS = gnop

.PHONY : all fall clean test

all : $(TARGETS)

fall : clean $(TARGETS)

clean :
	rm -f $(TARGETS)

msgbox : gnop.c
	$(CC) $(CFLAGS) $(LDFLAGS) gnop.c -o gnop
