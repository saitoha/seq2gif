
bin_PROGRAMS = seq2gif
seq2gif_SOURCES = main.c util.c pseudo.c terminal.c function.c \
				  osc.c dcs.c parse.c gifsave89.c \
				  malloc_stub.c wcwidth.c \
				  util.h pseudo.h terminal.h function.h \
				  osc.h dcs.h parse.h gifsave89.h \
				  wcwidth.h malloc_stub.h

run:
	cat ttyrecord | ./seq2gif | img2sixel
