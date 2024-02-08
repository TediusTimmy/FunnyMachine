/*
   Copyright (C) 2009 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef mio_h
#define mio_h

void GOTO (int x, int y);
void COLOR (int);

void PUTC (int);
int GETC (void);

void INIS (void); //Call this before doing anything.
void DINI (void); //And this once you're done

#endif /* mio.h */
