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
int eopen(const char *path, int flag);
void eclose(int fd);
FILE *efopen(const char *path, char *mode);
void efclose(FILE *fp);
void *emmap(void *addr, size_t len, int prot, int flag, int fd, off_t offset);
void emunmap(void *ptr, size_t len);
void *ecalloc(size_t nmemb, size_t size);
void *erealloc(void *ptr, size_t size);
void eselect(int max_fd, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *tv);
void ewrite(int fd, const void *buf, int size);
void esigaction(int signo, struct sigaction *act, struct sigaction *oact);
void etcgetattr(int fd, struct termios *tm);
void etcsetattr(int fd, int action, const struct termios *tm);
int eopenpty(int *amaster, int *aslave, char *aname,
             const struct termios *termp, const struct winsize *winsize);
pid_t eforkpty(int *amaster, char *name,
               const struct termios *termp, const struct winsize *winsize);
int esetenv(const char *name, const char *value, int overwrite);
int eexecvp(const char *file, const char *argv[]);
long estrtol(const char *nptr, char **endptr, int base);

/* parse_arg functions */
void reset_parm(struct parm_t *pt);
void add_parm(struct parm_t *pt, char *cp);
void parse_arg(char *buf, struct parm_t *pt, int delim, int (is_valid)(int c));

/* other functions */
uint32_t bit_reverse(uint32_t val, int bits);

int my_ceil(int val, int div);
int dec2num(char *str);
int hex2num(char *str);

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
