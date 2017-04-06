#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <xmlrpc-c/base.h>

#include "casprintf.h"
#include "dumpvalue.h"


static void 
die_if_fault_occurred (xmlrpc_env * const envP) {
    if (envP->fault_occurred) {
        fprintf(stderr, "Error: %s (%d)\n",
                envP->fault_string, envP->fault_code);
        exit(1);
    }
}



static void
readFile(xmlrpc_env *        const envP,
         FILE *              const ifP,
         xmlrpc_mem_block ** const fileContentsPP) {

    xmlrpc_mem_block * fileContentsP;

    fileContentsP = XMLRPC_MEMBLOCK_NEW(char, envP, 0);

    while (!envP->fault_occurred && !feof(ifP)) {
        char buffer[4096];
        size_t bytesRead;
        
        bytesRead = fread(buffer, 1, sizeof(buffer), ifP);
        XMLRPC_MEMBLOCK_APPEND(char, envP, 
                               fileContentsP, buffer, bytesRead);
    }
    if (envP->fault_occurred)
        XMLRPC_MEMBLOCK_FREE(char, fileContentsP);

    *fileContentsPP = fileContentsP;
}



int
main(int argc, const char ** argv) {

    xmlrpc_env env;
    const char * methodName;
    xmlrpc_value * paramArrayP;
    xmlrpc_mem_block * callXmlP;

    if (argc-1 > 0) {
        fprintf(stderr, "No arguments.  Input is from Standard Input\n");
        if (argv){}  /* defeat compiler warning */
        exit(99);
    }

    xmlrpc_env_init(&env);

    fprintf(stderr, "Reading call data from Standard Input...\n");

    readFile(&env, stdin, &callXmlP);
    die_if_fault_occurred(&env);

    xmlrpc_parse_call(&env,
                      XMLRPC_MEMBLOCK_CONTENTS(char, callXmlP),
                      XMLRPC_MEMBLOCK_SIZE(char, callXmlP),
                      &methodName,
                      &paramArrayP);

    if (env.fault_occurred)
        printf("Invalid call.  %s\n", env.fault_string);
    else {
        printf("Parsed successfully as XML-RPC call.\n");

        printf("Method name: '%s'\n", methodName);

        printf("Parameter array:\n");

        dumpValue("  ", paramArrayP);

        strfree(methodName);
        xmlrpc_DECREF(paramArrayP);
    }
    XMLRPC_MEMBLOCK_FREE(char, callXmlP);

    xmlrpc_env_clean(&env);

    return 0;
}
