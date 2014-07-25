/* See LICENSE for licence details. */
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
