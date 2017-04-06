#include "xmlrpc_config.h"

#define _XOPEN_SOURCE 600  /* For strdup(), sigaction */
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/abyss.h */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#ifdef _WIN32
#  include <io.h>
#else
#  include <signal.h>
#  include <sys/wait.h>
#  include <grp.h>
#endif

#include "bool.h"
#include "int.h"
#include "mallocvar.h"
#include "xmlrpc-c/abyss.h"

#include "xmlrpc-c/base.h"
#include "xmlrpc-c/server.h"
#include "xmlrpc-c/base_int.h"
#include "xmlrpc-c/string_int.h"

#include "abyss_handler.h"



static const char * trace_abyss;


void
xmlrpc_abyss_handler_trace(const char * const trace) {

    trace_abyss = trace;
}


static void
addAuthCookie(xmlrpc_env * const envP,
              TSession *   const abyssSessionP,
              const char * const authCookie) {

    const char * cookieResponse;
    
    xmlrpc_asprintf(&cookieResponse, "auth=%s", authCookie);
    
    if (xmlrpc_strnomem(cookieResponse))
        xmlrpc_faultf(envP, "Insufficient memory to generate cookie "
                      "response header.");
    else {
        ResponseAddField(abyssSessionP, "Set-Cookie", cookieResponse);
    
        xmlrpc_strfree(cookieResponse);
    }
}   
    


static void 
sendResponse(xmlrpc_env *      const envP,
             TSession *        const abyssSessionP, 
             const char *      const body, 
             size_t            const len,
             bool              const chunked,
             ResponseAccessCtl const accessControl) {
/*----------------------------------------------------------------------------
   Generate an HTTP response containing body 'body' of length 'len'
   characters.

   This is meant to run in the context of an Abyss URI handler for
   Abyss session 'abyssSessionP'.
-----------------------------------------------------------------------------*/
    const char * http_cookie = NULL;
        /* This used to set http_cookie to getenv("HTTP_COOKIE"), but
           that doesn't make any sense -- environment variables are not
           appropriate for this.  So for now, cookie code is disabled.
           - Bryan 2004.10.03.
        */

    /* Various bugs before Xmlrpc-c 1.05 caused the response to be not
       chunked in the most basic case, but chunked if the client explicitly
       requested keepalive.  I think it's better not to chunk, because
       it's simpler, so I removed this in 1.05.  I don't know what the
       purpose of chunking would be, and an original comment suggests
       the author wasn't sure chunking was a good idea.

       In 1.06 we added the user option to chunk.
    */
    if (chunked)
        ResponseChunked(abyssSessionP);

    ResponseStatus(abyssSessionP, 200);

    if (http_cookie)
        /* There's an auth cookie, so pass it back in the response. */
        addAuthCookie(envP, abyssSessionP, http_cookie);

    if ((size_t)(uint32_t)len != len)
        xmlrpc_faultf(envP, "XML-RPC method generated a response too "
                      "large for Abyss to send");
    else {
        uint32_t const abyssLen = (uint32_t)len;

        /* See discussion below of quotes around "utf-8" */
        ResponseContentType(abyssSessionP, "text/xml; charset=utf-8");
        ResponseContentLength(abyssSessionP, abyssLen);
        ResponseAccessControl(abyssSessionP, accessControl);
        
        ResponseWriteStart(abyssSessionP);
        ResponseWriteBody(abyssSessionP, body, abyssLen);
        ResponseWriteEnd(abyssSessionP);
    }
}



/* From 0.9.10 (May 2001) through 1.17 (December 2008), the content-type
   header said charset="utf-8" (i.e. with the value of 'charset' an HTTP quoted
   string).  Before 0.9.10, the header didn't have charset at all.

   We got a complaint in January 2009 that some client didn't understand that,
   saying

     apache2: XML-RPC: xmlrpcmsg::parseResponse: invalid charset encoding of
     received response: "UTF-8"

   And that removing the quotation marks fixes this.

   From what I can tell, the module is wrong to distinguish between the
   two, but I don't think it hurts anything to use a basic HTTP token instead
   of an HTTP quoted string here, so starting in 1.18, we do.  */


static void
sendError(TSession *   const abyssSessionP, 
          unsigned int const status,
          const char * const explanation) {
/*----------------------------------------------------------------------------
  Send an error response back to the client.
-----------------------------------------------------------------------------*/
    ResponseStatus(abyssSessionP, (uint16_t) status);
    ResponseError2(abyssSessionP, explanation);
}



static void
traceChunkRead(TSession * const abyssSessionP) {

    fprintf(stderr, "XML-RPC handler got a chunk of %u bytes\n",
            (unsigned int)SessionReadDataAvail(abyssSessionP));
}



static void
refillBufferFromConnection(xmlrpc_env * const envP,
                           TSession *   const abyssSessionP,
                           const char * const trace) {
/*----------------------------------------------------------------------------
   Get the next chunk of data from the connection into the buffer.
-----------------------------------------------------------------------------*/
    bool succeeded;

    succeeded = SessionRefillBuffer(abyssSessionP);

    if (!succeeded)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TIMEOUT_ERROR, "Timed out waiting for "
            "client to send its POST data");
    else {
        if (trace)
            traceChunkRead(abyssSessionP);
    }
}



static void
getBody(xmlrpc_env *        const envP,
        TSession *          const abyssSessionP,
        size_t              const contentSize,
        const char *        const trace,
        xmlrpc_mem_block ** const bodyP) {
/*----------------------------------------------------------------------------
   Get the entire body, which is of size 'contentSize' bytes, from the
   Abyss session and return it as the new memblock *bodyP.

   The first chunk of the body may already be in Abyss's buffer.  We
   retrieve that before reading more.
-----------------------------------------------------------------------------*/
    xmlrpc_mem_block * body;

    if (trace)
        fprintf(stderr, "XML-RPC handler processing body.  "
                "Content Size = %u bytes\n", (unsigned)contentSize);

    body = xmlrpc_mem_block_new(envP, 0);
    if (!envP->fault_occurred) {
        size_t bytesRead;
        const char * chunkPtr;
        size_t chunkLen;

        bytesRead = 0;

        while (!envP->fault_occurred && bytesRead < contentSize) {
            SessionGetReadData(abyssSessionP, contentSize - bytesRead, 
                               &chunkPtr, &chunkLen);
            bytesRead += chunkLen;

            assert(bytesRead <= contentSize);

            XMLRPC_MEMBLOCK_APPEND(char, envP, body, chunkPtr, chunkLen);
            if (bytesRead < contentSize)
                refillBufferFromConnection(envP, abyssSessionP, trace);
        }
        if (envP->fault_occurred)
            xmlrpc_mem_block_free(body);
    }
    *bodyP = body;
}



static void
storeCookies(TSession *     const httpRequestP,
             const char **  const errorP) {
/*----------------------------------------------------------------------------
   Get the cookie settings from the HTTP headers and remember them for
   use in responses.
-----------------------------------------------------------------------------*/
    const char * const cookie = RequestHeaderValue(httpRequestP, "cookie");
    if (cookie) {
        /* 
           Setting the value in an environment variable doesn't make
           any sense.  So for now, cookie code is disabled.
           -Bryan 04.10.03.

        setenv("HTTP_COOKIE", cookie, 1);
        */
    }
    /* TODO: parse HTTP_COOKIE to find auth pair, if there is one */

    *errorP = NULL;
}




static void
processContentLength(TSession *    const httpRequestP,
                     size_t *      const inputLenP,
                     bool *        const missingP,
                     const char ** const errorP) {
/*----------------------------------------------------------------------------
  Make sure the content length is present and non-zero.  This is
  technically required by XML-RPC, but we only enforce it because we
  don't want to figure out how to safely handle HTTP < 1.1 requests
  without it.
-----------------------------------------------------------------------------*/
    const char * const content_length = 
        RequestHeaderValue(httpRequestP, "content-length");

    if (content_length == NULL) {
        *missingP = TRUE;
        *errorP = NULL;
    } else {
        *missingP = FALSE;
        *inputLenP = 0;  /* quiet compiler warning */
        if (content_length[0] == '\0')
            xmlrpc_asprintf(errorP, "The value in your content-length "
                            "HTTP header value is a null string");
        else {
            unsigned long contentLengthValue;
            char * tail;
        
            contentLengthValue = strtoul(content_length, &tail, 10);
        
            if (*tail != '\0')
                xmlrpc_asprintf(errorP, "There's non-numeric crap in "
                                "the value of your content-length "
                                "HTTP header: '%s'", tail);
            else if (contentLengthValue < 1)
                xmlrpc_asprintf(errorP, "According to your content-length "
                                "HTTP header, your request is empty (zero "
                                "length)");
            else if ((unsigned long)(size_t)contentLengthValue 
                     != contentLengthValue)
                xmlrpc_asprintf(errorP, "According to your content-length "
                                "HTTP header, your request is too big to "
                                "process; we can't even do arithmetic on its "
                                "size: %s bytes", content_length);
            else {
                *errorP = NULL;
                *inputLenP = (size_t)contentLengthValue;
            }
        }
    }
}



static void
traceHandlerCalled(TSession * const abyssSessionP) {
    
    const char * methodDesc;
    const TRequestInfo * requestInfoP;

    fprintf(stderr, "xmlrpc_server_abyss URI path handler called.\n");

    SessionGetRequestInfo(abyssSessionP, &requestInfoP);

    fprintf(stderr, "URI = '%s'\n", requestInfoP->uri);

    switch (requestInfoP->method) {
    case m_unknown: methodDesc = "unknown";   break;
    case m_get:     methodDesc = "get";       break;
    case m_put:     methodDesc = "put";       break;
    case m_head:    methodDesc = "head";      break;
    case m_post:    methodDesc = "post";      break;
    case m_delete:  methodDesc = "delete";    break;
    case m_trace:   methodDesc = "trace";     break;
    case m_options: methodDesc = "m_options"; break;
    default:        methodDesc = "?";
    }
    fprintf(stderr, "HTTP method = '%s'\n", methodDesc);

    if (requestInfoP->query)
        fprintf(stderr, "query (component of URL)='%s'\n",
                requestInfoP->query);
    else
        fprintf(stderr, "URL has no query component\n");
}



static void
processCall(TSession *            const abyssSessionP,
            size_t                const contentSize,
            xmlrpc_call_processor       xmlProcessor,
            void *                const xmlProcessorArg,
            bool                  const wantChunk,
            ResponseAccessCtl     const accessControl,
            const char *          const trace) {
/*----------------------------------------------------------------------------
   Handle an RPC request.  This is an HTTP request that has the proper form
   to be an XML-RPC call.

   The text of the call is available through the Abyss session
   'abyssSessionP'.

   Its content length is 'contentSize' bytes.
-----------------------------------------------------------------------------*/
    xmlrpc_env env;

    if (trace)
        fprintf(stderr,
                "xmlrpc_server_abyss URI path handler processing RPC.\n");

    xmlrpc_env_init(&env);

    if (contentSize > xmlrpc_limit_get(XMLRPC_XML_SIZE_LIMIT_ID))
        xmlrpc_env_set_fault_formatted(
            &env, XMLRPC_LIMIT_EXCEEDED_ERROR,
            "XML-RPC request too large (%u bytes)", (unsigned)contentSize);
    else {
        xmlrpc_mem_block * body;
        /* Read XML data off the wire. */
        getBody(&env, abyssSessionP, contentSize, trace, &body);
        if (!env.fault_occurred) {
            xmlrpc_mem_block * output;

            /* Process the RPC. */
            xmlProcessor(
                &env, xmlProcessorArg,
                XMLRPC_MEMBLOCK_CONTENTS(char, body),
                XMLRPC_MEMBLOCK_SIZE(char, body),
                abyssSessionP,
                &output);
            if (!env.fault_occurred) {
                /* Send out the result. */
                sendResponse(&env, abyssSessionP, 
                             XMLRPC_MEMBLOCK_CONTENTS(char, output),
                             XMLRPC_MEMBLOCK_SIZE(char, output),
                             wantChunk, accessControl);
                
                XMLRPC_MEMBLOCK_FREE(char, output);
            }
            XMLRPC_MEMBLOCK_FREE(char, body);
        }
    }
    if (env.fault_occurred) {
        uint16_t httpResponseStatus;
        if (env.fault_code == XMLRPC_TIMEOUT_ERROR)
            httpResponseStatus = 408;  /* Request Timeout */
        else
            httpResponseStatus = 500;  /* Internal Server Error */

        sendError(abyssSessionP, httpResponseStatus, env.fault_string);
    }

    xmlrpc_env_clean(&env);
}



void
xmlrpc_initAccessCtl(ResponseAccessCtl * const accessCtlP,
                     const char *        const allowOrigin,
                     bool                const expires,
                     unsigned int        const maxAge) {
/*----------------------------------------------------------------------------
   Set up *accessCtlP to reflect the HTTP access control parameters
   'allowOrigin', 'expires', and 'maxAge'.

   Note that 'maxAge' is irrelevant when 'expires' is false.
-----------------------------------------------------------------------------*/
    accessCtlP->allowOrigin =
        allowOrigin ? xmlrpc_strdupsol(allowOrigin) : NULL;
    accessCtlP->expires     = expires;
    accessCtlP->maxAge      = maxAge;
}



void
xmlrpc_termAccessControl(ResponseAccessCtl * const accessCtlP) {

    xmlrpc_strfreenull(accessCtlP->allowOrigin);
}



void
xmlrpc_termUriHandler(void * const arg) {

    struct uriHandlerXmlrpc * const uriHandlerXmlrpcP = arg;

    xmlrpc_strfree(uriHandlerXmlrpcP->uriPath);
    xmlrpc_termAccessControl(&uriHandlerXmlrpcP->accessControl);
    free(uriHandlerXmlrpcP);
}



static void
handleXmlRpcCallReq(TSession *           const abyssSessionP,
                    const TRequestInfo * const requestInfoP ATTR_UNUSED,
                    xmlrpc_call_processor      xmlProcessor,
                    void *               const xmlProcessorArg,
                    bool                 const wantChunk,
                    ResponseAccessCtl    const accessControl) {
/*----------------------------------------------------------------------------
   Handle the HTTP request described by *requestInfoP, which arrived over
   Abyss HTTP session *abyssSessionP, which is an XML-RPC call
   (i.e. a POST request to /RPC2 or whatever other URI our server is
   supposed to handle).

   Handle it by feeding the XML which is its content to 'xmlProcessor'
   along with argument 'xmlProcessorArg'.
-----------------------------------------------------------------------------*/
    /* We used to reject the call if content-type was not present and
       text/xml, on some security theory (a firewall may block text/xml with
       the intent of blocking XML-RPC).  Now, we believe that is silly, and we
       have seen an incorrectly implemented client that says text/plain.
    */
    const char * error;

    assert(requestInfoP->method == m_post);

    storeCookies(abyssSessionP, &error);
    if (error) {
        sendError(abyssSessionP, 400, error);
        xmlrpc_strfree(error);
    } else {
        const char * error;
        bool missing;
        size_t contentSize;

        processContentLength(abyssSessionP, 
                             &contentSize, &missing, &error);
        if (error) {
            sendError(abyssSessionP, 400, error);
            xmlrpc_strfree(error);
        } else {
            if (missing)
                sendError(abyssSessionP, 411, "You must send a "
                          "content-length HTTP header in an "
                          "XML-RPC call.");
            else
                processCall(abyssSessionP, contentSize,
                            xmlProcessor, xmlProcessorArg,
                            wantChunk, accessControl,
                            trace_abyss);
        }
    }
}



static void
handleXmlRpcOptionsReq(TSession *        const abyssSessionP,
                       ResponseAccessCtl const accessControl) {

    ResponseAddField(abyssSessionP, "Allow", "POST");
    
    ResponseAccessControl(abyssSessionP, accessControl);
    ResponseContentLength(abyssSessionP, 0);
    ResponseStatus(abyssSessionP, 200);
    ResponseWriteStart(abyssSessionP);
    ResponseWriteEnd(abyssSessionP);
}



void
xmlrpc_handleIfXmlrpcReq(void *        const handlerArg,
                         TSession *    const abyssSessionP,
                         abyss_bool *  const handledP) {
/*----------------------------------------------------------------------------
   Our job is to look at this HTTP request that the Abyss server is
   trying to process and see if we can handle it.  If it's an XML-RPC
   call for this XML-RPC server, we handle it.  If it's not, we refuse
   it and Abyss can try some other handler.

   Our return code is TRUE to mean we handled it; FALSE to mean we didn't.

   Note that failing the request counts as handling it, and not handling
   it does not mean we failed it.

   This is an Abyss HTTP Request handler -- type handleReqFn3.
-----------------------------------------------------------------------------*/
    struct uriHandlerXmlrpc * const uriHandlerXmlrpcP = handlerArg;

    const TRequestInfo * requestInfoP;

    if (trace_abyss)
        traceHandlerCalled(abyssSessionP);

    SessionGetRequestInfo(abyssSessionP, &requestInfoP);

    /* Note that requestInfoP->uri is not the whole URI.  It is just
       the "file name" part of it.
    */
    if (!xmlrpc_streq(requestInfoP->uri, uriHandlerXmlrpcP->uriPath))
        /* It's not for the path (e.g. "/RPC2") that we're supposed to
           handle.
        */
        *handledP = FALSE;
    else {
        *handledP = TRUE;

        switch (requestInfoP->method) {
        case m_post:
            handleXmlRpcCallReq(abyssSessionP, requestInfoP,
                                uriHandlerXmlrpcP->xmlProcessor,
                                uriHandlerXmlrpcP->xmlProcessorArg,
                                uriHandlerXmlrpcP->chunkResponse,
                                uriHandlerXmlrpcP->accessControl);
            break;
        case m_options:
            handleXmlRpcOptionsReq(abyssSessionP,
                                   uriHandlerXmlrpcP->accessControl);
            break;
        default:
            sendError(abyssSessionP, 405,
                      "POST is the only HTTP method this server understands");
                /* 405 = Method Not Allowed */
        }
    }
    if (trace_abyss)
        fprintf(stderr, "xmlrpc_server_abyss URI path handler returning.\n");
}



unsigned int
xmlrpc_abyss_handler_stacksize() {
/*----------------------------------------------------------------------------
   The maximum number of bytes an HTTP request handler in this file might
   place on the stack.

   This doesn't include what the user's method function requires.
-----------------------------------------------------------------------------*/
    return 1024;
}



abyss_bool
xmlrpc_serverAbyssDefaultUriHandler(TSession * const sessionP) {
/*----------------------------------------------------------------------------
   This is an Abyss default handler.  It return a 404 Not Found for all
   requests.
-----------------------------------------------------------------------------*/
    const TRequestInfo * requestInfoP;

    const char * explanation;

    if (trace_abyss)
        fprintf(stderr, "xmlrpc_server_abyss default handler called.\n");

    SessionGetRequestInfo(sessionP, &requestInfoP);

    xmlrpc_asprintf(
        &explanation,
        "This XML-RPC For C/C++ Abyss XML-RPC server "
        "responds to only one URI path.  "
        "I don't know what URI path that is, "
        "but it's not the one you requested: '%s'.  (Typically, it's "
        "'/RPC2')", requestInfoP->uri);

    sendError(sessionP, 404, explanation);

    xmlrpc_strfree(explanation);

    return TRUE;
}



