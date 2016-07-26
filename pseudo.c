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
#include "yaft.h"
#include "util.h"
#include "pseudo.h"

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#if !defined(HAVE_MEMCPY)
# define memcpy(d, s, n) (bcopy ((s), (d), (n)))
#endif

static void draw_sixel(struct pseudobuffer *pb, int line, int col, uint8_t *bitmap)
{
    int h, w, src_offset, dst_offset;
    uint32_t pixel, color = 0;

    for (h = 0; h < CELL_HEIGHT; h++) {
        for (w = 0; w < CELL_WIDTH; w++) {
            src_offset = BYTES_PER_PIXEL * (h * CELL_WIDTH + w);
            memcpy(&color, bitmap + src_offset, BYTES_PER_PIXEL);

            dst_offset = (line * CELL_HEIGHT + h) * pb->line_length + (col * CELL_WIDTH + w) * pb->bytes_per_pixel;
            //pixel = color2pixel(&pb->vinfo, color);
            pixel = color;
            memcpy(pb->buf + dst_offset, &pixel, pb->bytes_per_pixel);
        }
    }
}

static void draw_line(struct pseudobuffer *pb, struct terminal *term, int line)
{
    int pos, bdf_padding, glyph_width, margin_right;
    int col, w, h;
    uint32_t pixel;
    struct color_pair_t color_pair;
    struct cell_t *cellp;

    for (col = term->cols - 1; col >= 0; col--) {
        margin_right = (term->cols - 1 - col) * CELL_WIDTH;

        /* target cell */
        cellp = &term->cells[col + line * term->cols];

        /* draw sixel bitmap */
        if (cellp->has_bitmap) {
            draw_sixel(pb, line, col, cellp->bitmap);
            continue;
        }

        /* copy current color_pair (maybe changed) */
        color_pair = cellp->color_pair;

        /* check wide character or not */
        glyph_width = (cellp->width == HALF) ? CELL_WIDTH: CELL_WIDTH * 2;
        bdf_padding = my_ceil(glyph_width, BITS_PER_BYTE) * BITS_PER_BYTE - glyph_width;
        if (cellp->width == WIDE)
            bdf_padding += CELL_WIDTH;

        /* check cursor positon */
        if ((term->mode & MODE_CURSOR && line == term->cursor.y)
            && (col == term->cursor.x
            || (cellp->width == WIDE && (col + 1) == term->cursor.x)
            || (cellp->width == NEXT_TO_WIDE && (col - 1) == term->cursor.x))) {
            color_pair.fg = term->default_bg;
            color_pair.bg = term->cursor_color;
        }

        for (h = 0; h < CELL_HEIGHT; h++) {
            /* if UNDERLINE attribute on, swap bg/fg */
            if ((h == (CELL_HEIGHT - 1)) && (cellp->attribute & attr_mask[ATTR_UNDERLINE]))
                color_pair.bg = color_pair.fg;

            for (w = 0; w < CELL_WIDTH; w++) {
                int isfg = 0;
                pos = (term->width - 1 - margin_right - w) * pb->bytes_per_pixel
                    + (line * CELL_HEIGHT + h) * pb->line_length;

                if (cellp->attribute & attr_mask[ATTR_BOLD] && !term->use_bright_instead_of_bold)
                    isfg = cellp->glyphp->bitmap[h] & (0x03 << (bdf_padding + w));
                else
                    isfg = cellp->glyphp->bitmap[h] & (0x01 << (bdf_padding + w));

                /* set color palette */
                pixel = color_list[isfg ? color_pair.fg : color_pair.bg];

                /* update copy buffer only */
                memcpy(pb->buf + pos, &pixel, pb->bytes_per_pixel);
            }
        }
    }
    term->line_dirty[line] = ((term->mode & MODE_CURSOR) && term->cursor.y == line) ? true: false;
}

void refresh(struct pseudobuffer *pb, struct terminal *term)
{
    int line;

    if (term->mode & MODE_CURSOR)
        term->line_dirty[term->cursor.y] = true;

    for (line = 0; line < term->lines; line++) {
        if (term->line_dirty[line])
            draw_line(pb, term, line);
    }
}

/* emacs Local Variables:      */
/* emacs mode: c               */
/* emacs tab-width: 4          */
/* emacs indent-tabs-mode: nil */
/* emacs c-basic-offset: 4     */
/* emacs End:                  */
/* vim: set expandtab ts=4 : */
/* EOF */
