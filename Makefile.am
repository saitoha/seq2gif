
if COND_GCOV
MAYBE_COVERAGE = --coverage
endif

CLEANFILES = *.gcno *.gcda *.gcov

bin_PROGRAMS = seq2gif
seq2gif_SOURCES = main.c util.c pseudo.c terminal.c function.c \
				  osc.c dcs.c parse.c gifsave89.c \
				  malloc_stub.c wcwidth.c \
				  util.h pseudo.h terminal.h function.h \
				  osc.h dcs.h parse.h gifsave89.h \
				  wcwidth.h malloc_stub.h
seq2gif_CFLAGS = $(MAYBE_COVERAGE)

run:
	cat ttyrecord | ./seq2gif | img2sixel

valgrind: all
	valgrind --leak-check=full --show-reachable=no --error-limit=no ./seq2gif -i tests/data/sl.tty -o /dev/null 2>&1 \
        | grep ^== \
        | tee valgrind.log
	grep "All heap blocks were freed" valgrind.log || grep "definitely lost: 0 bytes in 0 blocks" valgrind.log
	grep "All heap blocks were freed" valgrind.log || grep "indirectly lost: 0 bytes in 0 blocks" valgrind.log
	test ! $$(grep Invalid valgrind.log)
