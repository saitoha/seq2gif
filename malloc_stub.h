/*
 * Copyright (c) 2014 Hayaki Saito
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

#ifndef MALLOC_STUB_H
#define MALLOC_STUB_H

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif  /* HAVE_SYS_TYPES_H */

#if !HAVE_MALLOC
void * rpl_malloc(size_t n);
#endif /* !HAVE_MALLOC */

#if !HAVE_REALLOC
void * rpl_realloc(void *p, size_t n);
#endif /* !HAVE_REALLOC */

#if 0
int rpl_posix_memalign(void **memptr, size_t alignment, size_t size);
#endif

#endif /* MALLOC_STUB_H */

/* Hello emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
