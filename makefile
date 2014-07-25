CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -pedantic -Os
LDFLAGS =

HDR = util.h pseudo.h terminal.h function.h osc.h dcs.h parse.h gifsave89.h
SRC = main.c util.c pseudo.c terminal.c function.c osc.c dcs.c parse.c gifsave89.c
DST = seq2gif

all:  $(DST)

$(DST): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $@

clean:
	rm -f $(DST) *.o
