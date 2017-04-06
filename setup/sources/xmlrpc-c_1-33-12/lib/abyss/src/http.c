/* Copyright information is at the end of the file */

#define _XOPEN_SOURCE 600   /* For strdup() */
#define _BSD_SOURCE   /* For xmlrpc_strcaseeq() */

#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "xmlrpc_config.h"
#include "bool.h"
#include "mallocvar.h"
#include "xmlrpc-c/util.h"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/base64_int.h"
#include "xmlrpc-c/abyss.h"

#include "server.h"
#include "session.h"
#include "conn.h"
#include "token.h"
#include "date.h"
#include "data.h"

#include "http.h"

/*********************************************************************
** Request Parser
*********************************************************************/

/*********************************************************************
** Request
*********************************************************************/

static void
initRequestInfo(TRequestInfo * const requestInfoP,
                httpVersion    const httpVersion,
                const char *   const requestLine,
                TMethod        const httpMethod,
                const char *   const host,
                unsigned int   const port,
                const char *   const path,
                const char *   const query) {
/*----------------------------------------------------------------------------
  Set up the request info structure.  For information that is
  controlled by the header, use the defaults -- I.e. the value that
  applies if the request contains no applicable header field.
-----------------------------------------------------------------------------*/
    XMLRPC_ASSERT_PTR_OK(requestLine);
    XMLRPC_ASSERT_PTR_OK(path);

    requestInfoP->requestline = xmlrpc_strdupsol(requestLine);
    requestInfoP->method      = httpMethod;
    requestInfoP->host        = xmlrpc_strdupnull(host);
    requestInfoP->port        = port;
    requestInfoP->uri         = strdup(path);
    requestInfoP->query       = xmlrpc_strdupnull(query);
    requestInfoP->from        = NULL;
    requestInfoP->useragent   = NULL;
    requestInfoP->referer     = NULL;
    requestInfoP->user        = NULL;

    if (httpVersion.major > 1 ||
        (httpVersion.major == 1 && httpVersion.minor >= 1))
        requestInfoP->keepalive = TRUE;
    else
        requestInfoP->keepalive = FALSE;
}



static void
freeRequestInfo(TRequestInfo * const requestInfoP) {

    xmlrpc_strfreenull(requestInfoP->host);

    xmlrpc_strfreenull(requestInfoP->user);

    xmlrpc_strfree(requestInfoP->uri);

    xmlrpc_strfree(requestInfoP->requestline);
}



void
RequestInit(TSession * const sessionP,
            TConn *    const connectionP) {

    sessionP->validRequest = false;  /* Don't have valid request yet */

    time(&sessionP->date);

    sessionP->connP = connectionP;

    sessionP->responseStarted = FALSE;

    sessionP->chunkedwrite = FALSE;
    sessionP->chunkedwritemode = FALSE;

    sessionP->continueRequired = FALSE;

    ListInit(&sessionP->cookies);
    ListInit(&sessionP->ranges);
    TableInit(&sessionP->requestHeaderFields);
    TableInit(&sessionP->responseHeaderFields);

    sessionP->status = 0;  /* No status from handler yet */

    StringAlloc(&(sessionP->header));
}



void
RequestFree(TSession * const sessionP) {

    if (sessionP->validRequest)
        freeRequestInfo(&sessionP->requestInfo);

    ListFree(&sessionP->cookies);
    ListFree(&sessionP->ranges);
    TableFree(&sessionP->requestHeaderFields);
    TableFree(&sessionP->responseHeaderFields);
    StringFree(&(sessionP->header));
}



static char *
firstLfPos(TConn * const connectionP,
           char *  const lineStart) {
/*----------------------------------------------------------------------------
   Return a pointer in the connection's receive buffer to the first
   LF (linefeed aka newline) character in the buffer at or after 'lineStart'.

   If there is no LF in the buffer at or after 'lineStart', return NULL.
-----------------------------------------------------------------------------*/
    const char * const bufferEnd =
        connectionP->buffer.t + connectionP->buffersize;

    char * p;

    for (p = lineStart; p < bufferEnd && *p != LF; ++p);

    if (p < bufferEnd)
        return p;
    else
        return NULL;
}



static void
getLineInBuffer(TConn * const connectionP,
                char *  const lineStart,
                time_t  const deadline,
                char ** const lineEndP,
                bool *  const errorP) {
/*----------------------------------------------------------------------------
   Get a line into the connection's read buffer, starting at position
   'lineStart', if there isn't one already there.   'lineStart' is either
   within the buffer or just after it.

   Read the channel until we get a full line, except fail if we don't get
   one by 'deadline'.
-----------------------------------------------------------------------------*/
    bool error;
    char * lfPos;

    assert(lineStart <= connectionP->buffer.t + connectionP->buffersize);

    error = FALSE;  /* initial value */
    lfPos = NULL;  /* initial value */

    while (!error && !lfPos) {
        int const timeLeft = (int)(deadline - time(NULL));
        if (timeLeft <= 0)
            error = TRUE;
        else {
            lfPos = firstLfPos(connectionP, lineStart);
            if (!lfPos) {
                const char * readError;
                ConnRead(connectionP, timeLeft, NULL, NULL, &readError);
                if (readError) {
                    error = TRUE;
                    xmlrpc_strfree(readError);
                }
            }
        }
    }    
    *errorP = error;
    *lineEndP = lfPos + 1;
}



static bool
isContinuationLine(const char * const line) {

    return (line[0] == ' ' || line[0] == '\t');
}



static bool
isEmptyLine(const char * const line) {

    return (line[0] == '\n' || (line[0] == '\r' && line[1] == '\n'));
}



static void
convertLineEnd(char * const lineStart,
               char * const prevLineStart,
               char   const newVal) {
/*----------------------------------------------------------------------------
   Assuming a line begins at 'lineStart' and the line before it (the
   "previous line") begins at 'prevLineStart', replace the line
   delimiter at the end of the previous line with the character 'newVal'.

   The line delimiter is either CRLF or LF.  In the CRLF case, we replace
   both CR and LF with 'newVal'.
-----------------------------------------------------------------------------*/
    assert(lineStart >= prevLineStart + 1);
    *(lineStart-1) = newVal;
    if (prevLineStart + 1 < lineStart &&
        *(lineStart-2) == CR)
        *(lineStart-2) = newVal;
}



static void
getRestOfField(TConn *       const connectionP,
               char *        const lineEnd,
               time_t        const deadline,
               const char ** const fieldEndP,
               bool *        const errorP) {
/*----------------------------------------------------------------------------
   Given that the read buffer for connection *connectionP contains (at
   its current read position) the first line of an HTTP header field, which
   ends at position 'lineEnd', find the rest of it.

   Some or all of the rest of the field may be in the buffer already;
   we read more from the connection as necessary, but not if it takes past
   'deadline'.  In the latter case, we fail.

   We return the location of the end of the whole field as *headerEndP.
   We do not remove the field from the buffer, but we do modify the
   buffer so as to join the multiple lines of the field into a single
   line, and to NUL-terminate the field.
-----------------------------------------------------------------------------*/
    char * const fieldStart = connectionP->buffer.t + connectionP->bufferpos;

    char * fieldEnd;
        /* End of the field lines we've seen at so far */
    bool gotWholeField;
    bool error;

    fieldEnd = lineEnd;  /* initial value - end of 1st line */
        
    for (gotWholeField = FALSE, error = FALSE;
         !gotWholeField && !error;) {

        char * nextLineEnd;

        /* Note that we are guaranteed, assuming the HTTP stream is
           valid, that there is at least one more line in it.  Worst
           case, it's the empty line that marks the end of the headers.
        */
        getLineInBuffer(connectionP, fieldEnd, deadline,
                        &nextLineEnd, &error);
        if (!error) {
            if (isContinuationLine(fieldEnd)) {
                /* Join previous line to this one */
                convertLineEnd(fieldEnd, fieldStart, ' ');
                /* Add this line to the header */
                fieldEnd = nextLineEnd;
            } else {
                gotWholeField = TRUE;

                /* NUL-terminate the whole field */
                convertLineEnd(fieldEnd, fieldStart, '\0');
            }
        }
    }
    *fieldEndP = fieldEnd;
    *errorP    = error;
}



static void
readField(TConn * const connectionP,
          time_t  const deadline,
          bool *  const endOfHeaderP,
          char ** const fieldP,
          bool *  const errorP) {
/*----------------------------------------------------------------------------
   Read an HTTP header field, or the end of header empty line, on connection
   *connectionP.

   An HTTP header field is basically a line, except that if a line starts
   with white space, it's a continuation of the previous line.  A line
   is delimited by either LF or CRLF.

   The first line of an HTTP header field is never empty; an empty line
   signals the end of the HTTP header and beginning of the HTTP body.  We
   call that empty line the EOH mark.

   We assume the connection is positioned to a header or EOH mark.
   
   In the course of reading, we read at least one character past the
   line delimiter at the end of the field or EOH mark; we may read
   much more.  But we leave everything after the field or EOH (and
   its line delimiter) in the internal buffer, with the buffer pointer
   pointing to it.

   We use stuff already in the internal buffer (perhaps left by a
   previous call to this subroutine) before reading any more from from
   the channel.

   We return as *fieldP the next field as an ASCIIZ string, with no
   line delimiter.  That string is stored in the "unused" portion of
   the connection's internal buffer.  Iff there is no next field, we
   return *endOfHeaderP == true and nothing meaningful as *fieldP.
-----------------------------------------------------------------------------*/
    char * const bufferStart = connectionP->buffer.t + connectionP->bufferpos;

    bool error;
    char * lineEnd;

    getLineInBuffer(connectionP, bufferStart, deadline, &lineEnd, &error);

    if (!error) {
        if (isContinuationLine(bufferStart))
            error = TRUE;
        else if (isEmptyLine(bufferStart)) {
            /* Consume the EOH mark from the buffer */
            connectionP->bufferpos = lineEnd - connectionP->buffer.t;
            *endOfHeaderP = TRUE;
        } else {
            /* We have the first line of a field; there may be more. */

            const char * fieldEnd;

            *endOfHeaderP = FALSE;

            getRestOfField(connectionP, lineEnd, deadline,
                           &fieldEnd, &error);

            if (!error) {
                *fieldP = bufferStart;

                /* Consume the header from the buffer (but be careful --
                   you can't reuse that part of the buffer because the
                   string we will return is in it!
                */
                connectionP->bufferpos = fieldEnd - connectionP->buffer.t;
            }
        }
    }
    *errorP = error;
}



static void
skipToNonemptyLine(TConn * const connectionP,
                   time_t  const deadline,
                   bool *  const errorP) {

    char * const bufferStart = connectionP->buffer.t + connectionP->bufferpos;

    bool gotNonEmptyLine;
    bool error;
    char * lineStart;
    
    lineStart       = bufferStart;  /* initial value */
    gotNonEmptyLine = FALSE;        /* initial value */
    error           = FALSE;        /* initial value */          

    while (!gotNonEmptyLine && !error) {
        char * lineEnd;

        getLineInBuffer(connectionP, lineStart, deadline, &lineEnd, &error);

        if (!error) {
            if (!isEmptyLine(lineStart))
                gotNonEmptyLine = TRUE;
            else
                lineStart = lineEnd;
        }
    }
    if (!error) {
        /* Consume all the empty lines; advance buffer pointer to first
           non-empty line.
        */
        connectionP->bufferpos = lineStart - connectionP->buffer.t;
    }
    *errorP = error;
}



static void
readRequestField(TSession * const sessionP,
                 time_t     const deadline,
                 char **    const requestLineP,
                 uint16_t * const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   Read the HTTP request header field from session 'sessionP'.  We read
   through the session's internal buffer; i.e.  we may get data that was
   previously read from the network, or we may read more from the network.

   We assume the connection is presently positioned to the beginning of
   the HTTP document.  We leave it positioned after the request field.
   
   We ignore any empty lines at the beginning of the stream, per
   RFC2616 Section 4.1.

   Fail if we can't get the field before 'deadline'.

   Return as *requestLineP the request field read.  This ASCIIZ string is
   in the session's internal buffer.

   Return as *httpErrorCodeP the HTTP error code that describes how we
   are not able to read the request field, or 0 if we can.
   If we can't, *requestLineP is meaningless.
-----------------------------------------------------------------------------*/
    char * line;
    bool error;
    bool endOfHeader;

    skipToNonemptyLine(sessionP->connP, deadline, &error);

    if (!error) {
        readField(sessionP->connP, deadline, &endOfHeader, &line, &error);

        /* End of header is delimited by an empty line, and we skipped all
           the empty lines above, so readField() could not have encountered
           EOH:
        */
        assert(!endOfHeader);
    }
    if (error)
        *httpErrorCodeP = 408;  /* Request Timeout */
    else {
        *httpErrorCodeP = 0;
        *requestLineP = line;
    }
}



static void
hexDigitValue(char           const digit,
              unsigned int * const valueP,
              const char **  const errorP) {

    if (digit == '\0')
        xmlrpc_asprintf(errorP, "string ends in the middle of a "
                        "%% escape sequence");
    else {
        char const digitLc = tolower(digit);

        if ((digitLc >= '0') && (digitLc <= '9')) {
            *valueP = digitLc - '0';
            *errorP = NULL;
        } else if ((digitLc >= 'a') && (digitLc <= 'f')) {
            *valueP = 10 + digitLc - 'a';
            *errorP = NULL;
        } else
            xmlrpc_asprintf(errorP, "Non-hexadecimal digit '%c' in "
                            "%%HH escape sequence", digit);
    }
}



static void
parsePerCentEscape(const char ** const srcP,
                   char *        const unescapedP,
                   const char ** const errorP) {
/*-----------------------------------------------------------------------------
   With *srcP pointing to a supposed %HH escape sequence in a buffer, set
   *unescapedP to the character that the sequence represents and advance *srcP
   past it.
-----------------------------------------------------------------------------*/
    unsigned int digit0;

    const char * src;

    src = *srcP;  /* initial value */

    ++src;  /* Move past per cent sign */

    if (!*src)
        xmlrpc_asprintf(errorP, "URI ends after the %%");
    else {
        *errorP = NULL;  /* initial assumption */

        hexDigitValue(*src++, &digit0, errorP);
        
        if (!*errorP) {
            unsigned int digit1;
            
            if (!*src)
                xmlrpc_asprintf(errorP, "URI ends after the first digit");
            else {
                hexDigitValue(*src++, &digit1, errorP);
                
                if (!*errorP)
                    *unescapedP = ((digit0 << 4) | digit1);
            }
        }
    }
    *srcP = src;
}



static void
unescapeUri(const char *  const uriComponent,
            const char ** const unescapedP,
            const char ** const errorP) {
/*----------------------------------------------------------------------------
   Unescape a component of a URI, e.g. the host name.  That component may
   have %HH encoding, especially of characters that are delimiters within
   a URI like slash and colon.

   Return the unescaped version as *unescapedP in newly malloced storage.
-----------------------------------------------------------------------------*/
    char * buffer;

    buffer = strdup(uriComponent);

    if (!buffer)
        xmlrpc_asprintf(errorP, "Couldn't get memory for URI unescape buffer");
    else {
        const char * src;
        char * dst;

        src = dst = buffer;
    
        *errorP = NULL;  /* initial value */

        while (*src && !*errorP) {
            switch (*src) {
            case '%': {
                char unescaped;
                const char * error;

                parsePerCentEscape(&src, &unescaped, &error);

                if (error) {
                    xmlrpc_asprintf(errorP,
                                    "Invalid %%HH escape sequence.  %s",
                                    error);
                    xmlrpc_strfree(error);
                } else
                    *dst++ = unescaped;
            } break;

            default:
                *dst++ = *src++;
                break;
            }
        }
        *dst = '\0';

        if (*errorP)
            xmlrpc_strfree(buffer);
        else
            *unescapedP = buffer;
    }
}



static void
parseHostPort(const char *     const hostport,
              const char **    const hostP,
              unsigned short * const portP,
              const char **    const errorP) {
/*----------------------------------------------------------------------------
   Parse a 'hostport', a string in the form www.acme.com:8080 .

   Return the host name part (www.acme.com) as *hostP (in newly
   malloced storage), and the port part (8080) as *portP.

   Default the port to 80 if 'hostport' doesn't have the port part.
-----------------------------------------------------------------------------*/
    char * buffer;

    buffer = strdup(hostport);

    if (!buffer)
        xmlrpc_asprintf(errorP, "Couldn't get memory for host/port buffer");
    else {
        /* Note that the host portion may contain colons.  The old RFC says
           it can't, but a newer one says the host may be an IPv6 address
           in the form [x:x:x...].  But the port portion may contain only
           digits, so we use the _last_ colon as the delimiter.
        */
        char * const colonPos = strrchr(buffer, ':');

        if (colonPos) {
            const char * p;
            uint32_t port;

            *colonPos = '\0';  /* Split hostport at the colon */

            for (p = colonPos + 1, port = 0;
                 isdigit(*p) && port < 65535;
                 (port = port * 10 + (*p - '0')), ++p);
            
            if (*p || port == 0) {
                xmlrpc_asprintf(errorP, "There is nothing, or something "
                                "non-numeric for the port number after the "
                                "colon in '%s'", hostport);
            } else {
                *hostP = xmlrpc_strdupsol(buffer);
                *portP = port;
                *errorP = NULL;
            }
        } else {
            *hostP  = xmlrpc_strdupsol(buffer);
            *portP  = 80;
            *errorP = NULL;
        }
        free(buffer);
    }
}



static void
splitUriQuery(const char *  const requestUri,
              const char ** const queryP,
              const char ** const noQueryP,
              const char ** const errorP) {
/*----------------------------------------------------------------------------
   Split 'requestUri' at the question mark, returning the stuff after
   as *queryP and the stuff before as *noQueryP.
-----------------------------------------------------------------------------*/
    char * buffer;

    buffer = strdup(requestUri);

    if (!buffer)
        xmlrpc_asprintf(errorP, "Couldn't get memory for URI buffer");
    else {
        char * const qmark = strchr(buffer, '?');
            
        if (qmark) {
            *qmark = '\0';
            *queryP = xmlrpc_strdupsol(qmark + 1);
        } else
            *queryP = NULL;

        *errorP = NULL;
        *noQueryP = buffer;
    }
}



static void
parseHttpHostPortPath(const char *    const hostportpath,
                      const char **   const hostP,
                      unsigned short* const portP,
                      const char **   const pathP,
                      const char **   const errorP) {

    const char * path;

    char * buffer;

    buffer = strdup(hostportpath);

    if (!buffer)
        xmlrpc_asprintf(errorP,
                        "Couldn't get memory for host/port/path buffer");
    else {
        char * const slashPos = strchr(buffer, '/');

        char * hostport;
                
        if (slashPos) {
            path = xmlrpc_strdupsol(slashPos); /* Includes the initial slash */

            *slashPos = '\0';  /* NUL termination for hostport */
        } else
            path = strdup("*");

        hostport = buffer;

        /* The following interprets the port field without taking into account
           any %HH encoding, as the RFC says may be there.  We ignore that
           remote possibility out of laziness.
        */
        parseHostPort(hostport, hostP, portP, errorP);

        if (*errorP)
            xmlrpc_strfree(path);
        else
            *pathP = path;

        free(buffer);
    }
}



static void
unescapeHostPathQuery(const char *  const host,
                      const char *  const path,
                      const char *  const query,
                      const char ** const hostP,
                      const char ** const pathP,
                      const char ** const queryP,
                      const char ** const errorP) {
/*----------------------------------------------------------------------------
   Unescape each of the four components of a URI.

   Each may be NULL, in which case we return NULL.
-----------------------------------------------------------------------------*/
    if (host)
        unescapeUri(host, hostP, errorP);
    else
        *hostP = NULL;
    if (!*errorP) {
        if (path)
            unescapeUri(path, pathP, errorP);
        else
            *pathP = NULL;
        if (!*errorP) {
            if (query)
                unescapeUri(query, queryP, errorP);
            else
                *queryP = NULL;
            if (*errorP)
                xmlrpc_strfree(*pathP);
        } else {
            if (*hostP)
                xmlrpc_strfree(*hostP);
        }
    }
}



static void
parseRequestUri(char *           const requestUri,
                const char **    const hostP,
                unsigned short * const portP,
                const char **    const pathP,
                const char **    const queryP,
                const char **    const errorP) {
/*----------------------------------------------------------------------------
  Parse the request URI (in the request line
  "GET http://www.myserver.com:8080/myfile.cgi?parm HTTP/1.1",
  "http://www.myserver.com:8080/myfile.cgi?parm" is the request URI).

  Return as *hostP the "www.myserver.com" in the above example.  If
  that part of the URI doesn't exist, return *hostP == NULL.

  Return as *portP the 8080 in the above example.  If it doesn't exist,
  return 80.

  Return as *pathP the "/myfile.cgi" in the above example.  If it
  doesn't exist, return "*".

  Return as *queryP the "parm" in the above example.  If it doesn't
  exist, return *queryP == NULL.

  Return strings in newly malloc'ed storage.

  We can return syntactically invalid entities, e.g. a host name that
  contains "<", if 'requestUri' is similarly invalid.  We should fix that
  some day.  RFC 2396 lists a lot of characters as reserved for certain
  use in the URI, such as colon, and totally disallowed, such as space.
-----------------------------------------------------------------------------*/
    const char * requestUriNoQuery;
        /* The request URI with any query (the stuff marked by a question
           mark at the end of a request URI) chopped off.
        */
    const char * query;
    const char * path;
    const char * host;
    unsigned short port;

    splitUriQuery(requestUri, &query, &requestUriNoQuery, errorP);
    if (!*errorP) {
        if (requestUriNoQuery[0] == '/') {
            host = NULL;
            path = xmlrpc_strdupsol(requestUriNoQuery);
            port = 80;
            *errorP = NULL;
        } else {
            if (!xmlrpc_strneq(requestUriNoQuery, "http://", 7))
                xmlrpc_asprintf(errorP, "Scheme is not http://");
            else
                parseHttpHostPortPath(&requestUriNoQuery[7],
                                      &host, &port, &path, errorP);
        }

        if (!*errorP) {
            *portP = port;
            unescapeHostPathQuery(host, path, query,
                                  hostP, pathP, queryP, errorP);

            if (host)
                xmlrpc_strfree(host);
            if (path)
                xmlrpc_strfree(path);
        }

        if (query)
            xmlrpc_strfree(query);
        xmlrpc_strfree(requestUriNoQuery);
    }
}



static void
parseRequestLine(char *           const requestLine,
                 TMethod *        const httpMethodP,
                 httpVersion *    const httpVersionP,
                 const char **    const hostP,
                 unsigned short * const portP,
                 const char **    const pathP,
                 const char **    const queryP,
                 bool *           const moreLinesP,
                 uint16_t *       const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   Modifies *requestLine!
-----------------------------------------------------------------------------*/
    const char * httpMethodName;
    char * p;

    p = requestLine;

    /* Jump over spaces */
    NextToken((const char **)&p);

    httpMethodName = GetToken(&p);
    if (!httpMethodName)
        *httpErrorCodeP = 400;  /* Bad Request */
    else {
        char * requestUri;

        if (xmlrpc_streq(httpMethodName, "GET"))
            *httpMethodP = m_get;
        else if (xmlrpc_streq(httpMethodName, "PUT"))
            *httpMethodP = m_put;
        else if (xmlrpc_streq(httpMethodName, "OPTIONS"))
            *httpMethodP = m_options;
        else if (xmlrpc_streq(httpMethodName, "DELETE"))
            *httpMethodP = m_delete;
        else if (xmlrpc_streq(httpMethodName, "POST"))
            *httpMethodP = m_post;
        else if (xmlrpc_streq(httpMethodName, "TRACE"))
            *httpMethodP = m_trace;
        else if (xmlrpc_streq(httpMethodName, "HEAD"))
            *httpMethodP = m_head;
        else
            *httpMethodP = m_unknown;
        
        /* URI and Query Decoding */
        NextToken((const char **)&p);

        requestUri = GetToken(&p);
        if (!requestUri)
            *httpErrorCodeP = 400;  /* Bad Request */
        else {
            const char * host;
            unsigned short port;
            const char * path;
            const char * query;
            const char * error;

            parseRequestUri(requestUri, &host, &port, &path, &query, &error);

            if (error) {
                *httpErrorCodeP = 400;  /* Bad Request */
                xmlrpc_strfree(error);
                    /* Someday we should do something with this */
            } else {
                const char * httpVersion;

                NextToken((const char **)&p);
        
                /* HTTP Version Decoding */
                
                httpVersion = GetToken(&p);
                if (httpVersion) {
                    uint32_t vmin, vmaj;
                    if (sscanf(httpVersion, "HTTP/%d.%d", &vmaj, &vmin) != 2)
                        *httpErrorCodeP = 400;  /* Bad Request */
                    else {
                        httpVersionP->major = vmaj;
                        httpVersionP->minor = vmin;
                        *httpErrorCodeP = 0;  /* no error */
                    }
                    *moreLinesP = TRUE;
                } else {
                    /* There is no HTTP version, so this is a single
                       line request.
                    */
                    *httpErrorCodeP = 0;  /* no error */
                    *moreLinesP = FALSE;
                }
                if (*httpErrorCodeP) {
                    xmlrpc_strfree(host);
                    xmlrpc_strfree(path);
                    xmlrpc_strfree(query);
                }
                *hostP = host;
                *portP = port;
                *pathP = path;
                *queryP = query;
            }
        }
    }
}



static void
strtolower(char * const s) {

    char * t;

    t = &s[0];
    while (*t) {
        *t = tolower(*t);
        ++t;
    }
}



static void
getFieldNameToken(char **       const pP,
                  char **       const fieldNameP,
                  const char ** const errorP,
                  uint16_t *    const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   Assuming that *pP points to the place in an HTTP header where the field
   name belongs, return the field name and advance *pP past that token.

   The field name is the lower case representation of the value of the
   field name token.
-----------------------------------------------------------------------------*/
    char * fieldName;

    NextToken((const char **)pP);
    
    fieldName = GetToken(pP);
    if (!fieldName) {
        xmlrpc_asprintf(errorP, "The header has no field name token");
        *httpErrorCodeP = 400;  /* Bad Request */
    } else {
        if (fieldName[strlen(fieldName)-1] != ':') {
            /* Not a valid field name */
            xmlrpc_asprintf(errorP, "The field name token '%s' "
                            "does not end with a colon (:)", fieldName);
            *httpErrorCodeP = 400;  /* Bad Request */
        } else {
            fieldName[strlen(fieldName)-1] = '\0';  /* remove trailing colon */

            strtolower(fieldName);
            
            *errorP = NULL;
        }
    }
    *fieldNameP = fieldName;
}



static void
processField(const char *  const fieldName,
             char *        const fieldValue,
             TSession *    const sessionP,
             const char ** const errorP,
             uint16_t *    const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   We may modify *fieldValue, and we put pointers to *fieldValue and
   *fieldName into *sessionP.

   We must fix this some day.  *sessionP should point to individual
   malloc'ed strings.
-----------------------------------------------------------------------------*/
    *errorP = NULL;  /* initial assumption */

    if (xmlrpc_streq(fieldName, "connection")) {
        if (xmlrpc_strcaseeq(fieldValue, "keep-alive"))
            sessionP->requestInfo.keepalive = TRUE;
        else
            sessionP->requestInfo.keepalive = FALSE;
    } else if (xmlrpc_streq(fieldName, "host")) {
        if (sessionP->requestInfo.host) {
            xmlrpc_strfree(sessionP->requestInfo.host);
            sessionP->requestInfo.host = NULL;
        }
        parseHostPort(fieldValue, &sessionP->requestInfo.host,
                      &sessionP->requestInfo.port, errorP);
    } else if (xmlrpc_streq(fieldName, "from"))
        sessionP->requestInfo.from = fieldValue;
    else if (xmlrpc_streq(fieldName, "user-agent"))
        sessionP->requestInfo.useragent = fieldValue;
    else if (xmlrpc_streq(fieldName, "referer"))
        sessionP->requestInfo.referer = fieldValue;
    else if (xmlrpc_streq(fieldName, "range")) {
        if (xmlrpc_strneq(fieldValue, "bytes=", 6)) {
            bool succeeded;
            succeeded = ListAddFromString(&sessionP->ranges, &fieldValue[6]);
            if (!succeeded) {
                xmlrpc_asprintf(errorP, "ListAddFromString() failed for "
                                "\"range: bytes=...\" header value '%s'",
                                &fieldValue[6]);
                *httpErrorCodeP = 400;
            }
        }
    } else if (xmlrpc_streq(fieldName, "cookies")) {
        bool succeeded;
        succeeded = ListAddFromString(&sessionP->cookies, fieldValue);
        if (!succeeded) {
            xmlrpc_asprintf(errorP, "ListAddFromString() failed for "
                            "cookies: header value '%s'", fieldValue);
            *httpErrorCodeP = 400;
        }
    } else if (xmlrpc_streq(fieldName, "expect")) {
        if (xmlrpc_strcaseeq(fieldValue, "100-continue"))
            sessionP->continueRequired = TRUE;
    }
}



static void
readAndProcessHeaderFields(TSession *    const sessionP,
                           time_t        const deadline,
                           const char ** const errorP,
                           uint16_t *    const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   Read all the HTTP header fields from the session *sessionP, which has at
   least one field coming.  Update *sessionP to reflect the information in the
   fields.

   If we find an error in the fields or while trying to read them, we return
   a text explanation of the problem as *errorP and an appropriate HTTP error
   code as *httpErrorCodeP.  Otherwise, we return *errorP = NULL and nothing
   as *httpErrorCodeP.
-----------------------------------------------------------------------------*/
    bool endOfHeader;

    assert(!sessionP->validRequest);
        /* Calling us doesn't make sense if there is already a valid request */

    *errorP = NULL;  /* initial assumption */
    endOfHeader = false;  /* Caller assures us there is at least one header */

    while (!endOfHeader && !*errorP) {
        char * field;
        bool error;
        readField(sessionP->connP, deadline, &endOfHeader, &field, &error);
        if (error) {
            xmlrpc_asprintf(errorP, "Failed to read header from "
                            "client connection.");
            *httpErrorCodeP = 408;  /* Request Timeout */
        } else {
            if (!endOfHeader) {
                char * p;
                char * fieldName;

                p = &field[0];
                getFieldNameToken(&p, &fieldName, errorP, httpErrorCodeP);
                if (!*errorP) {
                    char * fieldValue;
                    
                    NextToken((const char **)&p);
                    
                    fieldValue = p;

                    TableAdd(&sessionP->requestHeaderFields,
                             fieldName, fieldValue);
                    
                    processField(fieldName, fieldValue, sessionP, errorP,
                                 httpErrorCodeP);
                }
            }
        }
    }
}



void
RequestRead(TSession *    const sessionP,
            uint32_t      const timeout,
            const char ** const errorP,
            uint16_t *    const httpErrorCodeP) {
/*----------------------------------------------------------------------------
   Read the headers of a new HTTP request (assuming nothing has yet been
   read on the session).

   Update *sessionP with the information from the headers.

   Leave the connection positioned to the body of the request, ready
   to be read by an HTTP request handler (via SessionRefillBuffer() and
   SessionGetReadData()).
-----------------------------------------------------------------------------*/
    time_t const deadline = time(NULL) + timeout;

    uint16_t httpErrorCode;  /* zero for no error */
    char * requestLine;  /* In connection;s internal buffer */

    readRequestField(sessionP, deadline, &requestLine, &httpErrorCode);
    if (httpErrorCode) {
        xmlrpc_asprintf(errorP, "Problem getting the request header");
        *httpErrorCodeP = httpErrorCode;
    } else {
        TMethod httpMethod;
        const char * host;
        const char * path;
        const char * query;
        unsigned short port;
        bool moreFields;

        parseRequestLine(requestLine, &httpMethod, &sessionP->version,
                         &host, &port, &path, &query,
                         &moreFields, &httpErrorCode);

        if (httpErrorCode) {
            xmlrpc_asprintf(errorP, "Unable to parse the request header "
                            "'%s'", requestLine);
            *httpErrorCodeP = httpErrorCode;
        } else {
            initRequestInfo(&sessionP->requestInfo, sessionP->version,
                            requestLine,
                            httpMethod, host, port, path, query);

            if (moreFields) {
                readAndProcessHeaderFields(sessionP, deadline,
                                           errorP, httpErrorCodeP);
            } else
                *errorP = NULL;

            if (!*errorP)
                sessionP->validRequest = true;

            xmlrpc_strfreenull(host);
            xmlrpc_strfree(path);
            xmlrpc_strfreenull(query);
        }
    }
}



char *
RequestHeaderValue(TSession *   const sessionP,
                   const char * const name) {

    return (TableFind(&sessionP->requestHeaderFields, name));
}



bool
RequestValidURI(TSession * const sessionP) {

    if (!sessionP->requestInfo.uri)
        return FALSE;
    
    if (xmlrpc_streq(sessionP->requestInfo.uri, "*"))
        return (sessionP->requestInfo.method != m_options);

    if (strchr(sessionP->requestInfo.uri, '*'))
        return FALSE;

    return TRUE;
}



bool
RequestValidURIPath(TSession * const sessionP) {

    uint32_t i;
    const char * p;

    p = sessionP->requestInfo.uri;

    i = 0;

    if (*p == '/') {
        i = 1;
        while (*p)
            if (*(p++) == '/') {
                if (*p == '/')
                    break;
                else if ((xmlrpc_strneq(p,"./", 2)) || (xmlrpc_streq(p, ".")))
                    ++p;
                else if ((xmlrpc_strneq(p, "../", 2)) ||
                         (xmlrpc_streq(p, ".."))) {
                    p += 2;
                    --i;
                    if (i == 0)
                        break;
                }
                /* Prevent accessing hidden files (starting with .) */
                else if (*p == '.')
                    return FALSE;
                else
                    if (*p)
                        ++i;
            }
    }
    return (*p == 0 && i > 0);
}



bool
RequestAuth(TSession *   const sessionP,
            const char * const credential,
            const char * const user,
            const char * const pass) {
/*----------------------------------------------------------------------------
   Authenticate requester, in a very simplistic fashion.

   If the request executing on session *sessionP specifies basic
   authentication (via Authorization header) with username 'user', password
   'pass', then return TRUE.  Else, return FALSE and set up an authorization
   failure response (HTTP response status 401) that says user must supply an
   identity in the 'credential' domain.

   When we return TRUE, we also set the username in the request info for the
   session to 'user' so that a future SessionGetRequestInfo can get it.
-----------------------------------------------------------------------------*/
    bool authorized;
    char * authHdrPtr;

    authHdrPtr = RequestHeaderValue(sessionP, "authorization");
    if (authHdrPtr) {
        const char * authType;
        NextToken((const char **)&authHdrPtr);
        GetTokenConst(&authHdrPtr, &authType);
        authType = GetToken(&authHdrPtr);
        if (authType) {
            if (xmlrpc_strcaseeq(authType, "basic")) {
                const char * userPass;
                char userPassEncoded[80];

                NextToken((const char **)&authHdrPtr);

                xmlrpc_asprintf(&userPass, "%s:%s", user, pass);
                xmlrpc_base64Encode(userPass, userPassEncoded);
                xmlrpc_strfree(userPass);

                if (xmlrpc_streq(authHdrPtr, userPassEncoded)) {
                    sessionP->requestInfo.user = xmlrpc_strdupsol(user);
                    authorized = TRUE;
                } else
                    authorized = FALSE;
            } else
                authorized = FALSE;
        } else
            authorized = FALSE;
    } else
        authorized = FALSE;

    if (!authorized) {
        const char * hdrValue;
        xmlrpc_asprintf(&hdrValue, "Basic realm=\"%s\"", credential);
        ResponseAddField(sessionP, "WWW-Authenticate", hdrValue);

        xmlrpc_strfree(hdrValue);

        ResponseStatus(sessionP, 401);
    }
    return authorized;
}



/*********************************************************************
** Range
*********************************************************************/

abyss_bool
RangeDecode(char *            const strArg,
            xmlrpc_uint64_t   const filesize,
            xmlrpc_uint64_t * const start,
            xmlrpc_uint64_t * const end) {

    char *str;
    char *ss;

    str = strArg;  /* initial value */

    *start=0;
    *end=filesize-1;

    if (*str=='-')
    {
        *start=filesize-strtol(str+1,&ss,10);
        return ((ss!=str) && (!*ss));
    };

    *start=strtol(str,&ss,10);

    if ((ss==str) || (*ss!='-'))
        return FALSE;

    str=ss+1;

    if (!*str)
        return TRUE;

    *end=strtol(str,&ss,10);

    if ((ss==str) || (*ss) || (*end<*start))
        return FALSE;

    return TRUE;
}

/*********************************************************************
** HTTP
*********************************************************************/

const char *
HTTPReasonByStatus(uint16_t const code) {

    struct _HTTPReasons {
        uint16_t status;
        const char * reason;
    };

    static struct _HTTPReasons const reasons[] =  {
        { 100,"Continue" }, 
        { 101,"Switching Protocols" }, 
        { 200,"OK" }, 
        { 201,"Created" }, 
        { 202,"Accepted" }, 
        { 203,"Non-Authoritative Information" }, 
        { 204,"No Content" }, 
        { 205,"Reset Content" }, 
        { 206,"Partial Content" }, 
        { 300,"Multiple Choices" }, 
        { 301,"Moved Permanently" }, 
        { 302,"Moved Temporarily" }, 
        { 303,"See Other" }, 
        { 304,"Not Modified" }, 
        { 305,"Use Proxy" }, 
        { 400,"Bad Request" }, 
        { 401,"Unauthorized" }, 
        { 402,"Payment Required" }, 
        { 403,"Forbidden" }, 
        { 404,"Not Found" }, 
        { 405,"Method Not Allowed" }, 
        { 406,"Not Acceptable" }, 
        { 407,"Proxy Authentication Required" }, 
        { 408,"Request Timeout" }, 
        { 409,"Conflict" }, 
        { 410,"Gone" }, 
        { 411,"Length Required" }, 
        { 412,"Precondition Failed" }, 
        { 413,"Request Entity Too Large" }, 
        { 414,"Request-URI Too Long" }, 
        { 415,"Unsupported Media Type" }, 
        { 500,"Internal Server Error" }, 
        { 501,"Not Implemented" }, 
        { 502,"Bad Gateway" }, 
        { 503,"Service Unavailable" }, 
        { 504,"Gateway Timeout" }, 
        { 505,"HTTP Version Not Supported" },
        { 000, NULL }
    };
    const struct _HTTPReasons * reasonP;

    reasonP = &reasons[0];

    while (reasonP->status <= code)
        if (reasonP->status == code)
            return reasonP->reason;
        else
            ++reasonP;

    return "No Reason";
}



int32_t
HTTPRead(TSession *   const s ATTR_UNUSED,
         const char * const buffer ATTR_UNUSED,
         uint32_t     const len ATTR_UNUSED) {

    return 0;
}



bool
HTTPWriteBodyChunk(TSession *   const sessionP,
                   const char * const buffer,
                   uint32_t     const len) {

    bool succeeded;

    if (sessionP->chunkedwrite && sessionP->chunkedwritemode) {
        char chunkHeader[16];

        sprintf(chunkHeader, "%x\r\n", len);

        succeeded =
            ConnWrite(sessionP->connP, chunkHeader, strlen(chunkHeader));
        if (succeeded) {
            succeeded = ConnWrite(sessionP->connP, buffer, len);
            if (succeeded)
                succeeded = ConnWrite(sessionP->connP, "\r\n", 2);
        }
    } else
        succeeded = ConnWrite(sessionP->connP, buffer, len);

    return succeeded;
}



bool
HTTPWriteEndChunk(TSession * const sessionP) {

    bool retval;

    if (sessionP->chunkedwritemode && sessionP->chunkedwrite) {
        /* May be one day trailer dumping will be added */
        sessionP->chunkedwritemode = FALSE;
        retval = ConnWrite(sessionP->connP, "0\r\n\r\n", 5);
    } else
        retval = TRUE;

    return retval;
}



bool
HTTPKeepalive(TSession * const sessionP) {
/*----------------------------------------------------------------------------
   Return value: the connection should be kept alive after the session
   *sessionP is over.
-----------------------------------------------------------------------------*/
    return (sessionP->requestInfo.keepalive &&
            !sessionP->serverDeniesKeepalive &&
            sessionP->status < 400);
}



bool
HTTPWriteContinue(TSession * const sessionP) {

    char const continueStatus[] = "HTTP/1.1 100 continue\r\n\r\n";
        /* This is a status line plus an end-of-headers empty line */

    return ConnWrite(sessionP->connP, continueStatus, strlen(continueStatus));
}



/******************************************************************************
**
** http.c
**
** Copyright (C) 2000 by Moez Mahfoudh <mmoez@bigfoot.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
******************************************************************************/
