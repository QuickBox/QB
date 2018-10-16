#include "xmlrpc-c/base.h"
#include "xmlrpc-c/xmlparser.h"



static unsigned int globallyInitialized = 0;
    /* Initialization count */


/* It ought to be a requirement for any user of libxmlrpc to call
   xmlrpc_init(), but for ten years it wasn't, so we can't start requiring it
   now except for new services.

   One thing we know requires it (and always did, but didn't show up as a
   problem until 2012) is libxml2.  Without a call to xmlrpc_init(),
   if libxmlrpc was built to use libxml2, the program leaks lots of memory.
*/

void
xmlrpc_init(xmlrpc_env * const envP) {

    /* Note that this is specified as not thread safe; user calls it at
       the beginning of his program, when it is only one thread.
    */

    XMLRPC_ASSERT_ENV_OK(envP);
    
    if (globallyInitialized == 0) {
        xml_init(envP);  /* Initialize the XML parser library */
    }
    ++globallyInitialized;
}



void
xmlrpc_term(void) {

    /* Note that this is specified as not thread safe; user calls it at
       the end of his program, when it is only one thread.
    */

    XMLRPC_ASSERT(globallyInitialized > 0);

    --globallyInitialized;

    if (globallyInitialized == 0) {
        xml_term();
    }
}


