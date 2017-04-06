const ENCODING *
NS(xmlrpc_XmlGetUtf8InternalEncoding)(void) {

    return &ns(internal_utf8_encoding).enc;
}



const ENCODING *
NS(xmlrpc_XmlGetUtf16InternalEncoding)(void) {

#if XML_BYTE_ORDER == 12
    return &ns(internal_little2_encoding).enc;
#elif XML_BYTE_ORDER == 21
    return &ns(internal_big2_encoding).enc;
#else
    const short n = 1;
    return *(const char *)&n ?
        &ns(internal_little2_encoding).enc :
        &ns(internal_big2_encoding).enc;
#endif
}

static
const ENCODING *NS(encodings)[] = {
  &ns(latin1_encoding).enc,
  &ns(ascii_encoding).enc,
  &ns(utf8_encoding).enc,
  &ns(big2_encoding).enc,
  &ns(big2_encoding).enc,
  &ns(little2_encoding).enc,
  &ns(utf8_encoding).enc /* NO_ENC */
};

static
int NS(initScanProlog)(const ENCODING *enc, const char *ptr, const char *end,
		       const char **nextTokPtr)
{
  return initScan(NS(encodings), (const INIT_ENCODING *)enc, XML_PROLOG_STATE, ptr, end, nextTokPtr);
}

static
int NS(initScanContent)(const ENCODING *enc, const char *ptr, const char *end,
		       const char **nextTokPtr)
{
  return initScan(NS(encodings), (const INIT_ENCODING *)enc, XML_CONTENT_STATE, ptr, end, nextTokPtr);
}



int
NS(xmlrpc_XmlInitEncoding)(INIT_ENCODING *   const p,
                           const ENCODING ** const encPP,
                           const char *      const name) {

    int const index = getEncodingIndex(name);

    int retval;

    if (index == UNKNOWN_ENC)
        retval = 0;
    else {
        SET_INIT_ENC_INDEX(p, index);
        p->initEnc.scanners[XML_PROLOG_STATE] = NS(initScanProlog);
        p->initEnc.scanners[XML_CONTENT_STATE] = NS(initScanContent);
        p->initEnc.updatePosition = initUpdatePosition;
        p->encPtr = encPP;
        *encPP = &(p->initEnc);

        retval = 1;
    }
    return retval;
}



static
const ENCODING *NS(findEncoding)(const ENCODING *enc, const char *ptr, const char *end)
{
#define ENCODING_MAX 128
  char buf[ENCODING_MAX];
  char *p = buf;
  int i;
  XmlUtf8Convert(enc, &ptr, end, &p, p + ENCODING_MAX - 1);
  if (ptr != end)
    return 0;
  *p = 0;
  if (streqci(buf, KW_UTF_16) && enc->minBytesPerChar == 2)
    return enc;
  i = getEncodingIndex(buf);
  if (i == UNKNOWN_ENC)
    return 0;
  return NS(encodings)[i];
}



int
NS(xmlrpc_XmlParseXmlDecl)(int               const isGeneralTextEntity,
                           const ENCODING *  const encodingOfDecl,
                           const char *      const piText,
                           const char *      const end,
                           const char **     const badP,
                           const char **     const versionP,
                           const char **     const encodingNameP,
                           const ENCODING ** const encodingPP,
                           int *             const standaloneP) {
/*----------------------------------------------------------------------------
   Parse an XML declaration XML Processing Instruction (PI), i.e.
   "<?xml ... ?>.

   'piText' points to the beginning of the PI (to the "<").

   'end' points to the end of the PI (just past the ">").

   'encodingOfDecl' is the character encoding of the declaration.

   Return as *encodingNameP the value of the "encoding" pseudo-attribute in
   the declaration.  Furthermore, return as *encodingPP a handle for the
   encoding so named, or NULL if we don't recognize the name.

   Return as *standaloneP 1 if the value of the "standalone" pseudo-attribute
   in the declaration is "yes"; 0 if it is "no".

   Return as *versionP the value of the "version" pseudo-attribute in the
   declaration.

   For all the return values, the pointer may be null, and in that case we
   just don't return that information.

   For all the return values, if the pseudo-attribute does not appear in
   the declaration, we just leave the pointed-to variable unchanged.
   (THIS IS WRONG; WE NEED TO FIX THIS).

   Iff the declaration has invalid syntax, return return value 1 and return as
   *badP a pointer to the part of the declaration that is bad.
-----------------------------------------------------------------------------*/
  return doParseXmlDecl(NS(findEncoding),
                        isGeneralTextEntity,
                        encodingOfDecl,
                        piText,
                        end,
                        badP,
                        versionP,
                        encodingNameP,
                        encodingPP,
                        standaloneP);
}
