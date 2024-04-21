#include <stdio.h>

void divmod(short dnd, short dor, short* q, short* r)
 {
   short quo, run, count;
   if (dnd == 0)
    {
      *q = 0;
      *r = 0;
      return;
    }
   if (dor == 0)
    {
      *q = 0;
      *r = dnd;
      return;
    }
   count = 16;
   while (0 == (dnd & 0x8000))
    {
      dnd <<= 1;
      --count;
    }
   dnd <<= 1;
   --count;
   quo = 0;
   run = 1;
   while (count != 0)
    {
      quo <<= 1;
      run <<= 1;
      run |= ((dnd >> 15) & 1);
      dnd <<= 1;
      --count;
      if (run >= dor)
       {
         run -= dor;
         quo |= 1;
       }
    }
   *q = quo;
   *r = run;
 }

int main (void)
 {
   short a, b, c, d;
   scanf("%hd%hd", &a, &b);
   divmod(a, b, &c, &d);
   printf("\nquot: %d\nrem: %d\n", c, d);
   return 0;
 }
