#include "xmlrpc_config.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "xmlrpc-c/base.h"
#include "xmlrpc-c/base_int.h"
#include "int.h"

#define KEY_ERROR_BUFFER_SZ (32)


void
xmlrpc_destroyStruct(xmlrpc_value * const structP) {

    _struct_member * const members = 
        XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
    size_t const size = 
        XMLRPC_MEMBLOCK_SIZE(_struct_member, &structP->_block);

    unsigned int i;

    for (i = 0; i < size; ++i) {
        xmlrpc_DECREF(members[i].key);
        xmlrpc_DECREF(members[i].value);
    }
    XMLRPC_MEMBLOCK_CLEAN(_struct_member, &structP->_block);
}



/*=========================================================================
**  xmlrpc_struct_new
**=========================================================================
**  Create a new <struct> value. The corresponding destructor code
**  currently lives in xmlrpc_DECREF.
**
**  We store the individual members in an array of _struct_member. This
**  contains a key, a hash code, and a value. We look up keys by doing
**  a linear search of the hash codes.
*/

xmlrpc_value *
xmlrpc_struct_new(xmlrpc_env * const envP) {

    xmlrpc_value * valP;

    XMLRPC_ASSERT_ENV_OK(envP);

    xmlrpc_createXmlrpcValue(envP, &valP);
    if (!envP->fault_occurred) {
        valP->_type = XMLRPC_TYPE_STRUCT;

        XMLRPC_MEMBLOCK_INIT(_struct_member, envP, &valP->_block, 0);

        if (envP->fault_occurred)
            free(valP);
    }
    return valP;
}



/*=========================================================================
**  xmlrpc_struct_size
**=========================================================================
**  Return the number of key-value pairs contained in the struct. If the
**  value is not a struct, return -1 and set a fault.
*/

int 
xmlrpc_struct_size(xmlrpc_env *   const envP,
                   xmlrpc_value * const structP) {

    int retval;

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);

    if (structP->_type != XMLRPC_TYPE_STRUCT) {
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TYPE_ERROR, "Value is not a struct.  It is type #%d",
            structP->_type);
        retval = -1;
    } else {
        size_t const size =
            XMLRPC_MEMBLOCK_SIZE(_struct_member, &structP->_block);

        assert((size_t)(int)size == size);
            /* Because structs are defined to have few enough members */

        retval = (int)size;
    }
    return retval;
}



static uint32_t
hashStructKey(const char * const key, 
              size_t       const keyLen) {

    uint32_t hash;
    size_t i;

    XMLRPC_ASSERT(key != NULL);
    
    /* This is the Bernstein hash, optimized for lower case ASCII
       keys.  Note that the bytes of such a key differ only in their
       lower 5 bits.
    */
    for (hash = 0, i = 0; i < keyLen; ++i)
        hash = hash + key[i] + (hash << 5);

    return hash;
}



static void
findMember(xmlrpc_value * const structP, 
           const char *   const key, 
           size_t         const keyLen,
           bool *         const foundP,
           unsigned int * const indexP) {

    size_t size, i;
    uint32_t searchHash;
    _struct_member * contents;  /* array */
    bool found;
    size_t foundIndex;  /* Meaningful only when 'found' is true */

    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT(key != NULL);
    foundIndex = 0;  /* defeat used-before-set compiler warning */

    /* Look for our key. */
    searchHash = hashStructKey(key, keyLen);
    size = XMLRPC_MEMBLOCK_SIZE(_struct_member, &structP->_block);
    contents = XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
    for (i = 0, found = false; i < size && !found; ++i) {
        if (contents[i].keyHash == searchHash) {
            xmlrpc_value * const keyvalP = contents[i].key;
            const char * const keystr =
                XMLRPC_MEMBLOCK_CONTENTS(char, &keyvalP->_block);
            size_t const keystrSize =
                XMLRPC_MEMBLOCK_SIZE(char, &keyvalP->_block)-1;
            if (keystrSize == keyLen && memcmp(key, keystr, keyLen) == 0) {
                found = true;
                foundIndex = i;
            }
        }   
    }
    if (found) {
        assert((size_t)(int)foundIndex == foundIndex);
            /* Definition of structure says it has few enough members */

        if (indexP)
            *indexP = foundIndex;
    }
    *foundP = found;
}



/*=========================================================================
**  xmlrpc_struct_has_key
**=========================================================================
*/

int 
xmlrpc_struct_has_key(xmlrpc_env *   const envP,
                      xmlrpc_value * const strctP,
                      const char *   const key) {

    XMLRPC_ASSERT(key != NULL);
    return xmlrpc_struct_has_key_n(envP, strctP, key, strlen(key));
}



int 
xmlrpc_struct_has_key_n(xmlrpc_env   * const envP,
                        xmlrpc_value * const structP,
                        const char *   const key, 
                        size_t         const keyLen) {
    int retval;

    /* Suppress a compiler warning about uninitialized variables. */
    retval = 0;

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT(key != NULL);
    
    if (structP->_type != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault(envP, XMLRPC_TYPE_ERROR,
                             "Value is not a struct");
    else {
        bool found;

        findMember(structP, key, keyLen, &found, NULL);

        retval = found ? 1 : 0;
    }
    return retval;
}



/*=========================================================================
**  xmlrpc_struct_find_value...
**=========================================================================
**  These functions look up a specified key value in a specified struct.
**  If it exists, they return the value of the struct member.  If not,
**  they return a NULL to indicate such.
*/

/* It would be a nice extension to be able to look up a key that is
   not a text string.
*/

void
xmlrpc_struct_find_value(xmlrpc_env *    const envP,
                         xmlrpc_value *  const structP,
                         const char *    const key,
                         xmlrpc_value ** const valuePP) {
/*----------------------------------------------------------------------------
  Given a key, retrieve a value from the struct.  If the key is not
  present, return NULL as *valuePP.
-----------------------------------------------------------------------------*/
    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT_PTR_OK(key);
    
    if (structP->_type != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TYPE_ERROR, "Value is not a struct.  It is type #%d",
            structP->_type);
    else {
        bool found;
        unsigned int index;

        /* Get our member index. */
        findMember(structP, key, strlen(key), &found, &index);
        if (!found)
            *valuePP = NULL;
        else {
            _struct_member * const members =
                XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
            *valuePP = members[index].value;
            
            XMLRPC_ASSERT_VALUE_OK(*valuePP);
            
            xmlrpc_INCREF(*valuePP);
        }
    }
}



void
xmlrpc_struct_find_value_v(xmlrpc_env *    const envP,
                           xmlrpc_value *  const structP,
                           xmlrpc_value *  const keyP,
                           xmlrpc_value ** const valuePP) {
/*----------------------------------------------------------------------------
  Given a key, retrieve a value from the struct.  If the key is not
  present, return NULL as *valuePP.
-----------------------------------------------------------------------------*/
    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT_VALUE_OK(keyP);
    
    if (structP->_type != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TYPE_ERROR, "Value is not a struct.  It is type #%d",
            structP->_type);
    else {
        if (keyP->_type != XMLRPC_TYPE_STRING)
            xmlrpc_env_set_fault_formatted(
                envP, XMLRPC_TYPE_ERROR, "Key value is not a string.  "
                "It is type #%d",
                keyP->_type);
        else {
            bool found;
            unsigned int index;

            /* Get our member index. */
            findMember(structP, 
                       XMLRPC_MEMBLOCK_CONTENTS(char, &keyP->_block),
                       XMLRPC_MEMBLOCK_SIZE(char, &keyP->_block)-1,
                       &found, &index);
            if (!found)
                *valuePP = NULL;
            else {
                _struct_member * const members =
                    XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
                *valuePP = members[index].value;
                
                XMLRPC_ASSERT_VALUE_OK(*valuePP);
                
                xmlrpc_INCREF(*valuePP);
            }
        }
    }
}



/*=========================================================================
**  xmlrpc_struct_read_value...
**=========================================================================
**  These fail if no member with the specified key exists.
**  Otherwise, they are the same as xmlrpc_struct_find_value...
*/

void
xmlrpc_struct_read_value_v(xmlrpc_env *    const envP,
                           xmlrpc_value *  const structP,
                           xmlrpc_value *  const keyP,
                           xmlrpc_value ** const valuePP) {

    xmlrpc_struct_find_value_v(envP, structP, keyP, valuePP);

    if (!envP->fault_occurred) {
        if (*valuePP == NULL) {
            xmlrpc_env_set_fault_formatted(
                envP, XMLRPC_INDEX_ERROR, "No member of struct has key '%.*s'",
                (int)XMLRPC_MEMBLOCK_SIZE(char, &keyP->_block),
                XMLRPC_MEMBLOCK_CONTENTS(char, &keyP->_block));
        }
    }
}



void
xmlrpc_struct_read_value(xmlrpc_env *    const envP,
                         xmlrpc_value *  const structP,
                         const char *    const key,
                         xmlrpc_value ** const valuePP) {

    xmlrpc_struct_find_value(envP, structP, key, valuePP);
    
    if (!envP->fault_occurred) {
        if (*valuePP == NULL) {
            xmlrpc_env_set_fault_formatted(
                envP, XMLRPC_INDEX_ERROR, "No member of struct has key '%s'",
                key);
            /* We should fix the error message to format the key for display */
        }
    }
}



/*=========================================================================
**  xmlrpc_struct_get_value...
**=========================================================================
**  These are for backward compatibility.  They used to be the only ones.
**  They're deprecated because they don't acquire a reference to the
**  value they return.
*/

xmlrpc_value * 
xmlrpc_struct_get_value_n(xmlrpc_env *   const envP,
                          xmlrpc_value * const structP,
                          const char *   const key, 
                          size_t         const keyLen) {

    xmlrpc_value * retval;
    xmlrpc_value * keyP;  /* 'key' as an XML-RPC string */
    
    keyP = xmlrpc_string_new_lp(envP, keyLen, key);
    if (!envP->fault_occurred) {
        xmlrpc_struct_find_value_v(envP, structP, keyP, &retval);

        if (!envP->fault_occurred) {
            if (retval == NULL) {
                xmlrpc_env_set_fault_formatted(
                    envP, XMLRPC_INDEX_ERROR, 
                    "No member of struct has key '%.*s'",
                    (int)keyLen, key);
                /* We should fix the error message to format the key
                   for display */
            } else
                /* For backward compatibility.  */
                xmlrpc_DECREF(retval);
        }
        xmlrpc_DECREF(keyP);
    }
    return retval;
}



xmlrpc_value * 
xmlrpc_struct_get_value(xmlrpc_env *   const envP,
                        xmlrpc_value * const strctP,
                        const char *   const key) {

    XMLRPC_ASSERT(key != NULL);
    return xmlrpc_struct_get_value_n(envP, strctP, key, strlen(key));
}



/*=========================================================================
**  xmlrpc_struct_set_value
**=========================================================================
*/

void 
xmlrpc_struct_set_value(xmlrpc_env *   const envP,
                        xmlrpc_value * const strctP,
                        const char *   const key,
                        xmlrpc_value * const valueP) {

    XMLRPC_ASSERT(key != NULL);
    xmlrpc_struct_set_value_n(envP, strctP, key, strlen(key), valueP);
}



void 
xmlrpc_struct_set_value_n(xmlrpc_env *    const envP,
                          xmlrpc_value *  const strctP,
                          const char *    const key, 
                          size_t          const keyLen,
                          xmlrpc_value *  const valueP) {

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT(key != NULL);

    if (xmlrpc_value_type(strctP) != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TYPE_ERROR,
            "Trying to set value in something not a struct.  "
            "Type is %d; struct is %d",
            xmlrpc_value_type(strctP), XMLRPC_TYPE_STRUCT);
    else {
        xmlrpc_value * keyvalP;  /* 'key' as an XML-RPC string */

        keyvalP = xmlrpc_string_new_lp(envP, keyLen, key);
        if (!envP->fault_occurred)
            xmlrpc_struct_set_value_v(envP, strctP, keyvalP, valueP);

        xmlrpc_DECREF(keyvalP);
    }
}



static void
changeMemberValue(xmlrpc_value * const structP,
                  unsigned int   const mbrIndex,
                  xmlrpc_value * const newValueP) {
/*----------------------------------------------------------------------------
  Change the value of an existing member. (But be careful--the original and
  new values might be the same object, so watch the order of INCREF and DECREF
  calls!)
-----------------------------------------------------------------------------*/
    _struct_member * const members =
        XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
    _struct_member * const memberP = &members[mbrIndex];
    xmlrpc_value * const oldValueP = memberP->value;

    /* Juggle our references. */
    memberP->value = newValueP;
    xmlrpc_INCREF(memberP->value);
    xmlrpc_DECREF(oldValueP);
}



static void
addNewMember(xmlrpc_env *   const envP,
             xmlrpc_value * const structP,
             xmlrpc_value * const keyvalP,
             xmlrpc_value * const valueP) {
/*----------------------------------------------------------------------------
   Add a new member.  Assume no member already exists with this key.
-----------------------------------------------------------------------------*/
    const char * const key =
        XMLRPC_MEMBLOCK_CONTENTS(char, &keyvalP->_block);
    size_t const keyLen =
        XMLRPC_MEMBLOCK_SIZE(char, &keyvalP->_block) - 1;

    _struct_member newMember;

    newMember.keyHash = hashStructKey(key, keyLen);
    newMember.key     = keyvalP;
    newMember.value   = valueP;

    XMLRPC_MEMBLOCK_APPEND(_struct_member, envP, &structP->_block,
                           &newMember, 1);

    if (!envP->fault_occurred) {
        xmlrpc_INCREF(keyvalP);
        xmlrpc_INCREF(valueP);
    }
}



void 
xmlrpc_struct_set_value_v(xmlrpc_env *   const envP,
                          xmlrpc_value * const structP,
                          xmlrpc_value * const keyvalP,
                          xmlrpc_value * const valueP) {

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT_VALUE_OK(keyvalP);
    XMLRPC_ASSERT_VALUE_OK(valueP);

    if (structP->_type != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault(envP, XMLRPC_TYPE_ERROR,
                             "Value is not a struct");
    else if (keyvalP->_type != XMLRPC_TYPE_STRING)
        xmlrpc_env_set_fault(envP, XMLRPC_TYPE_ERROR,
                             "Key value is not a string");
    else {
        const char * const key =
            XMLRPC_MEMBLOCK_CONTENTS(char, &keyvalP->_block);
        size_t const keyLen =
            XMLRPC_MEMBLOCK_SIZE(char, &keyvalP->_block) - 1;

        bool found;
        unsigned int index;

        findMember(structP, key, keyLen, &found, &index);

        if (found)
            changeMemberValue(structP, index, valueP);
        else
            addNewMember(envP, structP, keyvalP, valueP);
    }
}



/* Note that the order of keys and values is undefined, and may change
   when you modify the struct.
*/

void 
xmlrpc_struct_read_member(xmlrpc_env *    const envP,
                          xmlrpc_value *  const structP,
                          unsigned int    const index,
                          xmlrpc_value ** const keyvalP,
                          xmlrpc_value ** const valueP) {

    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT_PTR_OK(keyvalP);
    XMLRPC_ASSERT_PTR_OK(valueP);

    if (structP->_type != XMLRPC_TYPE_STRUCT)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_TYPE_ERROR, "Attempt to read a struct member "
            "of something that is not a struct");
    else {
        _struct_member * const members =
            XMLRPC_MEMBLOCK_CONTENTS(_struct_member, &structP->_block);
        size_t const size = 
            XMLRPC_MEMBLOCK_SIZE(_struct_member, &structP->_block);

        if (index >= size)
            xmlrpc_env_set_fault_formatted(
                envP, XMLRPC_INDEX_ERROR, "Index %u is beyond the end of "
                "the %u-member structure", index, (unsigned int)size);
        else {
            _struct_member * const memberP = &members[index];
            *keyvalP = memberP->key;
            xmlrpc_INCREF(memberP->key);
            *valueP = memberP->value;
            xmlrpc_INCREF(memberP->value);
        }
    }
}



void 
xmlrpc_struct_get_key_and_value(xmlrpc_env *    const envP,
                                xmlrpc_value *  const structP,
                                int             const index,
                                xmlrpc_value ** const keyvalP,
                                xmlrpc_value ** const valueP) {
/*----------------------------------------------------------------------------
   Same as xmlrpc_struct_read_member(), except doesn't take a reference
   to the returned value.

   This is obsolete.
-----------------------------------------------------------------------------*/
    XMLRPC_ASSERT_ENV_OK(envP);
    XMLRPC_ASSERT_VALUE_OK(structP);
    XMLRPC_ASSERT_PTR_OK(keyvalP);
    XMLRPC_ASSERT_PTR_OK(valueP);

    if (index < 0)
        xmlrpc_env_set_fault_formatted(
            envP, XMLRPC_INDEX_ERROR, "Index %d is negative.", index);
    else {
        xmlrpc_struct_read_member(envP, structP, index, keyvalP, valueP);
        if (!envP->fault_occurred) {
            xmlrpc_DECREF(*keyvalP);
            xmlrpc_DECREF(*valueP);
        }
    }
    if (envP->fault_occurred) {
        *keyvalP = NULL;
        *valueP = NULL;
    }
}



/* Copyright (C) 2001 by First Peer, Inc. All rights reserved.
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

