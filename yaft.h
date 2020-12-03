/*
 * Copyright (C) 2014 haru <uobikiemukot at gmail dot com>
 * Copyright (C) 2014 Hayaki Saito <user@zuse.jp>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef HAVE_WCHAR_H
# include <wchar.h>
#endif

#if SELECTED_FONT == FONT_MILKJF
# include "glyph/milkjf.h"
#elif SELECTED_FONT == FONT_MPLUS
# include "glyph/mplus.h"
#else
# include "glyph/unifont.h"
#endif
#include "glyph/conf.h"
#include "malloc_stub.h"
#include "color.h"

#define SIGWINCH 28

/* misc */
enum {
    DEBUG            = 0,      /* write dump of input to stdout, debug message to stderr */
};

enum char_code {
    /* 7 bit */
    BEL = 0x07, BS  = 0x08, HT  = 0x09,
    LF  = 0x0A, VT  = 0x0B, FF  = 0x0C,
    CR  = 0x0D, ESC = 0x1B, DEL = 0x7F,
    /* others */
    SPACE     = 0x20,
    BACKSLASH = 0x5C,
};

enum misc {
    BUFSIZE           = 1024,    /* read, esc, various buffer size */
    BITS_PER_BYTE     = 8,
    BYTES_PER_PIXEL   = 3,       /* pixel size of sixel bitmap data */
    BITS_PER_SIXEL    = 6,       /* number of bits of a sixel */
    ESCSEQ_SIZE       = 256,     /* limit size of terminal escape sequence */
    MAX_ARGS          = 16,      /* max parameters of csi/osc sequence */
    COLORS            = 256,     /* number of color */
    UCS2_CHARS        = 0x10000, /* number of UCS2 glyph */
    CTRL_CHARS        = 0x20,    /* number of ctrl_func */
    ESC_CHARS         = 0x80,    /* number of esc_func */
    DRCS_CHARSETS     = 63,      /* number of charset of DRCS (according to DRCSMMv1) */
    GLYPH_PER_CHARSET = 96,      /* number of glyph of each DRCS charset */
    DEFAULT_CHAR      = SPACE,   /* used for erase char */
    BRIGHT_INC        = 8,       /* value used for brightening color */
    OSC_GWREPT        = 8900,    /* OSC Ps: mode number of yaft GWREPT */
};

enum char_attr {
    ATTR_RESET     = 0,
    ATTR_BOLD      = 1, /* brighten foreground */
    ATTR_UNDERLINE = 4,
    ATTR_BLINK     = 5, /* brighten background */
    ATTR_REVERSE   = 7,
};

static const uint8_t attr_mask[] = {
    0x00, 0x01, 0x00, 0x00, /* 0:none      1:bold  2:none 3:none */
    0x02, 0x04, 0x00, 0x08, /* 4:underline 5:blink 6:none 7:reverse */
};

static const uint32_t bit_mask[] = {
    0x00,
    0x01,       0x03,       0x07,       0x0F,
    0x1F,       0x3F,       0x7F,       0xFF,
    0x1FF,      0x3FF,      0x7FF,      0xFFF,
    0x1FFF,     0x3FFF,     0x7FFF,     0xFFFF,
    0x1FFFF,    0x3FFFF,    0x7FFFF,    0xFFFFF,
    0x1FFFFF,   0x3FFFFF,   0x7FFFFF,   0xFFFFFF,
    0x1FFFFFF,  0x3FFFFFF,  0x7FFFFFF,  0xFFFFFFF,
    0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

enum term_mode {
    MODE_RESET   = 0x00,
    MODE_ORIGIN  = 0x01, /* origin mode: DECOM */
    MODE_CURSOR  = 0x02, /* cursor visible: DECTCEM */
    MODE_AMRIGHT = 0x04, /* auto wrap: DECAWM */
};

enum esc_state {
    STATE_RESET  = 0x00,
    STATE_ESC    = 0x01, /* 0x1B, \033, ESC */
    STATE_CSI    = 0x02, /* ESC [ */
    STATE_OSC    = 0x04, /* ESC ] */
    STATE_DCS    = 0x08, /* ESC P */
};

enum glyph_width_t {
    NEXT_TO_WIDE = 0,
    HALF,
    WIDE,
};

struct margin { uint16_t top, bottom; };
struct point_t { uint16_t x, y; };
struct color_pair_t { uint8_t fg, bg; };

struct cell_t {
    const struct glyph_t *glyphp;   /* pointer to glyph */
    struct color_pair_t color_pair; /* color (fg, bg) */
    enum char_attr attribute;       /* bold, underscore, etc... */
    enum glyph_width_t width;       /* wide char flag: WIDE, NEXT_TO_WIDE, HALF */
    bool has_bitmap;
    /* must be statically allocated for copy_cell() */
    uint8_t bitmap[BYTES_PER_PIXEL * CELL_WIDTH * CELL_HEIGHT];
};

struct esc_t {
    char *buf;
    char *bp;
    int size;
    enum esc_state state;
};

struct charset_t {
    uint32_t code; /* UCS4 code point: yaft only prints UCS2 and DRCSMMv1 */
    int following_byte, count;
    bool is_valid;
};

struct state_t {   /* for save, restore state */
    struct point_t cursor;
    enum term_mode mode;
    enum char_attr attribute;
};

struct sixel_canvas_t {
    uint8_t *bitmap;
    struct point_t point;
    int width, height;
    int line_length;
    uint8_t color_index;
    uint32_t color_table[COLORS];
};

typedef int (*wcwidth_func_t)(wchar_t);

struct terminal {
    int fd;                                      /* master fd */
    int width, height;                           /* terminal size (pixel) */
    int cols, lines;                             /* terminal size (cell) */
    struct cell_t *cells;                        /* pointer to each cell: cells[cols + lines * num_of_cols] */
    struct margin scroll;                        /* scroll margin */
    struct point_t cursor;                       /* cursor pos (x, y) */
    bool *line_dirty;                            /* dirty flag */
    bool *tabstop;                               /* tabstop flag */
    enum term_mode mode;                         /* for set/reset mode */
    bool wrap_occured;                           /* whether auto wrap occured or not */
    struct state_t state;                        /* for restore */
    struct color_pair_t color_pair;              /* color (fg, bg) */
    enum char_attr attribute;                    /* bold, underscore, etc... */
    struct charset_t charset;                    /* store UTF-8 byte stream */
    struct esc_t esc;                            /* store escape sequence */
    uint32_t color_palette[COLORS];              /* 256 color palette */
    const struct glyph_t *glyph_map[UCS2_CHARS]; /* array of pointer to glyphs[] */
    struct glyph_t *drcs[DRCS_CHARSETS];         /* DRCS chars */
    struct sixel_canvas_t sixel;
    int default_fg;                              /* default foreground color */
    int default_bg;                              /* default background color */
    int cursor_color;                            /* corsor color */
    int tabwidth;                                /* hardware tabstop */
    wcwidth_func_t fn_wcwidth;                   /* wcwidth strategy */
    bool use_bright_instead_of_bold;             /* interpretation of bold attributes */
};

struct parm_t { /* for parse_arg() */
    int argc;
    char *argv[MAX_ARGS];
};

/* emacs Local Variables:      */
/* emacs mode: c               */
/* emacs tab-width: 4          */
/* emacs indent-tabs-mode: nil */
/* emacs c-basic-offset: 4     */
/* emacs End:                  */
/* vim: set expandtab ts=4 : */
/* EOF */
