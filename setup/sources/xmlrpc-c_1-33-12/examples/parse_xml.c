/*=============================================================================
                               parse_xml
===============================================================================
  This is an example of using the XML parsing facilities of
  XML-RPC For C/C++, in particular its libxmlrpc_xmlparse library.

  Feed XML (any XML - doesn't have to be XML-RPC) to this program as
  standard input.  It parses it and tells you what it sees.

  Example:

    $ ./gen_sample_add_xml | ./parse_xml

=============================================================================*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xmlrpc-c/base.h"
#include "xmlrpc-c/xmlparser.h"



static const char *
getStdin(void) {

    size_t const maxInputSize = 64 * 1024;
    char * retval;

    retval = malloc(maxInputSize + 1);

    if (retval == NULL) {
        fprintf(stderr, "malloc failed.");
        exit(100);
    } else {
        size_t rc;

        rc = fread(retval, 1, maxInputSize, stdin);

        if (rc < 0)
            fprintf(stderr, "Failed to read Standard Input");
        else {
            size_t const bytesRead = rc;
            retval[bytesRead] = '\0';
        }
    }
    return retval;
}



static void
describeXmlElement(const xml_element * const elemP,
                   const char *        const prefix) {

    unsigned int i;

    printf("%sXML element type:         '%s'\n",
           prefix, xml_element_name(elemP));

    printf("%sNumber of child elements: %u\n",
           prefix, xml_element_children_size(elemP));

    for (i = 0; i < xml_element_children_size(elemP); ++i) {
        char * const newPrefix = malloc(strlen(prefix) + 2);
        sprintf(newPrefix, "%s  ", prefix);
        describeXmlElement(xml_element_children(elemP)[i], newPrefix);
        free(newPrefix);
    }
}



int
main(int argc, const char ** argv) {

    const char * xml;
    xmlrpc_env env;
    xml_element * elemP;

    xmlrpc_env_init(&env);

    xml = getStdin();

    xml_parse(&env, xml, strlen(xml), &elemP);

    if (env.fault_occurred)
        printf("xml_parse failed.  %s\n", env.fault_string);
    else {
        printf("Parsed successfully.\n");

        describeXmlElement(elemP, "");
    }
    xmlrpc_env_clean(&env);

    return 0;
}
