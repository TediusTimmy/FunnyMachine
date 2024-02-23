#include <stdio.h>

void divmod(short dnd, short dor, short* q, short* r)
 {
   short quo, bit;
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
   quo = 0;
   bit = 1;
   while (dor < dnd)
    {
      dor <<= 1;
      bit <<= 1;
    }
   while (bit != 0)
    {
      if (dnd >= dor)
       {
         dnd -= dor;
         quo += bit;
       }
      dor >>= 1;
      bit >>= 1;
    }
   *q = quo;
   *r = dnd;
 }

int main (void)
 {
   short a, b, c, d;
   scanf("%hd%hd", &a, &b);
   divmod(a, b, &c, &d);
   printf("\nquot: %d\nrem: %d\n", c, d);
   return 0;
 }
