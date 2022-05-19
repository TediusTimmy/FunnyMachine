Alternative
===========

I wanted to try out computing expressions on the belt, rather than using a stack. It is more likely to produce code that doesn't work, but will produce more efficient code when it does work.

Most notably, don't expect these to work:
```
   val <- 3 + doThing(5)
   // Computing doThing will push 3 off the belt

   if x < 3 & y > 7 then
   // Relational operators mess up belt references. Always do:
   if x < 3 && y > 7 then
```
