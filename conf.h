/*
	Copyright (C) 2014 haru <uobikiemukot at gmail dot com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* color: index number of color_palette[] (see color.h) */
enum {
	DEFAULT_FG           = 7,
	DEFAULT_BG           = 0,
	ACTIVE_CURSOR_COLOR  = 2,
	PASSIVE_CURSOR_COLOR = 1,
};

/* misc */
enum {
	DEBUG            = false,  /* write dump of input to stdout, debug message to stderr */
	TABSTOP          = 8,      /* hardware tabstop */
	BACKGROUND_DRAW  = false,  /* always draw even if vt is not active */
	SUBSTITUTE_HALF  = 0x0020, /* used for missing glyph (single width): U+0020 (SPACE) */
	SUBSTITUTE_WIDE  = 0x3000, /* used for missing glyph (double width): U+3000 (IDEOGRAPHIC SPACE) */
	REPLACEMENT_CHAR = 0x003F, /* used for malformed UTF-8 sequence    : U+003F (QUESTION MARK)  */
};

/* TERM value */
const char *term_name = "yaft-256color";

/* framubuffer device */
#if defined(__linux__)
	const char *fb_path = "/dev/fb0";
#elif defined(__FreeBSD__)
	const char *fb_path = "/dev/tty";
#elif defined(__NetBSD__)
	const char *fb_path = "/dev/ttyE0";
#elif defined(__OpenBSD__)
	const char *fb_path = "/dev/ttyC0";
#endif
//const char *fb_path = "/dev/graphics/fb0"; /* for Android */

/* shell */
#if defined(__linux__)
	const char *shell_cmd = "/bin/bash";
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	const char *shell_cmd = "/bin/csh";
#else
	const char *shell_cmd = "/bin/bash";
#endif
//const char *shell_cmd = "/system/bin/sh"; /* for Android */
