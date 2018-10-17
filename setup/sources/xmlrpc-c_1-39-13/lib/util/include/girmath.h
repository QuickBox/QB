#ifndef __GIRMATH_H
#define __GIRMATH_H

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define SQR(a) ((a)*(a))

/* ROUNDUP(a,b) rounds a up to the next multiple of b.
   ROUNDDN(a,b) rounds a down to the next multiple of b.
   ROUND(a,b) rounds a to the nearest multiple of b.

   These are for signed integers.

   "down" means arithmetically less: -4.3 rounded down to the next unit
   is -5.

   ROUNDUPU, etc. are the same for unsigned integers, but with no compiler
   warning about a comparison that is always true.
*/
   
#define ROUNDUPU(a,b) (((a)+(b)-1)/(b)*(b))
#define ROUNDDNU(a,b) ((a)/(b)*(b))
#define ROUNDU(a,b) (((a)+((b)/2))/(b)*(b))

#define ROUNDUP(a,b) ((a) >= 0 ? ROUNDUPU(a,b) : -ROUNDDNU(-a,b))
#define ROUNDDN(a,b) ((a) >= 0 ? ROUNDDNU(a,b) : -ROUNDUPU(-a,b))
#define ROUND(a,b) ((a) >= 0 ? ROUNDU(a,b) : -ROUNDU(-a,b))

/* ROUNDUP_DIV gives the quotient rounded up to the next integer.
   It works only on unsigned numbers
*/

#define ROUNDUP_DIV(dividend, divisor) ((dividend) + (divisor) - 1)/(divisor)

#define IS_POWER_OF_TWO(n) ((((n)-1) & (n)) == 0x0)

#endif

