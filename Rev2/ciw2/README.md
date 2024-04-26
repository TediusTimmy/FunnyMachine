CallIfWhile
===========

Alternative version switches between stack-mode and belt-mode when convenient. This should generate more efficient code that is also correct.

Multiplication has been implemented in software. Division and modulus have also been implemented in software, but are always unsigned.

Division isn't actually quite right. There are still corner cases where it doesn't work. This is mostly due to me not wanting to implement proper checks. Let's go over the problem.  
In the division algorithm, there is this check:  
`      if (run >= dor)`  
It is implemented as:  
`      if ((run - dor) >= 0)`  
This may look like a valid check, but we aren't accounting for unsigned overflow. What it should look like is:  
```
      tmp = run - dor;
      if ((0 == tmp) || (((run & -dor) | ((run | -dor) & ~tmp)) & 0x8000))
```
The first part is trivial: is it equal to zero. All of that other stuff is computing if there was a carry out. Then, we run into the actual actual implementation:  
```
        SUB 0, dor
 @dnd   ADD dnd, dnd
 @dor   PICK dor, dor, dor
 @cnt   DEC cnt
 @quo   PICKZP 3, 7, 8
 @run   PICKZP 4, 4, 5
```
This conditional isn't actually implemented with a branch. It has been reduced to a conditional move. However, this conditional move is contingent on being able to easily check the unsigned result of `run - dor`. In order to do this correctly, I think I need a real branch and some scratchpad locations. And I don't want this algorithm to use either. This seems like a machine that: if you want to do heavy algorithms, you'll have to be smarter or fight the hardware.
