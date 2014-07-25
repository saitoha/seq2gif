/* See LICENSE for licence details. */

/* color: index number of color_palette[] (see color.h) */
enum {
	DEFAULT_FG           = 7,
	DEFAULT_BG           = 0,
	ACTIVE_CURSOR_COLOR  = 2,
	PASSIVE_CURSOR_COLOR = 1,
};

/* misc */
enum {
	DEBUG            = 0,      /* write dump of input to stdout, debug message to stderr */
	TABSTOP          = 8,      /* hardware tabstop */
	BACKGROUND_DRAW  = 0,      /* always draw even if vt is not active */
	SUBSTITUTE_HALF  = 0x0020, /* used for missing glyph (single width): U+0020 (SPACE) */
	SUBSTITUTE_WIDE  = 0x3000, /* used for missing glyph (double width): U+3000 (IDEOGRAPHIC SPACE) */
	REPLACEMENT_CHAR = 0x003F, /* used for malformed UTF-8 sequence    : U+003F (QUESTION MARK)  */
};

