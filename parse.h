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

/* ctr char/esc sequence/charset function */
void control_character(struct terminal *term, uint8_t ch);
void esc_sequence(struct terminal *term, uint8_t ch);
void csi_sequence(struct terminal *term, uint8_t ch);
int is_osc_parm(int c);
void omit_string_terminator(char *bp, uint8_t ch);
void osc_sequence(struct terminal *term, uint8_t ch);
void dcs_sequence(struct terminal *term, uint8_t ch);
void utf8_charset(struct terminal *term, uint8_t ch);
void parse(struct terminal *term, uint8_t *buf, int size);

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
