#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED

#include <sys/types.h>

#include "bool.h"
#include "conn.h"

/*********************************************************************
** Request
*********************************************************************/

bool RequestValidURI(TSession * const r);
bool RequestValidURIPath(TSession * const r);
bool RequestUnescapeURI(TSession *r);

void
RequestRead(TSession *    const sessionP,
            uint32_t      const timeout,
            const char ** const errorP,
            uint16_t *    const httpErrorCodeP);

void RequestInit(TSession * const r,TConn * const c);
void RequestFree(TSession * const r);

/*********************************************************************
** HTTP
*********************************************************************/

const char *
HTTPReasonByStatus(uint16_t const code);

int32_t
HTTPRead(TSession *   const sessionP,
         const char * const buffer,
         uint32_t     const len);

bool
HTTPWriteBodyChunk(TSession *   const sessionP,
                   const char * const buffer,
                   uint32_t     const len);

bool
HTTPWriteEndChunk(TSession * const sessionP);

bool
HTTPKeepalive(TSession * const sessionP);

bool
HTTPWriteContinue(TSession * const sessionP);

#endif
