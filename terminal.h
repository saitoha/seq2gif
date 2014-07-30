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

void erase_cell(struct terminal *term, int y, int x);
void copy_cell(struct terminal *term, int dst_y, int dst_x, int src_y, int src_x);
int set_cell(struct terminal *term, int y, int x, const struct glyph_t *glyphp);
void scroll(struct terminal *term, int from, int to, int offset);

/* relative movement: cause scrolling */
void move_cursor(struct terminal *term, int y_offset, int x_offset);

/* absolute movement: never scroll */
void set_cursor(struct terminal *term, int y, int x);
const struct glyph_t *drcsch(struct terminal *term, uint32_t code);
void addch(struct terminal *term, uint32_t code);
void reset_esc(struct terminal *term);
bool push_esc(struct terminal *term, uint8_t ch);
void reset_charset(struct terminal *term);
void reset(struct terminal *term);
void term_init(struct terminal *term, int width, int height,
               int foreground_color, int background_color,
               int cursor_color, int tabwidth, int cjkwidth);
void term_die(struct terminal *term);

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
