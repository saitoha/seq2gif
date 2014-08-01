
all: unifont.h milkjf.h mplus.h

mkfont_bdf: mkfont_bdf.c font.h bdf.h
	$(CC) -o $@ $<

milkjf.h: mkfont_bdf
	# If you want to use your favorite fonts, please change following line
	# usage: mkfont_bdf ALIAS BDF1 BDF2 BDF3... > glyph.h
	# ALIAS: glyph substitution rule file (see table/alias for more detail)
	# BDF1 BDF2 BDF3...: monospace bdf files (must be the same size)
	./mkfont_bdf table/alias fonts/milkjf_k16.bdf fonts/milkjf_8x16r.bdf fonts/milkjf_8x16.bdf | cat milkjf.txt - > $@

mplus.h: mkfont_bdf
	./mkfont_bdf table/alias fonts/mplus_j12r.bdf fonts/mplus_f12r.bdf | cat mplus.txt - > $@

unifont.h: mkfont_bdf
	./mkfont_bdf table/alias fonts/unifont.bdf | cat unifont.txt - > $@

clean:
	rm -f mkfont_bdf
