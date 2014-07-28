/*
 * Copyright(c) 2012-2012, John Forkosh Associates, Inc. All rights reserved.
 *           http://www.forkosh.com   mailto: john@forkosh.com
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

/* --- program id --- */
#define        PROGRAMID     "gifsave89"        /* program name */
#define        GIFSAVE_VERSION       "1.00"        /* version number */
#define REVISIONDATE  "  10 Oct 2012  "        /* date of most recent revision */
#define COPYRIGHTTEXT "Copyright(c) 2012-2012, John Forkosh Associates, Inc"
/* --- program messages --- */
static char *copyright =                /* gnu/gpl copyright notice */
        "+-----------------------------------------------------------------------+\n"
        "|" PROGRAMID " ver" GIFSAVE_VERSION ", " COPYRIGHTTEXT "|\n"
        "|               (  current revision:" REVISIONDATE
        ")                    |\n"
        "+-----------------------------------------------------------------------+\n"
        "|gifsave89 is free software, licensed to you under terms of the GNU/GPL,|\n"
        "|           and comes with absolutely no warranty whatsoever.           |\n"
        "|     See http://www.forkosh.com/gifsave89.html for further details.    |\n"
        "+-----------------------------------------------------------------------+";

/* -------------------------------------------------------------------------
url of cgi to pixelize plaintext expressions (see plainmimetext() below)...
-------------------------------------------------------------------------- */
/* --- pixelizing cgi --- */
#if !defined(MIMETEX)
#define MIMETEX "http://www.forkosh.com/mimetex.cgi"
#endif
/* --- local /path/to/wget to run the cgi (-DWGET to supply path/to) --- */
#if !defined(WGET)
#define WGET "wget"
#endif

/* -------------------------------------------------------------------------
gifsave89 data and structures...
-------------------------------------------------------------------------- */
/* ---
 * gifsave89 datatypes
 * ---------------------- */
typedef uint8_t BYTE;                        /* one byte (8 bits) */
typedef uint16_t WORD;                        /* used for two byte integers */

/* ---
 * gif image buffer (note: ptr to caller's buffer address is BYTE **block)
 * ----------------------------------------------------------------------- */
#define        BLOCK struct _Block
#define        BK BLOCK                                /* shorthand for funcs and args */
#define        MAXBLKBYTES  (1024)                /* initial gif buffer allocation */
BLOCK {
        BYTE **block;                                /* block buffer */
        int nblkbytes,                                /* #bytes already in our block */
         maxblkbytes;                                /* #bytes currently (re)allocated */
};                                                                /* --- end-of-struct BLOCK --- */

/* ---
 * data subblock (as per gif standard)
 * -------------------------------------- */
#define        SUBBLOCK struct _SubBlock
#define        SB SUBBLOCK                                /* shorthand for funcs and args */
#define        SUBBLOCKSIZE (255)                /* maximum gif subblock size */
#define        BITSPERBYTE    (8)                /* #bits per byte */
SUBBLOCK {
        BYTE subblock[SUBBLOCKSIZE + 1];        /* subblock buffer */
        BLOCK *block;                                /* parent block to flush subblock */
        int nsubbytes,                                /* #bytes already in our subblock */
         nsubbits;                                        /* #bits  already in current byte */
        int index;                                        /* >=0 writes subblock index# byte */
};                                                                /* --- end-of-struct SUBBLOCK --- */

/* ---
 * LZW parameters
 * ----------------- */
#define        POW2(n)   ((int)(1<<(n)))        /* 2**n */
#define RESCODES  (2)
#define HASHFREE  (0xFFFF)
#define NEXTFIRST (0xFFFF)
#define CODEBITS  (12)
#define NSTRINGS  POW2((CODEBITS))
#define HASHSIZE  (9973)
#define HASHSTEP  (2039)
#define HASH(index,byte) ( (((byte) << 8) ^ (index)) % (HASHSIZE) )

/* ---
 * "global" gifsave89 data
 * Note: gifsave89 is reentrant by keeping all variables that need to
 * be maintained from call to call in this struct, i.e., no globals.
 * --------------------------------------------------------------------- */
#define        GIFSAVE89 struct _GifSave89
#define        GS GIFSAVE89                        /* shorthand for funcs and args */
GIFSAVE89 {
        /* ---
         * gif image block and subblock buffers
         * --------------------------------------- */
        BLOCK gifimage;                                /* buffer for gif image block */
        SUBBLOCK gifsubblock;                /* buffer for gifimage subblock */
        /* ---
         * LZW string table
         * ------------------- */
        BYTE strbyte[NSTRINGS];
        int strnext[NSTRINGS], strhash[HASHSIZE], nstrings;
        /* ---
         * additional control data
         * -------------------------- */
        int version;                                /* 87=GIF87a, 89=GIF89a */
        int width, height;                        /* #row, #col pixels for screen */
        int ncolorbits_gct,                        /* #bits/index (global color table) */
         ncolorbits;                                /* local(if given) or global #bits */
        int bgindex;                                /* global&local colortable bgindex */
        int npixels;                                /* width*height of current image */
        int ncontrol;                                /* #controlgif calls before putgif */
        int isanimated,                                /* true if animategif() called */
         delay, tcolor, disposal;        /* animation frame defaults */
        /* ---
         * plaintext control data
         * ------------------------- */
        int isplaintext;                        /* plaintext flag, 1or2 if present */
        int pt_left, pt_top;                /* col,row of top-left corner */
        int pt_bg, pt_fg;                        /* bg,fg colortable indexes */
        int pt_width, pt_height;        /* width,height of pixelized data */
        char pt_data[1024];                        /* local copy of user text data */
        BYTE *pt_pixels;                        /* pixelized data (kept if flag=2) */
        /* ---
         * debugging control data
         * ------------------------- */
        int msglevel;                                /* desired verbosity (0=none) */
        void *msgfp;                                /* debug file ptr */
};                                                                /* --- end-of-struct GIFSAVE89 --- */


void *newgif(void **gifimage, int width, int height, int *colors, int bgindex);
int controlgif(GS * gs, int tcolor, int delay, int userinput, int disposal);
int animategif(GS * gs, int nrepetitions, int delay, int tcolor, int disposal);
int putgif(GS * gs, void *pixels);
int endgif(GS * gs);
