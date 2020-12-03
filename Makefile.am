
if COND_GCOV
MAYBE_COVERAGE = --coverage
endif

CLEANFILES = *.gcno *.gcda *.gcov

bin_PROGRAMS = seq2gif
seq2gif_SOURCES = main.c util.c pseudo.c terminal.c function.c \
				  dcs.c parse.c gifsave89.c \
				  malloc_stub.c wcwidth.c \
				  util.h pseudo.h terminal.h function.h \
				  dcs.h parse.h gifsave89.h \
				  wcwidth.h malloc_stub.h
seq2gif_CFLAGS = $(MAYBE_COVERAGE)

test: all
	for ttyfile in tests/data/*.tty; do ./seq2gif -i "$$ttyfile" -o /dev/null; done
	! ./seq2gif -i /non-existent-file.tty -o /dev/null
	! ./seq2gif -i tests/data/sl.tty -o /non-existent-file.out
	for dither in none fs; do for map in rgb332 xterm256; do \
	  ./seq2gif -d $$dither -m $$map -i tests/data/ls.tty -o /dev/null; \
	done; done

coveralls: test
	coveralls -E '.*\.h' -e tests -e glyph -e m4

valgrind: all
	valgrind --leak-check=full --show-reachable=no --error-limit=no ./seq2gif -i tests/data/sl.tty -o /dev/null 2>&1 \
        | grep ^== \
        | tee valgrind.log
	grep "All heap blocks were freed" valgrind.log || grep "definitely lost: 0 bytes in 0 blocks" valgrind.log
	grep "All heap blocks were freed" valgrind.log || grep "indirectly lost: 0 bytes in 0 blocks" valgrind.log
	test ! $$(grep Invalid valgrind.log)
