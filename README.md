seq2gif
=======
[![Build Status](https://travis-ci.org/saitoha/seq2gif.svg?branch=master)](https://travis-ci.org/saitoha/seq2gif)

Description
------------

Convert a ttyrec record into a gif animation directly
(almost vt102 compatible terminal emulation).


Usage
-----

```
seq2gif < ttyrecord > record.gif

Options:
-w, --width           specify terminal width in cell size
-h, --height          specify terminal height in cell size
-V  --version         show version
```


Acknowledgements
----------------

### recterm

This program is derived from [recterm](https://github.com/uobikiemukot/recterm)
written by haru <uobikiemukot at gmail dot com> (@uobikiemukot)

### gifsave89

GIF encoder (gifsave89.c) is imported from [gifsave89](http://www.forkosh.com/gifsave90.html)
distributed under GPLv3+ by [John Forkosh Associates, Inc.](http://www.forkosh.com)(john@forkosh.com)

### milkjf
This package includes a C header file derived from milkjf font(gryph/milkjf.h).
Original milkjf font was designed by japanfarm (Nihon-Nouen) and used on Sharp X68000 series.
Chisato Yamauchi converted milkjf font to bdf for X Window System.
haru(@uobikiemukot) converted it to a C-styled header file.

### M+ BITMAP FONTS
This package includes a C header file derived from M+ font(gryph/mplus.h).

Original License:

```
M+ BITMAP FONTS            Copyright 2002-2005  COZ <coz@users.sourceforge.jp>

LICENSE

These fonts are free softwares.
Unlimited permission is granted to use, copy, and distribute it, with
or without modification, either commercially and noncommercially.
THESE FONTS ARE PROVIDED "AS IS" WITHOUT WARRANTY.
```

### Markus Kuhn's wcwidth
This package includes Markus Kuhn's wcwidth(mk_wcwidth)

```
Markus Kuhn -- 2007-05-26 (Unicode 5.0)

Permission to use, copy, modify, and distribute this software
for any purpose and without fee is hereby granted. The author
disclaims all warranties with regard to this software.

Latest version: http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
```

License
-------

![GPLv3](https://raw.githubusercontent.com/saitoha/seq2gif/master/images/gplv3.png)

