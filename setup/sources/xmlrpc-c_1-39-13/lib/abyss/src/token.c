#include <stddef.h>

#include "bool.h"

#include "token.h"


void
NextToken(const char ** const pP) {

    bool gotToken;

    gotToken = false;

    while (!gotToken) {
        switch (**pP) {
        case '\t':
        case ' ':
            ++(*pP);
            break;
        default:
            gotToken = true;
        };
    }
}



char *
GetToken(char ** const pP) {

    char * p0;
        
    p0 = *pP;

    while (1) {
        switch (**pP) {
        case '\t':
        case ' ':
        case '\r':
        case '\n':
        case '\0':
            if (p0 == *pP)
                return NULL;

            if (**pP) {
                **pP = '\0';
                ++(*pP);
            };
            return p0;

        default:
            ++(*pP);
        };
    }
}



void
GetTokenConst(char **       const pP,
              const char ** const tokenP) {

    *tokenP = GetToken(pP);
}
