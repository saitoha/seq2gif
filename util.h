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

/* error functions */
void error(char *str);
void fatal(char *str);

/* wrapper of C functions */
void *ecalloc(size_t nmemb, size_t size);
void *erealloc(void *ptr, size_t size);
void ewrite(int fd, const void *buf, int size);

/* parse_arg functions */
void reset_parm(struct parm_t *pt);
void add_parm(struct parm_t *pt, char *cp);
void parse_arg(char *buf, struct parm_t *pt, int delim, int (is_valid)(int c));

/* other functions */
int my_ceil(int val, int div);
int dec2num(char *str);
int hex2num(char *str);

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
