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
#include "terminal.h"
#include "function.h"
#include "dcs.h"
#include "parse.h"
#include "gifsave89.h"

#include <stdio.h>

#if HAVE_STDLIB_H
# include <stdlib.h>
#endif
#if HAVE_STRING_H
# include <string.h>
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_GETOPT_H
# include <getopt.h>
#endif
#if HAVE_ERRNO_H
# include <errno.h>
#endif
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif


#if !defined(HAVE_MEMCPY)
# define memcpy(d, s, n) (bcopy ((s), (d), (n)))
#endif

#if !defined(O_BINARY) && defined(_O_BINARY)
# define O_BINARY _O_BINARY
#endif  /* !defined(O_BINARY) && !defined(_O_BINARY) */

struct settings_t {
    int width;
    int height;
    int show_version;
    int show_help;
    int last_frame_delay;
    int foreground_color;
    int background_color;
    int cursor_color;
    int tabwidth;
    int cjkwidth;
    int repeat;
    char *input;
    char *output;
};

enum cmap_bitfield {
    RED_SHIFT   = 5,
    GREEN_SHIFT = 2,
    BLUE_SHIFT  = 0,
    RED_MASK    = 3,
    GREEN_MASK  = 3,
    BLUE_MASK   = 2
};

static void pb_init(struct pseudobuffer *pb, int width, int height)
{
    pb->width  = width;
    pb->height = height;
    pb->bytes_per_pixel = BYTES_PER_PIXEL;
    pb->line_length = pb->width * pb->bytes_per_pixel;
    pb->buf = ecalloc(pb->width * pb->height, pb->bytes_per_pixel);
}

static void pb_die(struct pseudobuffer *pb)
{
    free(pb->buf);
}

static void set_colormap(int colormap[COLORS * BYTES_PER_PIXEL + 1])
{
    int i, ci, r, g, b;
    uint8_t index;

    /* colormap: terminal 256color
    for (i = 0; i < COLORS; i++) {
        ci = i * BYTES_PER_PIXEL;

        r = (color_list[i] >> 16) & bit_mask[8];
        g = (color_list[i] >> 8)  & bit_mask[8];
        b = (color_list[i] >> 0)  & bit_mask[8];

        colormap[ci + 0] = r;
        colormap[ci + 1] = g;
        colormap[ci + 2] = b;
    }
    */

    /* colormap: red/green: 3bit blue: 2bit
    */
    for (i = 0; i < COLORS; i++) {
        index = (uint8_t) i;
        ci = i * BYTES_PER_PIXEL;

        r = (index >> RED_SHIFT)   & bit_mask[RED_MASK];
        g = (index >> GREEN_SHIFT) & bit_mask[GREEN_MASK];
        b = (index >> BLUE_SHIFT)  & bit_mask[BLUE_MASK];

        colormap[ci + 0] = r * bit_mask[BITS_PER_BYTE] / bit_mask[RED_MASK];
        colormap[ci + 1] = g * bit_mask[BITS_PER_BYTE] / bit_mask[GREEN_MASK];
        colormap[ci + 2] = b * bit_mask[BITS_PER_BYTE] / bit_mask[BLUE_MASK];
    }
    colormap[COLORS * BYTES_PER_PIXEL] = -1;
}

static uint32_t pixel2index(uint32_t pixel)
{
    /* pixel is always 24bpp */
    uint32_t r, g, b;

    /* split r, g, b bits */
    r = (pixel >> 16) & bit_mask[8];
    g = (pixel >> 8)  & bit_mask[8];
    b = (pixel >> 0)  & bit_mask[8];

    /* colormap: terminal 256color
    if (r == g && r == b) { // 24 gray scale
        r = 24 * r / COLORS;
        return 232 + r;
    }                       // 6x6x6 color cube

    r = 6 * r / COLORS;
    g = 6 * g / COLORS;
    b = 6 * b / COLORS;

    return 16 + (r * 36) + (g * 6) + b;
    */

    /* colormap: red/green: 3bit blue: 2bit
    */
    // get MSB ..._MASK bits
    r = (r >> (8 - RED_MASK))   & bit_mask[RED_MASK];
    g = (g >> (8 - GREEN_MASK)) & bit_mask[GREEN_MASK];
    b = (b >> (8 - BLUE_MASK))  & bit_mask[BLUE_MASK];

    return (r << RED_SHIFT) | (g << GREEN_SHIFT) | (b << BLUE_SHIFT);
}

static void apply_colormap(struct pseudobuffer *pb, unsigned char *img)
{
    int w, h;
    uint32_t pixel = 0;

    for (h = 0; h < pb->height; h++) {
        for (w = 0; w < pb->width; w++) {
            memcpy(&pixel, pb->buf + h * pb->line_length
                + w * pb->bytes_per_pixel, pb->bytes_per_pixel);
            *(img + h * pb->width + w) = pixel2index(pixel) & bit_mask[BITS_PER_BYTE];
        }
    }
}

static size_t write_gif(unsigned char *gifimage, int size, FILE *f)
{
    size_t wsize = 0;

    wsize = fwrite(gifimage, sizeof(unsigned char), size, f);
    return wsize;
}

static void show_version()
{
    printf(PACKAGE_NAME " " PACKAGE_VERSION "\n"
           "Copyright (C) 2014 haru <uobikiemukot at gmail dot com>\n"
           "Copyright (C) 2012-2014 Hayaki Saito <user@zuse.jp>.\n"
           "\n"
           "This program is free software; you can redistribute it and/or modify\n"
           "it under the terms of the GNU General Public License as published by\n"
           "the Free Software Foundation; either version 3 of the License, or\n"
           "(at your option) any later version.\n"
           "\n"
           "This program is distributed in the hope that it will be useful,\n"
           "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
           "GNU General Public License for more details.\n"
           "\n"
           "You should have received a copy of the GNU General Public License\n"
           "along with this program. If not, see http://www.gnu.org/licenses/.\n"
           "%s\n", copyright
          );
}

static void show_help()
{
    fprintf(stderr,
            "Usage: seq2gif [Options] < ttyrecord > record.gif\n"
            "       seq2gif [Options] -i ttyrecord -o record.gif\n"
            "\n"
            "Options:\n"
            "-w WIDTH, --width=WIDTH               specify terminal width in cell size.\n"
            "                                      (default: 80)\n"
            "-h HEIGHT, --height=HEIGHT            specify terminal height in cell size.\n"
            "                                      (default: 24)\n"
            "-l DELAY, --last-frame-delay=DELAY    specify delay in msec which is added\n"
            "                                      to the last frame. (default: 300)\n"
            "-f COLORNO --foreground-color=COLORNO specify foreground color palette.\n"
            "                                      number.\n"
            "-b COLORNO --background-color=COLORNO specify background color palette\n"
            "                                      number.\n"
            "-c COLORNO --cursor-color=COLORNO     specify cursor color palette\n"
            "                                      number.\n"
            "-t TABSTOP --tabstop=TABSTOP          specify hardware tabstop(default: 8)\n"
            "-j --cjkwidth                         treat East Asian Ambiguous width\n"
            "                                      characters (UAX#11) as wide.\n"
            "-r COUNT --repeat=COUNT               specify animation repeat count. loop\n"
            "                                      infinitely if 0 is given. (default: 0)\n"
            "-i FILE --input=FILE                  specify input file name. use STDIN\n"
            "                                      if '-' is given. (default: '-')\n"
            "-o FILE --output=FILE                 specify output file name. use STDOUT\n"
            "                                      if '-' is given. (default: '-')\n"
            "-V, --version                         show version and license information.\n"
            "-H, --help                            show this help.\n"
           );
}

static int parse_args(int argc, char *argv[], struct settings_t *psettings)
{
    int long_opt;
    int n;
    char const *optstring = "w:h:HVl:f:b:c:t:jr:i:o:";
#if HAVE_GETOPT_LONG
    int option_index;
#endif  /* HAVE_GETOPT_LONG */

#if HAVE_GETOPT_LONG
    struct option long_options[] = {
        {"width",             required_argument,  &long_opt, 'w'},
        {"height",            required_argument,  &long_opt, 'h'},
        {"last-frame-delay",  required_argument,  &long_opt, 'l'},
        {"foreground-color",  required_argument,  &long_opt, 'f'},
        {"background-color",  required_argument,  &long_opt, 'b'},
        {"cursor-color",      required_argument,  &long_opt, 'c'},
        {"tabstop",           required_argument,  &long_opt, 't'},
        {"cjkwidth",          no_argument,        &long_opt, 'j'},
        {"repeat",            required_argument,  &long_opt, 'r'},
        {"input",             required_argument,  &long_opt, 'i'},
        {"output",            required_argument,  &long_opt, 'o'},
        {"help",              no_argument,        &long_opt, 'H'},
        {"version",           no_argument,        &long_opt, 'V'},
        {0, 0, 0, 0}
    };
#endif  /* HAVE_GETOPT_LONG */

    for (;;) {

#if HAVE_GETOPT_LONG
        n = getopt_long(argc, argv, optstring,
                        long_options, &option_index);
#else
        n = getopt(argc, argv, optstring);
#endif  /* HAVE_GETOPT_LONG */
        if (n == -1) {
            break;
        }
        if (n == 0) {
            n = long_opt;
        }
        switch(n) {
        case 'w':
            psettings->width = atoi(optarg);
            if (psettings->width < 1) {
                goto argerr;
            }
            break;
        case 'h':
            psettings->height = atoi(optarg);
            if (psettings->height < 1) {
                goto argerr;
            }
            break;
        case 'l':
            psettings->last_frame_delay = atoi(optarg);
            if (psettings->last_frame_delay < 0) {
                goto argerr;
            }
            break;
        case 'f':
            psettings->foreground_color = atoi(optarg);
            if (psettings->foreground_color < 0) {
                goto argerr;
            }
            if (psettings->foreground_color > 255) {
                goto argerr;
            }
            break;
        case 'b':
            psettings->background_color = atoi(optarg);
            if (psettings->background_color < 0) {
                goto argerr;
            }
            if (psettings->background_color > 255) {
                goto argerr;
            }
            break;
        case 'c':
            psettings->cursor_color = atoi(optarg);
            if (psettings->cursor_color < 0) {
                goto argerr;
            }
            if (psettings->cursor_color > 255) {
                goto argerr;
            }
            break;
        case 't':
            psettings->tabwidth = atoi(optarg);
            if (psettings->tabwidth < 0) {
                goto argerr;
            }
            if (psettings->tabwidth > 255) {
                goto argerr;
            }
            break;
        case 'j':
            psettings->cjkwidth = 1;
            break;
        case 'r':
            psettings->repeat = atoi(optarg);
            if (psettings->repeat < 0) {
                goto argerr;
            }
            if (psettings->repeat > 0xffff) {
                goto argerr;
            }
            break;
        case 'i':
            psettings->input = strdup(optarg);
            break;
        case 'o':
            psettings->output = strdup(optarg);
            break;
        case 'H':
            psettings->show_help = 1;
            break;
        case 'V':
            psettings->show_version = 1;
            break;
        default:
            goto argerr;
        }
    }
    return 0;

argerr:
    show_help();
    return 1;
}

static FILE * open_input_file(char const *filename)
{
    FILE *f;

    if (filename == NULL || strcmp(filename, "-") == 0) {
        /* for windows */
#if defined(O_BINARY)
# if HAVE__SETMODE
        _setmode(fileno(stdin), O_BINARY);
# elif HAVE_SETMODE
        setmode(fileno(stdin), O_BINARY);
# endif  /* HAVE_SETMODE */
#endif  /* defined(O_BINARY) */
        return stdin;
    }
    f = fopen(filename, "rb");
    if (!f) {
#if _ERRNO_H
        fprintf(stderr, "fopen('%s') failed.\n" "reason: %s.\n",
                filename, strerror(errno));
#endif  /* HAVE_ERRNO_H */
        return NULL;
    }
    return f;
}

static FILE * open_output_file(char const *filename)
{
    FILE *f;

    if (filename == NULL || strcmp(filename, "-") == 0) {
        /* for windows */
#if defined(O_BINARY)
# if HAVE__SETMODE
        _setmode(fileno(stdout), O_BINARY);
# elif HAVE_SETMODE
        setmode(fileno(stdout), O_BINARY);
# endif  /* HAVE_SETMODE */
#endif  /* defined(O_BINARY) */
        return stdout;
    }
    f = fopen(filename, "wb");
    if (!f) {
#if _ERRNO_H
        fprintf(stderr, "fopen('%s') failed.\n" "reason: %s.\n",
                filename, strerror(errno));
#endif  /* HAVE_ERRNO_H */
        return NULL;
    }
    return f;
}

static int32_t readtime(FILE *f, uint8_t *obuf)
{
    int nread;
    int32_t tv_sec;
    int32_t tv_usec;

    nread = fread(obuf, 1, sizeof(tv_sec), f);
    if (nread != sizeof(tv_sec)) {
        return -1;
    }
    tv_sec = obuf[0] | obuf[1] << 8
           | obuf[2] << 16 | obuf[3] << 24;
    nread = fread(obuf, 1, sizeof(tv_usec), f);
    if (nread != sizeof(tv_usec)) {
        return -1;
    }
    tv_usec = obuf[0] | obuf[1] << 8
            | obuf[2] << 16 | obuf[3] << 24;

    return tv_sec * 1000000 + tv_usec;
}

static int32_t readlen(FILE *f, uint8_t *obuf)
{
    int nread;
    uint32_t len;

    nread = fread(obuf, 1, sizeof(len), f);
    if (nread != sizeof(len)) {
        return -1;
    }
    len = obuf[0]
        | obuf[1] << 8
        | obuf[2] << 16
        | obuf[3] << 24;

    return len;
}

int main(int argc, char *argv[])
{
    uint8_t *obuf;
    ssize_t nread;
    struct terminal term;
    struct pseudobuffer pb;
    uint32_t prev = 0;
    uint32_t now = 0;
    ssize_t len = 0;
    size_t maxlen = 0;
    int delay = 0;
    int dirty = 0;
    FILE *in_file = NULL;
    FILE *out_file = NULL;
    int nret = EXIT_SUCCESS;

    void *gsdata;
    unsigned char *gifimage = NULL;
    int gifsize, colormap[COLORS * BYTES_PER_PIXEL + 1];
    unsigned char *img;

    struct settings_t settings = {
        80,     /* width */
        24,     /* height */
        0,      /* show_version */
        0,      /* show_help */
        300,    /* last_frame_delay */
        7,      /* foreground_color */
        0,      /* background_color */
        2,      /* cursor_color */
        8,      /* tabwidth */
        0,      /* cjkwidth */
        0,      /* repeat */
        NULL,   /* input */
        NULL,   /* output */
    };

    if (parse_args(argc, argv, &settings) != 0) {
        exit(1);
    }

    if (settings.show_help) {
        show_help();
        exit(0);
    }

    if (settings.show_version) {
        show_version();
        exit(0);
    }

    /* init */
    pb_init(&pb, settings.width * CELL_WIDTH, settings.height * CELL_HEIGHT);
    term_init(&term, pb.width, pb.height,
              settings.foreground_color,
              settings.background_color,
              settings.cursor_color,
              settings.tabwidth,
              settings.cjkwidth);

    /* init gif */
    img = (unsigned char *) ecalloc(pb.width * pb.height, 1);
    set_colormap(colormap);
    if (!(gsdata = newgif((void **) &gifimage, pb.width, pb.height, colormap, 0))) {
        free(img);
        term_die(&term);
        pb_die(&pb);
        return EXIT_FAILURE;
    }

    animategif(gsdata, /* repetitions */ settings.repeat, 0,
        /* transparent background */  -1, /* disposal */ 2);

    in_file = open_input_file(settings.input);
    out_file = open_output_file(settings.output);

    maxlen = 2048;
    obuf = malloc(maxlen);
    prev = now = readtime(in_file, obuf);

    /* main loop */
    for(;;) {
        len = readlen(in_file, obuf);
        if (len <= 0) {
            nret = EXIT_FAILURE;
            break;
        }
        if (len > maxlen) {
            obuf = realloc(obuf, len);
            maxlen = len;
        }
        nread = fread(obuf, 1, len, in_file);
        if (nread != len) {
            nret = EXIT_FAILURE;
            break;
        }
        parse(&term, obuf, nread, &dirty);
        now = readtime(in_file, obuf);
        if (now == -1) {
            break;
        }
        if (term.esc.state != STATE_DCS || dirty) {
            refresh(&pb, &term);
            delay = now - prev;

            /* take screenshot */
            apply_colormap(&pb, img);
            controlgif(gsdata, -1, delay / 10000, 0, 0);
            prev = now;
            putgif(gsdata, img);
        }
        dirty = 0;
    }
    if (settings.last_frame_delay > 0) {
        controlgif(gsdata, -1, settings.last_frame_delay / 10, 0, 0);
        putgif(gsdata, img);
    }

    /* output gif */
    gifsize = endgif(gsdata);
    if (gifsize > 0) {
        write_gif(gifimage, gifsize, out_file);
        free(gifimage);
    }

    /* normal exit */
    free(img);
    if (settings.input) {
        if (fileno(in_file) != STDIN_FILENO) {
            fclose(in_file);
        }
        free(settings.input);
    }
    if (settings.output) {
        if (fileno(out_file) != STDOUT_FILENO) {
            fclose(out_file);
        }
        free(settings.output);
    }
    term_die(&term);
    pb_die(&pb);
    free(obuf);

    return nret;
}

/* emacs, -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */
/* vim: set expandtab ts=4 : */
/* EOF */
