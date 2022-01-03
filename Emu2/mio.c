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
#ifdef USE_CURSES
 #include <curses.h>
#endif /* USE_CURSES */
#ifdef USE_WAPI
 #include <stdio.h>
 #include <windows.h>
 #include <conio.h>
#endif /* USE_WAPI */
#ifdef USE_CONIO
 #include <conio.h>
#endif /* USE_CONIO */
#ifdef USE_TERMIOS
 #include <termios.h>
 #include <unistd.h>
 #include <stdio.h>
#endif /* USE_TERMIOS */

void INIS (void) {
#ifdef USE_CURSES
   int i, j, k;
   initscr();
   noecho();
   cbreak();
   curs_set(0);
   start_color();
   assume_default_colors(0, 0);
   for (i = 1; i < 64; i++) {
      j = i & 7;
      k = i >> 3;
      if ((j & 5) && (j != 7)) j ^= 5;
      if ((k & 5) && (k != 7)) k ^= 5;
      init_pair (i, j, k);
   }
#endif /* USE_CURSES */
#ifdef USE_TERMIOS
   struct termios tochange;
   tcgetattr(STDIN_FILENO, &tochange);
   tochange.c_lflag = ISIG | IEXTEN;
   tcsetattr(STDIN_FILENO, TCSANOW, &tochange);
#endif /* USE_TERMIOS */
#ifdef USE_CONIO
   _setcursortype(_NOCURSOR);
#endif /* USE_CONIO*/
#ifdef USE_WAPI
   CONSOLE_CURSOR_INFO x;
   GetConsoleCursorInfo (GetStdHandle(STD_OUTPUT_HANDLE), &x);
   x.bVisible = FALSE;
   SetConsoleCursorInfo (GetStdHandle(STD_OUTPUT_HANDLE), &x);
   SetConsoleTextAttribute (GetStdHandle(STD_OUTPUT_HANDLE), 7);
#endif /* USE_WAPI */
   return;
}

void DINI (void) {
#ifdef USE_CURSES
   endwin();
#endif /* !USE_CURSES */
#ifdef USE_TERMIOS
   struct termios tochange;
   tcgetattr(STDIN_FILENO, &tochange);
   tochange.c_lflag = TTYDEF_LFLAG;
   tcsetattr(STDIN_FILENO, TCSANOW, &tochange);
   printf("\33[H\33[m");
#endif /* USE_TERMIOS */
#ifdef USE_CONIO
   _setcursortype(_NORMALCURSOR);
   textcolor(LIGHTGRAY);
   textbackground(BLACK);
#endif /* USE_CONIO*/
#ifdef USE_WAPI
   CONSOLE_CURSOR_INFO x;
   GetConsoleCursorInfo (GetStdHandle(STD_OUTPUT_HANDLE), &x);
   x.bVisible = TRUE;
   SetConsoleCursorInfo (GetStdHandle(STD_OUTPUT_HANDLE), &x);
#endif /* USE_WAPI */
   return;
}

#ifdef USE_TERMIOS
 #ifdef USE_UTF8

int CP437_2_Unicode [256] =
{
   0x2007, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
   0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
   0x25ba, 0x25c4, 0x2195, 0x203c, 0xb6, 0xa7, 0x25ac, 0x21a8,
   0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc,
   ' ', '!', '"', '#', '$', '%', '&', '\'',
   '(', ')', '*', '+', ',', '-', '.', '/',
   '0', '1', '2', '3', '4', '5', '6', '7',
   '8', '9', ':', ';', '<', '=', '>', '?',
   '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
   'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
   'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
   'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
   '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
   'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
   'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
   'x', 'y', 'z', '{', '|', '}', '~', 0x2302,
   0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7,
   0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
   0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9,
   0xff, 0xd6, 0xdc, 0xa2, 0xa3, 0xa5, 0x20a7, 0x192,
   0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba,
   0xbf, 0x2310, 0xac, 0xbd, 0xbc, 0xa1, 0xab, 0xbb,
   0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
   0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
   0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
   0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
   0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
   0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
   0x381, 0x382, 0x383, 0x3c0, 0x3a3, 0x3c3, 0xb5, 0x3c4,
   0x3a6, 0x398, 0x3a9, 0x384, 0x221e, 0x2205, 0x2208, 0x2229,
   0x2261, 0xb1, 0x2265, 0x2264, 0x2320, 0x2321, 0xf7, 0x2248,
   0xb0, 0x2219, 0xb7, 0x221a, 0x207f, 0xb2, 0x25a0, 0xa0
};

unsigned long UTF32_2_UTF8 (unsigned long c)
{
   if (c < 128);
   else if (c < 2048)
      c = ((c & 0x3F) << 8) | ((c & 0x1C0) >> 6) | 0x80C0;
   else if (c < 65536)
      c = ((c & 0x3F) << 16) | ((c & 0xFC0) << 2) | ((c & 0xF000) >> 12) |
          0x8080E0;
   else
      c = ((c & 0x3F) << 24) | ((c & 0xFC0) << 10) | ((c & 0x3F000) >> 4) |
          ((c & 0x1C0000) >> 18) | 0x808080F0;
   return c;
}

void myputch (int c)
{
   unsigned long r;

   c &= 255;
   r = (unsigned long) UTF32_2_UTF8(CP437_2_Unicode[c]);

   while (r)
   {
      putchar(r & 255);
      r >>= 8;
   }

   return;
}
 #endif /* USE_UTF8 */
#endif /* USE_TERMIOS */


void GOTO (int x, int y)
 {
#ifdef USE_CURSES
   move(y, x);
#endif /* !USE_CURSES */
#ifdef USE_TERMIOS
   printf("\33[%d;%dH", y + 1, x + 1);
#endif /* USE_TERMIOS */
#ifdef USE_CONIO
   gotoxy (x + 1, y + 1);
#endif /* USE_CONIO */
#ifdef USE_WAPI
   SetConsoleCursorPosition (GetStdHandle(STD_OUTPUT_HANDLE), (COORD) {x, y});
#endif /* USE_WAPI */
   return;
 }

void COLOR (int x)
 {
#ifdef USE_CURSES
   color_set((x & 7) | ((x & 112) >> 1), NULL);
   if (x & 8) attron(A_BOLD); else attroff(A_BOLD);
   if (x & 128) attron(A_BLINK); else attroff(A_BLINK);
#endif /* !USE_CURSES */
#ifdef USE_TERMIOS
   if ((x & 5) && ((x & 7) != 7)) x ^= 5;
   if ((x & 80) && ((x & 112) != 112)) x ^= 80;
   printf("\33[3%d;4%dm", x & 7, (x >> 4) & 7);
   if (x & 8) printf("\33[1m");
   else printf("\33[22m");
   if (x & 128) printf("\33[5m");
   else printf("\33[25m");
#endif /* USE_TERMIOS */
#ifdef USE_CONIO
   textcolor (x & 15);
   textbackground ((x >> 4) & 15);
#endif /* USE_CONIO*/
#ifdef USE_WAPI
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
#endif /* USE_WAPI */
   return;
 }

int GETC (void) {
#ifdef USE_TERMIOS
   return getchar();
#else /* !USE_TERMIOS */
   return getch();
#endif
}

void PUTC (int x) {
#ifdef USE_TERMIOS
 #ifdef USE_UTF8
   myputch(x);
 #else
   putchar(x);
 #endif /* USE_UTF8 */
#endif /* !USE_TERMIOS */
#ifdef USE_CURSES
   echochar(x);
#endif /* USE_CURSES */
#ifdef USE_CONIO
   putch(x);
#endif /* USE_CONIO */
#ifdef USE_WAPI
   putchar(x);
#endif /* USE_WAPI */
   return;
}
