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
#include "terminal.h"
#include "function.h"
#include "dcs.h"

#include <stdio.h>
#ifdef HAVE_CTYPE_H
# include <ctype.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif

static void (*ctrl_func[CTRL_CHARS])(struct terminal *term) = {
    [BS]  = bs,
    [HT]  = tab,
    [LF]  = nl,
    [VT]  = nl,
    [FF]  = nl,
    [CR]  = cr,
    [ESC] = enter_esc,
};

static void (*esc_func[ESC_CHARS])(struct terminal *term) = {
    ['7'] = save_state,
    ['8'] = restore_state,
    ['D'] = nl,
    ['E'] = crnl,
    ['H'] = set_tabstop,
    ['M'] = reverse_nl,
    ['P'] = enter_dcs,
    ['['] = enter_csi,
    [']'] = enter_osc,
    ['c'] = ris,
};

static void (*csi_func[ESC_CHARS])(struct terminal *term, struct parm_t *) = {
    ['@'] = insert_blank,
    ['A'] = curs_up,
    ['B'] = curs_down,
    ['C'] = curs_forward,
    ['D'] = curs_back,
    ['E'] = curs_nl,
    ['F'] = curs_pl,
    ['G'] = curs_col,
    ['H'] = curs_pos,
    ['J'] = erase_display,
    ['K'] = erase_line,
    ['L'] = insert_line,
    ['M'] = delete_line,
    ['P'] = delete_char,
    ['X'] = erase_char,
    ['a'] = curs_forward,
    ['d'] = curs_line,
    ['e'] = curs_down,
    ['f'] = curs_pos,
    ['g'] = clear_tabstop,
    ['h'] = set_mode,
    ['l'] = reset_mode,
    ['m'] = set_attr,
    ['r'] = set_margin,
    //['s'] = save_state,
    //['u'] = restore_state,
    ['`'] = curs_col,
};

/* ctr char/esc sequence/charset function */
static void control_character(struct terminal *term, uint8_t ch)
{
    static const char *ctrl_char[] = {
        "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
        "BS ", "HT ", "LF ", "VT ", "FF ", "CR ", "SO ", "SI ",
        "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
        "CAN", "EM ", "SUB", "ESC", "FS ", "GS ", "RS ", "US ",
    };

    *term->esc.bp = '\0';

    if (DEBUG)
        fprintf(stderr, "ctl: %s\n", ctrl_char[ch]);

    if (ctrl_func[ch])
        ctrl_func[ch](term);
}

static void esc_sequence(struct terminal *term, uint8_t ch)
{
    *term->esc.bp = '\0';

    if (DEBUG)
        fprintf(stderr, "esc: ESC %s\n", term->esc.buf);

    if (strlen(term->esc.buf) == 1 && esc_func[ch])
        esc_func[ch](term);

    /* not reset if csi/osc/dcs seqence */
    if (ch == '[' || ch == ']' || ch == 'P')
        return;

    reset_esc(term);
}

static void csi_sequence(struct terminal *term, uint8_t ch)
{
    struct parm_t parm;

    *(term->esc.bp - 1) = '\0'; /* omit final character */

    if (DEBUG)
        fprintf(stderr, "csi: CSI %s\n", term->esc.buf + 1);

    reset_parm(&parm);
    parse_arg(term->esc.buf + 1, &parm, ';', isdigit); /* skip '[' */

    if (csi_func[ch])
        csi_func[ch](term, &parm);

    reset_esc(term);
}

static int is_osc_parm(int c)
{
    if (isdigit(c) || isalpha(c) ||
        c == '?' || c == ':' || c == '/' || c == '#')
        return true;
    else
        return false;
}

static void omit_string_terminator(char *bp, uint8_t ch)
{
    if (ch == BACKSLASH) /* ST: ESC BACKSLASH */
        *(bp - 2) = '\0';
    else                 /* ST: BEL */
        *(bp - 1) = '\0';
}

static void osc_sequence(struct terminal *term, uint8_t ch)
{
    int osc_mode;
    struct parm_t parm;

    omit_string_terminator(term->esc.bp, ch);

    if (DEBUG)
        fprintf(stderr, "osc: OSC %s\n", term->esc.buf);

    reset_parm(&parm);
    parse_arg(term->esc.buf + 1, &parm, ';', is_osc_parm); /* skip ']' */

    reset_esc(term);
}

static void dcs_sequence(struct terminal *term, uint8_t ch)
{
    char *cp;

    omit_string_terminator(term->esc.bp, ch);

    if (DEBUG)
        fprintf(stderr, "dcs: DCS %s\n", term->esc.buf);

    /* check DCS header */
    cp = term->esc.buf + 1; /* skip P */
    while (cp < term->esc.bp) {
        if (*cp == '{' || *cp == 'q')      /* DECDLD or sixel */
            break;
        else if (*cp == ';'                /* valid DCS header */
            || ('0' <= *cp && *cp <= '9'))
            ;
        else                               /* invalid sequence */
            cp = term->esc.bp;
        cp++;
    }

    if (cp != term->esc.bp) { /* header only or cannot find final char */
        /* parse DCS header */
        if (*cp == 'q')
            sixel_parse_header(term, term->esc.buf + 1);
        else if (*cp == '{')
            decdld_parse_header(term, term->esc.buf + 1);
    }

    reset_esc(term);
}

static void utf8_charset(struct terminal *term, uint8_t ch)
{
    if (0x80 <= ch && ch <= 0xBF) {
        /* check illegal UTF-8 sequence
            * ? byte sequence: first byte must be between 0xC2 ~ 0xFD
            * 2 byte sequence: first byte must be between 0xC2 ~ 0xDF
             * 3 byte sequence: second byte following 0xE0 must be between 0xA0 ~ 0xBF
             * 4 byte sequence: second byte following 0xF0 must be between 0x90 ~ 0xBF
             * 5 byte sequence: second byte following 0xF8 must be between 0x88 ~ 0xBF
             * 6 byte sequence: second byte following 0xFC must be between 0x84 ~ 0xBF
        */
        if ((term->charset.following_byte == 0)
            || (term->charset.following_byte == 1 && term->charset.count == 0 && term->charset.code <= 1)
            || (term->charset.following_byte == 2 && term->charset.count == 0 && term->charset.code == 0 && ch < 0xA0)
            || (term->charset.following_byte == 3 && term->charset.count == 0 && term->charset.code == 0 && ch < 0x90)
            || (term->charset.following_byte == 4 && term->charset.count == 0 && term->charset.code == 0 && ch < 0x88)
            || (term->charset.following_byte == 5 && term->charset.count == 0 && term->charset.code == 0 && ch < 0x84))
            term->charset.is_valid = false;

        term->charset.code <<= 6;
        term->charset.code += ch & 0x3F;
        term->charset.count++;
    }
    else if (0xC0 <= ch && ch <= 0xDF) {
        term->charset.code = ch & 0x1F;
        term->charset.following_byte = 1;
        term->charset.count = 0;
        return;
    }
    else if (0xE0 <= ch && ch <= 0xEF) {
        term->charset.code = ch & 0x0F;
        term->charset.following_byte = 2;
        term->charset.count = 0;
        return;
    }
    else if (0xF0 <= ch && ch <= 0xF7) {
        term->charset.code = ch & 0x07;
        term->charset.following_byte = 3;
        term->charset.count = 0;
        return;
    }
    else if (0xF8 <= ch && ch <= 0xFB) {
        term->charset.code = ch & 0x03;
        term->charset.following_byte = 4;
        term->charset.count = 0;
        return;
    }
    else if (0xFC <= ch && ch <= 0xFD) {
        term->charset.code = ch & 0x01;
        term->charset.following_byte = 5;
        term->charset.count = 0;
        return;
    }
    else { /* 0xFE - 0xFF: not used in UTF-8 */
        addch(term, REPLACEMENT_CHAR);
        reset_charset(term);
        return;
    }

    if (term->charset.count >= term->charset.following_byte) {
        /*    illegal code point (ref: http://www.unicode.org/reports/tr27/tr27-4.html)
            0xD800   ~ 0xDFFF : surrogate pair
            0xFDD0   ~ 0xFDEF : noncharacter
            0xnFFFE  ~ 0xnFFFF: noncharacter (n: 0x00 ~ 0x10)
            0x110000 ~        : invalid (unicode U+0000 ~ U+10FFFF)
        */
        if (!term->charset.is_valid
            || (0xD800 <= term->charset.code && term->charset.code <= 0xDFFF)
            || (0xFDD0 <= term->charset.code && term->charset.code <= 0xFDEF)
            || ((term->charset.code & 0xFFFF) == 0xFFFE || (term->charset.code & 0xFFFF) == 0xFFFF)
            || (term->charset.code > 0x10FFFF))
            addch(term, REPLACEMENT_CHAR);
        else
            addch(term, term->charset.code);

        reset_charset(term);
    }
}

void parse(struct terminal *term, uint8_t *buf, int size, int *pdirty)
{
    /*
        CTRL CHARS      : 0x00 ~ 0x1F
        ASCII(printable): 0x20 ~ 0x7E
        CTRL CHARS(DEL) : 0x7F
        UTF-8           : 0x80 ~ 0xFF
    */
    uint8_t ch;
    int i;

    for (i = 0; i < size; i++) {
        ch = buf[i];
        if (term->esc.state == STATE_RESET) {
            /* interrupted by illegal byte */
            if (term->charset.following_byte > 0 && (ch < 0x80 || ch > 0xBF)) {
                addch(term, REPLACEMENT_CHAR);
                reset_charset(term);
            }

            if (ch <= 0x1F)
                control_character(term, ch);
            else if (ch <= 0x7F)
                addch(term, ch);
            else
                utf8_charset(term, ch);
        }
        else if (term->esc.state == STATE_ESC) {
            if (push_esc(term, ch))
                esc_sequence(term, ch);
        }
        else if (term->esc.state == STATE_CSI) {
            if (push_esc(term, ch))
                csi_sequence(term, ch);
        }
        else if (term->esc.state == STATE_OSC) {
            if (push_esc(term, ch))
                osc_sequence(term, ch);
        }
        else if (term->esc.state == STATE_DCS) {
            if (push_esc(term, ch)) {
                dcs_sequence(term, ch);
                *pdirty = 1;
            }
        }
    }
}

/* emacs Local Variables:      */
/* emacs mode: c               */
/* emacs tab-width: 4          */
/* emacs indent-tabs-mode: nil */
/* emacs c-basic-offset: 4     */
/* emacs End:                  */
/* vim: set expandtab ts=4 : */
/* EOF */
