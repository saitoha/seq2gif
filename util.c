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

#include <stdio.h>
#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif
#ifdef HAVE_LIMITS_H
# include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif

#include "yaft.h"

/* error functions */
void error(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void fatal(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(EXIT_FAILURE);
}

/* wrapper of C functions */
void *ecalloc(size_t nmemb, size_t size)
{
    void *ptr;
#if defined(HAVE_SYS_ERRNO_H) || (HAVE_ERRNO_H)
    errno = 0;
#endif

#ifdef HAVE_CALLOC
    if ((ptr = calloc(nmemb, size)) == NULL)
        error("calloc");
#else
    if ((ptr = malloc(nmemb * size)) == NULL)
        error("malloc");
    memset(ptr, 0, nmemb * size);
#endif

    return ptr;
}

void *erealloc(void *ptr, size_t size)
{
    void *new;
#if defined(HAVE_SYS_ERRNO_H) || (HAVE_ERRNO_H)
    errno = 0;
#endif

    if ((new = realloc(ptr, size)) == NULL)
        error("realloc");

    return new;
}

static long estrtol(const char *nptr, char **endptr, int base)
{
    long int ret;
#if defined(HAVE_SYS_ERRNO_H) || (HAVE_ERRNO_H)
    errno = 0;
#endif

    ret = strtol(nptr, endptr, base);
#ifdef HAVE_LIMITS_H
    if (ret == LONG_MIN || ret == LONG_MAX) {
        perror("strtol");
        return 0;
    }
#endif

    return ret;
}

/* parse_arg functions */
void reset_parm(struct parm_t *pt)
{
    int i;

    pt->argc = 0;
    for (i = 0; i < MAX_ARGS; i++)
        pt->argv[i] = NULL;
}

void add_parm(struct parm_t *pt, char *cp)
{
    if (pt->argc >= MAX_ARGS)
        return;

    if (DEBUG)
        fprintf(stderr, "argv[%d]: %s\n",
            pt->argc, (cp == NULL) ? "NULL": cp);

    pt->argv[pt->argc] = cp;
    pt->argc++;
}

void parse_arg(char *buf, struct parm_t *pt, int delim, int (is_valid)(int c))
{
    /*
        v..........v d           v.....v d v.....v ... d
        (valid char) (delimiter)
        argv[0]                  argv[1]   argv[2] ...   argv[argc - 1]
    */
    size_t i, length;
    char *cp, *vp;

    if (buf == NULL)
        return;

    length = strlen(buf);
    if (DEBUG)
        fprintf(stderr, "parse_arg()\nlength:%u\n", (unsigned) length);

    vp = NULL;
    for (i = 0; i < length; i++) {
        cp = buf + i;

        if (vp == NULL && is_valid(*cp))
            vp = cp;

        if (*cp == delim) {
            *cp = '\0';
            add_parm(pt, vp);
            vp = NULL;
        }

        if (i == (length - 1) && (vp != NULL || *cp == '\0'))
            add_parm(pt, vp);
    }

    if (DEBUG)
        fprintf(stderr, "argc:%d\n", pt->argc);
}

/* other functions */
int my_ceil(int val, int div)
{
    return (val + div - 1) / div;
}

int dec2num(char *str)
{
    if (str == NULL)
        return 0;

    return estrtol(str, NULL, 10);
}

int hex2num(char *str)
{
    if (str == NULL)
        return 0;

    return estrtol(str, NULL, 16);
}

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
