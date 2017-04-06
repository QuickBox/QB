/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* Copyright (C) 2001 by First Peer, Inc. All rights reserved.
** Copyright (C) 2002 Ximian, Inc.
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
** SUCH DAMAGE. */

#include "xmlrpc_config.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
/* There was code here from 2006-2013 that included <xmlparser.h>
   instead of <libxml/parser.h> when compiling for Windows.  It was probably
   compiled rarely if ever (this file is an optional part of the build).
   In Feburary 2013, a Mingw user found <libxml/parser.h> was necessary,
   and that makes more sense, so we changed it.
*/
#include <libxml/parser.h>

#include "mallocvar.h"
#include "xmlrpc-c/base.h"
#include "xmlrpc-c/base_int.h"
#include "xmlrpc-c/string_int.h"
#include "xmlrpc-c/xmlparser.h"

struct _xml_element {
    xml_element * parentP;
    const char * name;
    xmlrpc_mem_block cdata;    /* char */
    xmlrpc_mem_block children; /* xml_element* */
};

#define XMLRPC_ASSERT_ELEM_OK(elem) \
    XMLRPC_ASSERT((elem) != NULL && (elem)->name != XMLRPC_BAD_POINTER)



void
xml_init(xmlrpc_env * const envP) {

    XMLRPC_ASSERT_ENV_OK(envP);

    /* N.B. xmlInitParser() does not stack.  Calling it twice is the
       same as calling it once.  Consequently, the same is true
       of xml_init().

       N.B. xmlInitParser() is necessary for form only, because every
       libxml2 subroutine that needs it to be called just calls it itself.
    */
    xmlInitParser();
}



void
xml_term(void) {

    /* N.B xmlCleanupParser() doesn't know how many times you called
       xmlInitParser().  Calling it twice is the same as calling it once.
       This means you must not call xml_term() while anything else in
       the process is still using libxml2.
    */
    xmlCleanupParser();
}



static xml_element *
xmlElementNew(xmlrpc_env * const envP,
              const char * const name) {
/*----------------------------------------------------------------------------
  Create a new xml_element. This routine isn't exported, because the
  arguments are implementation-dependent.
-----------------------------------------------------------------------------*/

    xml_element * retval;
    bool nameIsValid;
    bool cdataIsValid;
    bool childrenAreValid;

    XMLRPC_ASSERT_ENV_OK(envP);
    assert(name != NULL);

    /* Set up our error-handling preconditions. */
    retval = NULL;
    nameIsValid = cdataIsValid = childrenAreValid = false;

    MALLOCVAR(retval);
    XMLRPC_FAIL_IF_NULL(retval, envP, XMLRPC_INTERNAL_ERROR,
                        "Couldn't allocate memory for XML element");

    retval->parentP = NULL;
    
    /* Copy over the element name. */
    retval->name = strdup(name);
    XMLRPC_FAIL_IF_NULL(retval->name, envP, XMLRPC_INTERNAL_ERROR,
                        "Couldn't allocate memory for XML element");
    nameIsValid = true;

    /* Initialize a block to hold our CDATA. */
    XMLRPC_TYPED_MEM_BLOCK_INIT(char, envP, &retval->cdata, 0);
    XMLRPC_FAIL_IF_FAULT(envP);
    cdataIsValid = true;

    /* Initialize a block to hold our child elements. */
    XMLRPC_TYPED_MEM_BLOCK_INIT(xml_element *, envP, &retval->children, 0);
    XMLRPC_FAIL_IF_FAULT(envP);
    childrenAreValid = true;

cleanup:
    if (envP->fault_occurred) {
        if (retval) {
            if (nameIsValid)
                xmlrpc_strfree(retval->name);
            if (cdataIsValid)
                xmlrpc_mem_block_clean(&retval->cdata);
            if (childrenAreValid)
                xmlrpc_mem_block_clean(&retval->children);
            free(retval);
        }
        retval = NULL;
    }
    return retval;
}



void
xml_element_free(xml_element * const elemP) {
/*----------------------------------------------------------------------------
  Blow away an existing element & all of its child elements.
-----------------------------------------------------------------------------*/
    xmlrpc_mem_block * children;
    unsigned int size;
    unsigned int i;
    xml_element ** contents;

    XMLRPC_ASSERT_ELEM_OK(elemP);

    xmlrpc_strfree(elemP->name);
    elemP->name = XMLRPC_BAD_POINTER;
    xmlrpc_mem_block_clean(&elemP->cdata);

    /* Deallocate all of our children recursively. */
    children = &elemP->children;
    contents = XMLRPC_TYPED_MEM_BLOCK_CONTENTS(xml_element *, children);
    size = XMLRPC_TYPED_MEM_BLOCK_SIZE(xml_element *, children);
    for (i = 0; i < size; ++i)
        xml_element_free(contents[i]);

    xmlrpc_mem_block_clean(&elemP->children);

    free(elemP);
}



/*=========================================================================
**  Miscellaneous Accessors
**=========================================================================
**  Return the fields of the xml_element. See the header for more
**  documentation on each function works.
*/

const char *
xml_element_name(const xml_element * const elemP) {

    XMLRPC_ASSERT_ELEM_OK(elemP);
    return elemP->name;
}

size_t
xml_element_cdata_size(xml_element * const elemP) {
    /* The result of this function is NOT VALID until the end_element handler
       has been called!
    */
    XMLRPC_ASSERT_ELEM_OK(elemP);
    return XMLRPC_TYPED_MEM_BLOCK_SIZE(char, &elemP->cdata) - 1;
}



char *
xml_element_cdata(xml_element * const elemP) {
    XMLRPC_ASSERT_ELEM_OK(elemP);
    return XMLRPC_TYPED_MEM_BLOCK_CONTENTS(char, &elemP->cdata);
}



size_t
xml_element_children_size(const xml_element * const elemP) {
    XMLRPC_ASSERT_ELEM_OK(elemP);
    return XMLRPC_TYPED_MEM_BLOCK_SIZE(xml_element *, &elemP->children);
}



xml_element **
xml_element_children(const xml_element * const elemP) {
    XMLRPC_ASSERT_ELEM_OK(elemP);
    return XMLRPC_TYPED_MEM_BLOCK_CONTENTS(xml_element *, &elemP->children);
}



/*=========================================================================
**  Internal xml_element Utility Functions
**=========================================================================
*/

static void
xmlElementAppendCdata(xmlrpc_env *  const envP,
				      xml_element * const elemP,
				      const char *  const cdata,
				      size_t        const size) {

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_ELEM_OK(elemP);    

    XMLRPC_TYPED_MEM_BLOCK_APPEND(char, envP, &elemP->cdata, cdata, size);
}



static void
xmlElementAppendChild(xmlrpc_env *  const envP,
				      xml_element * const elemP,
				      xml_element * const childP) {

    /* Whether or not this function succeeds, it takes ownership of the 'child'
       argument.
       WARNING - This is the exact opposite of the usual memory ownership
       rules for xmlrpc_value! So please pay attention.
    */
    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_ELEM_OK(elemP);
    XMLRPC_ASSERT_ELEM_OK(childP);
    assert(childP->parentP == NULL);

    XMLRPC_TYPED_MEM_BLOCK_APPEND(xml_element *, envP, &elemP->children,
                                  &childP, 1);
    if (!envP->fault_occurred)
        childP->parentP = elemP;
    else
        xml_element_free(childP);
}



typedef struct {
/*----------------------------------------------------------------------------
   Our parse context. We pass this around as libxml user data.
-----------------------------------------------------------------------------*/

    xmlrpc_env env;
    xml_element * rootP;
    xml_element * currentP;
} ParseContext;



/*=========================================================================
**  LibXML Event Handler Functions
**=========================================================================
*/

static void
startElement_(void *           const userData,
              const xmlChar *  const name,
              const xmlChar ** const attrs ATTR_UNUSED) {

    ParseContext * contextP;
    xml_element  * elemP;
    xml_element  * newCurrentP;

    assert(userData != NULL && name != NULL);

    /* Get our context and see if an error has already occured. */
    contextP = (ParseContext*) userData;
    if (!contextP->env.fault_occurred) {
        /* Build a new element. */
        elemP = xmlElementNew(&contextP->env, (char *) name);
        XMLRPC_FAIL_IF_FAULT(&contextP->env);

        /* Insert it in the appropriate place. */
        if (!contextP->rootP) {
            contextP->rootP = elemP;
            contextP->currentP = elemP;
            elemP = NULL;
        } else {
            assert(contextP->currentP != NULL);

            /* (We need to watch our error handling invariants very carefully
            ** here. Read the docs for xml_elementAppendChild. */
            newCurrentP = elemP;
            xmlElementAppendChild(&contextP->env, contextP->currentP, elemP);
            elemP = NULL;
            XMLRPC_FAIL_IF_FAULT(&contextP->env);
            contextP->currentP = newCurrentP;
        }
        
    cleanup:
        if (elemP)
            xml_element_free(elemP);
    }
}



static void
endElement_(void *          const userData,
            const xmlChar * const name ATTR_UNUSED) {

    ParseContext * contextP;

    assert(userData != NULL && name != NULL);

    /* Get our context and see if an error has already occured. */
    contextP = (ParseContext*) userData;
    if (!contextP->env.fault_occurred) {
        assert(xmlrpc_streq((const char *)name,
                            contextP->currentP->name));
        assert(contextP->currentP->parentP != NULL ||
               contextP->currentP == contextP->rootP);

        /* Add a trailing '\0' to our cdata. */
        xmlElementAppendCdata(&contextP->env, contextP->currentP, "\0", 1);
        if (!contextP->env.fault_occurred) {
            /* Pop our "stack" of elements. */
            contextP->currentP = contextP->currentP->parentP;
        }
    }
}



static void
characterData(void *          const userData,
              const xmlChar * const s,
              int             const len) {

    ParseContext * contextP;

    assert(userData != NULL && s != NULL);
    
    /* Get our context and see if an error has already occured. */
    contextP = (ParseContext*)userData;
    if (!contextP->env.fault_occurred) {
        assert(contextP->currentP != NULL);
	
        xmlElementAppendCdata(&contextP->env,
                              contextP->currentP,
                              (char *)s,
                              len);
    }
}



/*=========================================================================
**  LibXML Driver
**=========================================================================
**  XXX - We should allow the user to specify the encoding of our xml_data.
*/

static xmlSAXHandler const saxHandler = {
    NULL,      /* internalSubset */
    NULL,      /* isStandalone */
    NULL,      /* hasInternalSubset */
    NULL,      /* hasExternalSubset */
    NULL,      /* resolveEntity */
    NULL,      /* getEntity */
    NULL,      /* entityDecl */
    NULL,      /* notationDecl */
    NULL,      /* attributeDecl */
    NULL,      /* elementDecl */
    NULL,      /* unparsedEntityDecl */
    NULL,      /* setDocumentLocator */
    NULL,      /* startDocument */
    NULL,      /* endDocument */
    startElement_,       /* startElement */
    endElement_,         /* endElement */
    NULL,      /* reference */
    characterData,      /* characters */
    NULL,      /* ignorableWhitespace */
    NULL,      /* processingInstruction */
    NULL,      /* comment */
    NULL,      /* warning */
    NULL,      /* error */
    NULL,      /* fatalError */
    NULL,      /* getParameterEntity */
    NULL,      /* cdataBlock */
    NULL,      /* externalSubset */
    1          /* initialized */
    
    /* Following are SAX2 fields. Any ifdef here? */ 

    ,NULL,     /* _private */
    NULL,      /* startElementNs */
    NULL,      /* endElementNs */
    NULL       /* serror */
};



void
xml_parse(xmlrpc_env *   const envP,
          const char *   const xmlData,
          size_t         const xmlDataLen,
          xml_element ** const resultPP) {

    ParseContext context;
    xmlParserCtxt * parserP;

    XMLRPC_ASSERT_ENV_OK(envP);
    assert(xmlData != NULL);

    xmlrpc_env_init(&context.env);
    context.rootP    = NULL;
    context.currentP = NULL;

    parserP = xmlCreatePushParserCtxt((xmlSAXHandler *)&saxHandler, &context,
                                      NULL, 0, NULL);
    if (!parserP)
        xmlrpc_faultf(envP, "Failed to create libxml2 parser.");
    else {
        int rc;

        rc = xmlParseChunk(parserP, xmlData, xmlDataLen, 1);

        if (rc != 0)
            xmlrpc_env_set_fault(envP, XMLRPC_PARSE_ERROR,
                                 "XML parsing failed");
        else {
            if (context.env.fault_occurred) {
                xmlrpc_env_set_fault(envP, XMLRPC_PARSE_ERROR,
                                     context.env.fault_string);
                /* This should be done by the parser, but I'm not sure which
                   callbacks need to do it.
                */
                if (context.rootP)
                    xml_element_free(context.rootP);
            } else {
                assert(context.rootP != NULL);
                assert(context.currentP == NULL);

                *resultPP = context.rootP;
            }
        }
        /* xmlParseChunk() creates a document.  You find it with
           parserP->myDoc.
        */
        if (parserP->myDoc)
            xmlFreeDoc(parserP->myDoc);
        xmlFreeParserCtxt(parserP);
    }
    xmlrpc_env_clean(&context.env);
}



