seq2gif
=======

Description
------------

Convert a ttyrec record into gif animation directly
(almost vt102 compatible terminal emulation).


Acknowledgements
----------------

This program is derived from [recterm](https://github.com/uobikiemukot/recterm)
written by haru <uobikiemukot at gmail dot com> (@uobikiemukot)

GIF encoder (gifsave89.c) is imported from [gifsave89](http://www.forkosh.com/gifsave90.html)
distributed under GPLv3+ by [John Forkosh Associates, Inc.](http://www.forkosh.com)(john@forkosh.com)

    Copyright(c) 2012-2012, John Forkosh Associates, Inc. All rights reserved.
              http://www.forkosh.com   mailto: john@forkosh.com
    Copyright (C) 2014 haru <uobikiemukot at gmail dot com>
    Copyright (C) 2014 Hayaki Saito <user@zuse.jp>
   
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

    [!GPLv3](https://raw.githubusercontent.com/saitoha/seq2gif/master/images/gplv3.png)

Usage
-----

```
seq2gif < ttyrecord | img2sixel
```

