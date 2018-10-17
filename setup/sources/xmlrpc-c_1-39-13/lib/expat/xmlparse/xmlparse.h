/*
  Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
  See the file copying.txt for copying permission.
*/

#ifndef XMLPARSE_H_INCLUDED
#define XMLPARSE_H_INCLUDED

#include "xmlrpc-c/c_util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *XML_Parser;

/* Information is UTF-8 encoded. */
typedef char XML_Char;
typedef char XML_LChar;

/* Constructs a new parser; encoding is the encoding specified by the external
protocol or null if there is none specified. */

XMLRPC_DLLEXPORT
XML_Parser
xmlrpc_XML_ParserCreate(const XML_Char * const encoding);

/* Constructs a new parser and namespace processor.  Element type names
and attribute names that belong to a namespace will be expanded;
unprefixed attribute names are never expanded; unprefixed element type
names are expanded only if there is a default namespace. The expanded
name is the concatenation of the namespace URI, the namespace separator character,
and the local part of the name.  If the namespace separator is '\0' then
the namespace URI and the local part will be concatenated without any
separator.  When a namespace is not declared, the name and prefix will be
passed through without expansion. */

XMLRPC_DLLEXPORT
XML_Parser
xmlrpc_XML_ParserCreateNS(const XML_Char * const encoding,
                          XML_Char const namespaceSeparator);


/* atts is array of name/value pairs, terminated by 0;
   names and values are 0 terminated. */

typedef void (*XML_StartElementHandler)(void *userData,
                    const XML_Char *name,
                    const XML_Char **atts);

typedef void (*XML_EndElementHandler)(void *userData,
                      const XML_Char *name);

/* s is not 0 terminated. */
typedef void (*XML_CharacterDataHandler)(void *userData,
                     const XML_Char *s,
                     int len);

/* target and data are 0 terminated */
typedef void (*XML_ProcessingInstructionHandler)(void *userData,
                         const XML_Char *target,
                         const XML_Char *data);

/* data is 0 terminated */
typedef void (*XML_CommentHandler)(void *userData, const XML_Char *data);

typedef void (*XML_StartCdataSectionHandler)(void *userData);
typedef void (*XML_EndCdataSectionHandler)(void *userData);

/* This is called for any characters in the XML document for
which there is no applicable handler.  This includes both
characters that are part of markup which is of a kind that is
not reported (comments, markup declarations), or characters
that are part of a construct which could be reported but
for which no handler has been supplied. The characters are passed
exactly as they were in the XML document except that
they will be encoded in UTF-8.  Line boundaries are not normalized.
Note that a byte order mark character is not passed to the default handler.
There are no guarantees about how characters are divided between calls
to the default handler: for example, a comment might be split between
multiple calls. */

typedef void (*XML_DefaultHandler)(void *userData,
                   const XML_Char *s,
                   int len);

/* This is called for the start of the DOCTYPE declaration when the
name of the DOCTYPE is encountered. */
typedef void (*XML_StartDoctypeDeclHandler)(void *userData,
                        const XML_Char *doctypeName);

/* This is called for the start of the DOCTYPE declaration when the
closing > is encountered, but after processing any external subset. */
typedef void (*XML_EndDoctypeDeclHandler)(void *userData);

/* This is called for a declaration of an unparsed (NDATA)
entity.  The base argument is whatever was set by XML_SetBase.
The entityName, systemId and notationName arguments will never be null.
The other arguments may be. */

typedef void (*XML_UnparsedEntityDeclHandler)(void *userData,
                          const XML_Char *entityName,
                          const XML_Char *base,
                          const XML_Char *systemId,
                          const XML_Char *publicId,
                          const XML_Char *notationName);

/* This is called for a declaration of notation.
The base argument is whatever was set by XML_SetBase.
The notationName will never be null.  The other arguments can be. */

typedef void (*XML_NotationDeclHandler)(void *userData,
                    const XML_Char *notationName,
                    const XML_Char *base,
                    const XML_Char *systemId,
                    const XML_Char *publicId);

typedef void (*XML_ExternalParsedEntityDeclHandler)(void *userData,
                            const XML_Char *entityName,
                            const XML_Char *base,
                            const XML_Char *systemId,
                            const XML_Char *publicId);

typedef void (*XML_InternalParsedEntityDeclHandler)(void *userData,
                            const XML_Char *entityName,
                            const XML_Char *replacementText,
                            int replacementTextLength);

/* When namespace processing is enabled, these are called once for
each namespace declaration. The call to the start and end element
handlers occur between the calls to the start and end namespace
declaration handlers. For an xmlns attribute, prefix will be null.
For an xmlns="" attribute, uri will be null. */

typedef void (*XML_StartNamespaceDeclHandler)(void *userData,
                          const XML_Char *prefix,
                          const XML_Char *uri);

typedef void (*XML_EndNamespaceDeclHandler)(void *userData,
                        const XML_Char *prefix);

/* This is called if the document is not standalone (it has an
external subset or a reference to a parameter entity, but does not
have standalone="yes"). If this handler returns 0, then processing
will not continue, and the parser will return a
XML_ERROR_NOT_STANDALONE error. */

typedef int (*XML_NotStandaloneHandler)(void *userData);

/* This is called for a reference to an external parsed general entity.
The referenced entity is not automatically parsed.
The application can parse it immediately or later using
XML_ExternalEntityParserCreate.
The parser argument is the parser parsing the entity containing the reference;
it can be passed as the parser argument to XML_ExternalEntityParserCreate.
The systemId argument is the system identifier as specified in the entity declaration;
it will not be null.
The base argument is the system identifier that should be used as the base for
resolving systemId if systemId was relative; this is set by XML_SetBase;
it may be null.
The publicId argument is the public identifier as specified in the entity declaration,
or null if none was specified; the whitespace in the public identifier
will have been normalized as required by the XML spec.
The context argument specifies the parsing context in the format
expected by the context argument to
XML_ExternalEntityParserCreate; context is valid only until the handler
returns, so if the referenced entity is to be parsed later, it must be copied.
The handler should return 0 if processing should not continue because of
a fatal error in the handling of the external entity.
In this case the calling parser will return an XML_ERROR_EXTERNAL_ENTITY_HANDLING
error.
Note that unlike other handlers the first argument is the parser, not userData. */

typedef int (*XML_ExternalEntityRefHandler)(XML_Parser parser,
                        const XML_Char *context,
                        const XML_Char *base,
                        const XML_Char *systemId,
                        const XML_Char *publicId);

/* This structure is filled in by the XML_UnknownEncodingHandler
to provide information to the parser about encodings that are unknown
to the parser.
The map[b] member gives information about byte sequences
whose first byte is b.
If map[b] is c where c is >= 0, then b by itself encodes the Unicode scalar value c.
If map[b] is -1, then the byte sequence is malformed.
If map[b] is -n, where n >= 2, then b is the first byte of an n-byte
sequence that encodes a single Unicode scalar value.
The data member will be passed as the first argument to the convert function.
The convert function is used to convert multibyte sequences;
s will point to a n-byte sequence where map[(unsigned char)*s] == -n.
The convert function must return the Unicode scalar value
represented by this byte sequence or -1 if the byte sequence is malformed.
The convert function may be null if the encoding is a single-byte encoding,
that is if map[b] >= -1 for all bytes b.
When the parser is finished with the encoding, then if release is not null,
it will call release passing it the data member;
once release has been called, the convert function will not be called again.

Expat places certain restrictions on the encodings that are supported
using this mechanism.

1. Every ASCII character that can appear in a well-formed XML document,
other than the characters

  $@\^`{}~

must be represented by a single byte, and that byte must be the
same byte that represents that character in ASCII.

2. No character may require more than 4 bytes to encode.

3. All characters encoded must have Unicode scalar values <= 0xFFFF,
(ie characters that would be encoded by surrogates in UTF-16
are  not allowed).  Note that this restriction doesn't apply to
the built-in support for UTF-8 and UTF-16.

4. No Unicode character may be encoded by more than one distinct sequence
of bytes. */

typedef struct {
  int map[256];
  void *data;
  int (*convert)(void *data, const char *s);
  void (*release)(void *data);
} XML_Encoding;

/* This is called for an encoding that is unknown to the parser.
The encodingHandlerData argument is that which was passed as the
second argument to XML_SetUnknownEncodingHandler.
The name argument gives the name of the encoding as specified in
the encoding declaration.
If the callback can provide information about the encoding,
it must fill in the XML_Encoding structure, and return 1.
Otherwise it must return 0.
If info does not describe a suitable encoding,
then the parser will return an XML_UNKNOWN_ENCODING error. */

typedef int (*XML_UnknownEncodingHandler)(void *encodingHandlerData,
                      const XML_Char *name,
                      XML_Encoding *info);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetElementHandler(XML_Parser parser,
                             XML_StartElementHandler start,
                             XML_EndElementHandler end);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetCharacterDataHandler(XML_Parser parser,
                                   XML_CharacterDataHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetProcessingInstructionHandler(
    XML_Parser parser,
    XML_ProcessingInstructionHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetCommentHandler(XML_Parser parser,
                             XML_CommentHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetCdataSectionHandler(XML_Parser parser,
                                  XML_StartCdataSectionHandler start,
                                  XML_EndCdataSectionHandler end);

/* This sets the default handler and also inhibits expansion of
   internal entities.  The entity reference will be passed to the default
   handler.
*/

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetDefaultHandler(XML_Parser parser,
                             XML_DefaultHandler handler);

/* This sets the default handler but does not inhibit expansion of internal
   entities.  The entity reference will not be passed to the default
   handler.
*/

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetDefaultHandlerExpand(XML_Parser parser,
                                   XML_DefaultHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetDoctypeDeclHandler(XML_Parser parser,
                                 XML_StartDoctypeDeclHandler start,
                                 XML_EndDoctypeDeclHandler end);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetUnparsedEntityDeclHandler(XML_Parser parser,
                                        XML_UnparsedEntityDeclHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetNotationDeclHandler(XML_Parser parser,
                                  XML_NotationDeclHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetExternalParsedEntityDeclHandler(
    XML_Parser parser,
    XML_ExternalParsedEntityDeclHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetInternalParsedEntityDeclHandler(
    XML_Parser parser,
    XML_InternalParsedEntityDeclHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetNamespaceDeclHandler(XML_Parser parser,
                                   XML_StartNamespaceDeclHandler start,
                                   XML_EndNamespaceDeclHandler end);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetNotStandaloneHandler(XML_Parser parser,
                                   XML_NotStandaloneHandler handler);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetExternalEntityRefHandler(XML_Parser parser,
                                       XML_ExternalEntityRefHandler handler);

/* If a non-null value for arg is specified here, then it will be
   passed as the first argument to the external entity ref handler
   instead of the parser object.
*/
XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetExternalEntityRefHandlerArg(XML_Parser, void *arg);

XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetUnknownEncodingHandler(XML_Parser parser,
                                     XML_UnknownEncodingHandler handler,
                                     void *encodingHandlerData);

/* This can be called within a handler for a start element, end element,
   processing instruction or character data.  It causes the corresponding
   markup to be passed to the default handler.
*/
XMLRPC_DLLEXPORT
void
xmlrpc_XML_DefaultCurrent(XML_Parser parser);

/* This value is passed as the userData argument to callbacks. */
XMLRPC_DLLEXPORT
void
xmlrpc_XML_SetUserData(XML_Parser parser, void *userData);

/* Returns the last value set by XML_SetUserData or null. */
#define XML_GetUserData(parser) (*(void **)(parser))

/* This is equivalent to supplying an encoding argument
to XML_ParserCreate. It must not be called after XML_Parse
or XML_ParseBuffer. */

XMLRPC_DLLEXPORT
int
xmlrpc_XML_SetEncoding(XML_Parser parser, const XML_Char *encoding);

/* If this function is called, then the parser will be passed
   as the first argument to callbacks instead of userData.
   The userData will still be accessible using XML_GetUserData.
*/
XMLRPC_DLLEXPORT
void
xmlrpc_XML_UseParserAsHandlerArg(XML_Parser parser);

/* Sets the base to be used for resolving relative URIs in system
   identifiers in declarations.  Resolving relative identifiers is left
   to the application: this value will be passed through as the base
   argument to the XML_ExternalEntityRefHandler, XML_NotationDeclHandler
   and XML_UnparsedEntityDeclHandler. The base argument will be copied.
   Returns zero if out of memory, non-zero otherwise.
*/
XMLRPC_DLLEXPORT
int
xmlrpc_XML_SetBase(XML_Parser parser, const XML_Char *base);

XMLRPC_DLLEXPORT 
const XML_Char *
xmlrpc_XML_GetBase(XML_Parser parser);

/* Returns the number of the attribute/value pairs passed in last call
   to the XML_StartElementHandler that were specified in the start-tag
   rather than defaulted. Each attribute/value pair counts as 2; thus
   this correspondds to an index into the atts array passed to the
   XML_StartElementHandler.
*/
XMLRPC_DLLEXPORT
int
xmlrpc_XML_GetSpecifiedAttributeCount(XML_Parser parser);

/* Returns the index of the ID attribute passed in the last call to
   XML_StartElementHandler, or -1 if there is no ID attribute.  Each
   attribute/value pair counts as 2; thus this correspondds to an index
   into the atts array passed to the XML_StartElementHandler.
*/
XMLRPC_DLLEXPORT
int
xmlrpc_XML_GetIdAttributeIndex(XML_Parser parser);

/* Parses some input. Returns 0 if a fatal error is detected.
   The last call to XML_Parse must have isFinal true;
   len may be zero for this call (or any other).
*/
XMLRPC_DLLEXPORT
int
xmlrpc_XML_Parse(XML_Parser   const xmlParserP,
                 const char * const s,
                 size_t       const len,
                 int          const isFinal);

XMLRPC_DLLEXPORT 
void *
xmlrpc_XML_GetBuffer(XML_Parser const xmlParserP,
                     size_t     const len);

XMLRPC_DLLEXPORT
int
xmlrpc_XML_ParseBuffer(XML_Parser const parser,
                       int        const len,
                       int        const isFinal);

/* Creates an XML_Parser object that can parse an external general
   entity; context is a '\0'-terminated string specifying the parse
   context; encoding is a '\0'-terminated string giving the name of the
   externally specified encoding, or null if there is no externally
   specified encoding.  The context string consists of a sequence of
   tokens separated by formfeeds (\f); a token consisting of a name
   specifies that the general entity of the name is open; a token of the
   form prefix=uri specifies the namespace for a particular prefix; a
   token of the form =uri specifies the default namespace.  This can be
   called at any point after the first call to an
   ExternalEntityRefHandler so longer as the parser has not yet been
   freed.  The new parser is completely independent and may safely be
   used in a separate thread.  The handlers and userData are initialized
   from the parser argument.  Returns 0 if out of memory.  Otherwise
   returns a new XML_Parser object.
*/
XMLRPC_DLLEXPORT
XML_Parser
xmlrpc_XML_ExternalEntityParserCreate(XML_Parser parser,
                                      const XML_Char *context,
                                      const XML_Char *encoding);

enum XML_ParamEntityParsing {
  XML_PARAM_ENTITY_PARSING_NEVER,
  XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE,
  XML_PARAM_ENTITY_PARSING_ALWAYS
};

/* Controls parsing of parameter entities (including the external DTD
   subset). If parsing of parameter entities is enabled, then references
   to external parameter entities (including the external DTD subset)
   will be passed to the handler set with
   XML_SetExternalEntityRefHandler.  The context passed will be 0.
   Unlike external general entities, external parameter entities can only
   be parsed synchronously.  If the external parameter entity is to be
   parsed, it must be parsed during the call to the external entity ref
   handler: the complete sequence of XML_ExternalEntityParserCreate,
   XML_Parse/XML_ParseBuffer and XML_ParserFree calls must be made during
   this call.  After XML_ExternalEntityParserCreate has been called to
   create the parser for the external parameter entity (context must be 0
   for this call), it is illegal to make any calls on the old parser
   until XML_ParserFree has been called on the newly created parser.  If
   the library has been compiled without support for parameter entity
   parsing (ie without XML_DTD being defined), then
   XML_SetParamEntityParsing will return 0 if parsing of parameter
   entities is requested; otherwise it will return non-zero.
*/
XMLRPC_DLLEXPORT
int
xmlrpc_XML_SetParamEntityParsing(XML_Parser parser,
                                 enum XML_ParamEntityParsing parsing);

/* Sets the hash salt to use for internal hash calculations.
   Helps in preventing DoS attacks based on predicting hash
   function behavior. This must be called before parsing is started.
   Returns 1 if successful, 0 when called after parsing has started.
*/
int
XML_SetHashSalt(XML_Parser parser, unsigned long hash_salt);

enum XML_Error {
  XML_ERROR_NONE,
  XML_ERROR_NO_MEMORY,
  XML_ERROR_SYNTAX,
  XML_ERROR_NO_ELEMENTS,
  XML_ERROR_INVALID_TOKEN,
  XML_ERROR_UNCLOSED_TOKEN,
  XML_ERROR_PARTIAL_CHAR,
  XML_ERROR_TAG_MISMATCH,
  XML_ERROR_DUPLICATE_ATTRIBUTE,
  XML_ERROR_JUNK_AFTER_DOC_ELEMENT,
  XML_ERROR_PARAM_ENTITY_REF,
  XML_ERROR_UNDEFINED_ENTITY,
  XML_ERROR_RECURSIVE_ENTITY_REF,
  XML_ERROR_ASYNC_ENTITY,
  XML_ERROR_BAD_CHAR_REF,
  XML_ERROR_BINARY_ENTITY_REF,
  XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF,
  XML_ERROR_MISPLACED_XML_PI,
  XML_ERROR_UNKNOWN_ENCODING,
  XML_ERROR_INCORRECT_ENCODING,
  XML_ERROR_UNCLOSED_CDATA_SECTION,
  XML_ERROR_EXTERNAL_ENTITY_HANDLING,
  XML_ERROR_NOT_STANDALONE
};

/* If xmlrpc_XML_Parse or xmlrpc_XML_ParseBuffer have returned 0, then
   xmlrpc_XML_GetErrorString and xmlrpc_XML_GetErrorCode return
   information about the error.
*/

XMLRPC_DLLEXPORT
enum XML_Error
xmlrpc_XML_GetErrorCode(XML_Parser const parser);

XMLRPC_DLLEXPORT
const char *
xmlrpc_XML_GetErrorString(XML_Parser const parser);

/* These functions return information about the current parse location.
   They may be called when XML_Parse or XML_ParseBuffer return 0;
   in this case the location is the location of the character at which
   the error was detected.
   They may also be called from any other callback called to report
   some parse event; in this the location is the location of the first
   of the sequence of characters that generated the event.
*/

XMLRPC_DLLEXPORT
int
xmlrpc_XML_GetCurrentLineNumber(XML_Parser parser);

XMLRPC_DLLEXPORT
int
xmlrpc_XML_GetCurrentColumnNumber(XML_Parser parser);

XMLRPC_DLLEXPORT
long
xmlrpc_XML_GetCurrentByteIndex(XML_Parser parser);

/* Return the number of bytes in the current event.
Returns 0 if the event is in an internal entity. */

XMLRPC_DLLEXPORT
int
xmlrpc_XML_GetCurrentByteCount(XML_Parser parser);

/* For backwards compatibility with previous versions. */
#define XML_GetErrorLineNumber XML_GetCurrentLineNumber
#define XML_GetErrorColumnNumber XML_GetCurrentColumnNumber
#define XML_GetErrorByteIndex XML_GetCurrentByteIndex

/* Frees memory used by the parser. */
XMLRPC_DLLEXPORT
void
xmlrpc_XML_ParserFree(XML_Parser parser);

/* Returns a string describing the error. */
XMLRPC_DLLEXPORT 
const XML_LChar *
xmlrpc_XML_ErrorString(int const code);

#ifdef __cplusplus
}
#endif

#endif
