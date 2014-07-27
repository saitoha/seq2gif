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

#include "yaft.h"
#include "conf.h"
#include "util.h"
#include "pseudo.h"
#include "terminal.h"
#include "function.h"
#include "osc.h"
#include "dcs.h"
#include "parse.h"
#include "gifsave89.h"

enum {
    TERM_WIDTH      = 640,
    TERM_HEIGHT     = 384,
    INPUT_WAIT      = 0,
    INPUT_BUF       = 1,
    OUTPUT_BUF      = 1024,
    NO_OUTPUT_LIMIT = 16,
};

enum cmap_bitfield {
    RED_SHIFT   = 5,
    GREEN_SHIFT = 2,
    BLUE_SHIFT  = 0,
    RED_MASK    = 3,
    GREEN_MASK  = 3,
    BLUE_MASK   = 2
};

struct tty_state tty = {
    .visible        = true,
    .redraw_flag    = false,
    .loop_flag      = true,
    .window_resized = false,
};

void pb_init(struct pseudobuffer *pb)
{
    pb->width  = TERM_WIDTH;
    pb->height = TERM_HEIGHT;
    pb->bytes_per_pixel = BYTES_PER_PIXEL;
    pb->line_length = pb->width * pb->bytes_per_pixel;
    pb->buf = ecalloc(pb->width * pb->height, pb->bytes_per_pixel);
}

void pb_die(struct pseudobuffer *pb)
{
    free(pb->buf);
}

void set_colormap(int colormap[COLORS * BYTES_PER_PIXEL + 1])
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

uint32_t pixel2index(uint32_t pixel)
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

void apply_colormap(struct pseudobuffer *pb, unsigned char *img)
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

size_t write_gif(unsigned char *gifimage, int size)
{
    size_t wsize = 0;

    wsize = fwrite(gifimage, sizeof(unsigned char), size, stdout);
    return wsize;
}

int main(int argc, char *argv[])
{
    uint8_t *obuf;
    ssize_t nread;
    struct terminal term;
    struct pseudobuffer pb;
    int32_t sec = 0;
    int32_t usec = 0;
    int32_t tv_sec = 0;
    int32_t tv_usec = 0;
    int32_t len = 0;

    void *gsdata;
    unsigned char *gifimage = NULL;
    int gifsize, colormap[COLORS * BYTES_PER_PIXEL + 1];
    unsigned char *img;

    (void) argc;
    (void) argv;

    /* init */
    pb_init(&pb);
    term_init(&term, pb.width, pb.height);

    /* init gif */
    img = (unsigned char *) ecalloc(pb.width * pb.height, 1);
    set_colormap(colormap);
    if (!(gsdata = newgif((void **) &gifimage, pb.width, pb.height, colormap, 0)))
        return EXIT_FAILURE;

    animategif(gsdata, /* repetitions */ 0, 10,
        /* transparent background */  -1, /* disposal */ 2);

    obuf = malloc(4);
    /* main loop */
    int delay;
    for(;;) {
        nread = read(STDIN_FILENO, obuf, sizeof(tv_sec));
        if (nread != sizeof(tv_sec)) {
            break;
        }
        tv_sec = obuf[0] | obuf[1] << 8
               | obuf[2] << 16 | obuf[3] << 24;
        nread = read(STDIN_FILENO, obuf, sizeof(tv_usec));
        if (nread != sizeof(tv_usec)) {
            break;
        }
        tv_usec = obuf[0] | obuf[1] << 8
                | obuf[2] << 16 | obuf[3] << 24;
        nread = read(STDIN_FILENO, obuf, sizeof(len));
        if (nread != sizeof(len)) {
            break;
        }
        len = obuf[0] | obuf[1] << 8
            | obuf[2] << 16 | obuf[3] << 24;
        if (len <= 0) {
            break;
        }
        obuf = realloc(obuf, len);
        nread = read(STDIN_FILENO, obuf, len);
        if (nread != len) {
            break;
        }
        parse(&term, obuf, nread);
        refresh(&pb, &term);

        /* take screenshot */
        apply_colormap(&pb, img);
        delay = (tv_sec - sec) * 1000000 + tv_usec - usec;
        if (delay >= 0 && delay < 1000000) {
            controlgif(gsdata, -1, (delay + 5000) / 10000 + 1, 0, 0);
        }
        sec = tv_sec;
        usec = tv_usec;

        putgif(gsdata, img);
    }

    /* output gif */
    gifsize = endgif(gsdata);
    if (gifsize > 0) {
        write_gif(gifimage, gifsize);
        free(gifimage);
    }
    free(img);

    /* normal exit */
    term_die(&term);
    pb_die(&pb);
    free(obuf);

    return EXIT_SUCCESS;
}

