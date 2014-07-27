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

/* misc */
int sum(struct parm_t *parm);

/* function for control character */
void bs(struct terminal *term);
void tab(struct terminal *term);
void nl(struct terminal *term);
void cr(struct terminal *term);
void enter_esc(struct terminal *term);

/* function for escape sequence */
void save_state(struct terminal *term);
void restore_state(struct terminal *term);
void crnl(struct terminal *term);
void set_tabstop(struct terminal *term);
void reverse_nl(struct terminal *term);
void identify(struct terminal *term);
void enter_csi(struct terminal *term);
void enter_osc(struct terminal *term);
void enter_dcs(struct terminal *term);
void ris(struct terminal *term);

/* function for csi sequence */
void insert_blank(struct terminal *term, struct parm_t *parm);
void curs_up(struct terminal *term, struct parm_t *parm);
void curs_down(struct terminal *term, struct parm_t *parm);
void curs_forward(struct terminal *term, struct parm_t *parm);
void curs_back(struct terminal *term, struct parm_t *parm);
void curs_nl(struct terminal *term, struct parm_t *parm);
void curs_pl(struct terminal *term, struct parm_t *parm);
void curs_col(struct terminal *term, struct parm_t *parm);
void curs_pos(struct terminal *term, struct parm_t *parm);
void erase_display(struct terminal *term, struct parm_t *parm);
void erase_line(struct terminal *term, struct parm_t *parm);
void insert_line(struct terminal *term, struct parm_t *parm);
void delete_line(struct terminal *term, struct parm_t *parm);
void delete_char(struct terminal *term, struct parm_t *parm);
void erase_char(struct terminal *term, struct parm_t *parm);
void curs_line(struct terminal *term, struct parm_t *parm);
void set_attr(struct terminal *term, struct parm_t *parm);
void status_report(struct terminal *term, struct parm_t *parm);
void set_mode(struct terminal *term, struct parm_t *parm);
void reset_mode(struct terminal *term, struct parm_t *parm);
void set_margin(struct terminal *term, struct parm_t *parm);
void clear_tabstop(struct terminal *term, struct parm_t *parm);

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
