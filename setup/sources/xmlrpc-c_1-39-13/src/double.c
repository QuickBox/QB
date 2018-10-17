#include <assert.h>
#include <stdlib.h>
#include <float.h>

#include "xmlrpc-c/util.h"
#include "xmlrpc-c/util_int.h"

#include "double.h"

typedef struct {
    char * bytes;
        /* NULL means there has been a memory allocation failure.
           bufferConcat() still works in this case, because we dont' want
           callers to have to deal with the out-of-memory possibility;
           it's just a no-op.
        */
    char * next;
    char * end;
} buffer;


static void
bufferInit(buffer * const bufferP) {

    unsigned int const initialSize = 64;

    bufferP->bytes = malloc(initialSize);

    if (bufferP->bytes) {
        bufferP->next = bufferP->bytes;
        bufferP->end  = bufferP->bytes + initialSize;
    }
}



static void
bufferConcat(buffer * const bufferP,
             char     const newChar) {

    if (bufferP->bytes) {
        if (bufferP->next >= bufferP->end) {
            size_t const oldSize = bufferP->end - bufferP->bytes;
            size_t const newSize = oldSize + 64;
            bufferP->bytes = realloc(bufferP->bytes, newSize);
            bufferP->next = bufferP->bytes + oldSize;
            bufferP->end  = bufferP->bytes + newSize;
        }

        if (bufferP->bytes)
            *(bufferP->next++) = newChar;
    }
}



static char
digitChar(unsigned int const digitValue) {

    assert(digitValue < 10);

    return '0' + digitValue;
}



static unsigned int
leadDigit(double const arg,
          double const precision) {
/*----------------------------------------------------------------------------
   Assuming 'arg' has one digit before the decimal point (which may be zero),
   return that digit.

   We assume the precision of 'arg' is plus or minus 'precision', and bias our
   estimation of the first digit up.  We do that bias in order to bias toward
   shorter decimal ciphers: It's cleaner to consider 2.9999999 to be 3 than to
   consider 3 to be 2.999999.
-----------------------------------------------------------------------------*/
    return MIN(9, (unsigned int)(arg + precision));
}



static void
floatWhole(double   const value,
           buffer * const formattedP,
           double * const formattedAmountP,
           double * const precisionP) {
/*----------------------------------------------------------------------------
   Format into *formattedP the whole part of 'value', i.e. the part before the
   decimal point.

   Return as *formattedAmountP the whole amount; e.g. if 'value' is 35.2,
   we return *formattedAmountP = 35.

   As there is imprecision involved in our calculations, return as *precisionP
   the maximum difference there may be be between 'double' and what we
   formatted.
-----------------------------------------------------------------------------*/
    if (value < 1.0) {
        /* No digits to add to the whole part */
        *formattedAmountP = 0;
        *precisionP       = DBL_EPSILON;
    } else {
        double nonLeastAmount;
        double nonLeastPrecision;
        unsigned int leastValue;

        /* Add all digits but the least significant to *formattedP */

        floatWhole(value/10.0, formattedP, &nonLeastAmount,
                   &nonLeastPrecision);

        /* Add the least significant digit to *formattedP */

        if (nonLeastPrecision > 0.1) {
            /* We're down in the noise now; no point in showing any more
               significant digits (and we couldn't if we wanted to, because
               nonLeastPrecision * 10 might be more than 10 less than
               'value').
            */
            leastValue = 0;
        } else
            leastValue = leadDigit(value - nonLeastAmount * 10,
                                   nonLeastPrecision * 10);

        bufferConcat(formattedP, digitChar(leastValue));
        
        *formattedAmountP = nonLeastAmount * 10 + leastValue;
        *precisionP       = nonLeastPrecision * 10;
    }        
}



static void
floatFractionPart(double   const value,
                  double   const wholePrecision,
                  buffer * const formattedP) {
/*----------------------------------------------------------------------------
   Serialize the part that comes after the decimal point, assuming there
   is something (nonzero) before the decimal point that uses up all but
   'wholePrecision' of the available precision.
-----------------------------------------------------------------------------*/
    double precision;
    double d;

    assert(value < 1.0);

    for (d = value, precision = wholePrecision;
         d > precision;
         precision *= 10) {

        unsigned int digitValue;

        d *= 10;
        digitValue = leadDigit(d, precision);

        d -= digitValue;

        assert(d < 1.0);

        bufferConcat(formattedP, digitChar(digitValue));
    }
}



static void
floatFraction(double   const value,
              buffer * const formattedP) {
/*----------------------------------------------------------------------------
   Serialize the part that comes after the decimal point, assuming there
   is nothing before the decimal point.
-----------------------------------------------------------------------------*/
    double precision;
    double d;

    assert(0.0 < value && value < 1.0);

    /* Do the leading zeroes, which eat no precision */

    for (d = value * 10; d < 1.0; d *= 10)
        bufferConcat(formattedP, '0');

    /* Now the significant digits */

    precision = DBL_EPSILON;

    while (d > precision) {
        unsigned int const digitValue = leadDigit(d, precision);

        bufferConcat(formattedP, digitChar(digitValue));

        d -= digitValue;

        assert(d < 1.0);

        d *= 10;
        precision *= 10;
    }
}



void
xmlrpc_formatFloat(xmlrpc_env *  const envP,
                   double        const value,
                   const char ** const formattedP) {

    double absvalue;
    buffer formatted;

    bufferInit(&formatted);

    if (value < 0.0) {
        bufferConcat(&formatted, '-');
        absvalue = - value;
    } else
        absvalue = value;

    if (absvalue >= 1.0) {
        double wholePart;
        double wholePrecision;

        floatWhole(absvalue, &formatted, &wholePart, &wholePrecision);

        if (wholePrecision >= 1.0) {
            /* We ran out of precision before we got to the decimal point */
        } else {
            double const fractionPart = absvalue - wholePart;

            if (fractionPart > wholePrecision) {
                bufferConcat(&formatted, '.');

                floatFractionPart(fractionPart, wholePrecision, &formatted);
            }
        }    
    } else {
        bufferConcat(&formatted, '0');

        if (absvalue > 0.0) {
            bufferConcat(&formatted, '.');
            floatFraction(absvalue, &formatted);
        }
    }
    bufferConcat(&formatted, '\0');

    if (formatted.bytes == NULL)
        xmlrpc_faultf(envP, "Couldn't allocate memory to format %g", value);
    else
        *formattedP = formatted.bytes;
}
