/*
**  Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
**  See the file COPYING for copying permission.
**
**  Written by Alfonso [alfie] Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2.
*/

/* init.c */
void freeBase ( void );
ULONG initBase ( void );

/* loc.c */
void initStrings ( void );
STRPTR getString ( ULONG id );

/* utils.c */
APTR allocVecPooled ( APTR pool , ULONG size );
void freeVecPooled ( APTR pool , APTR mem );
APTR allocArbitratePooled ( ULONG size );
void freeArbitratePooled ( APTR mem , ULONG size );
APTR allocArbitrateVecPooled ( ULONG size );
void freeArbitrateVecPooled ( APTR mem );
void *palloc ( size_t size );
void *prealloc ( void *ptr , size_t size );
void pfree ( void *ptr );
#ifdef __MORPHOS__
ULONG callPkt ( APTR func , ULONG numArgs , ... ) /*__attribute((varargs68k))*/;
#endif

/* xmlparse.c */
XML_Parser XMLCALL XML_ParserCreate (REG(a0,const XML_Char *encodingName));
XML_Parser XMLCALL XML_ParserCreateNS (REG(a0,const XML_Char *encodingName),REG(d0,XML_Char nsSep));
XML_Parser XMLCALL XML_ParserCreate_MM (REG(a0,const XML_Char *encodingName),REG(a1,const XML_Memory_Handling_Suite *memsuite),REG(a2,const XML_Char *nameSep));
XML_Bool XMLCALL XML_ParserReset ( REG(a0,XML_Parser parser ), REG(a1,const XML_Char *encodingName ));
enum XML_Status XMLCALL XML_SetEncoding ( REG(a0,XML_Parser parser ), REG(a1,const XML_Char *encodingName ));
XML_Parser XMLCALL XML_ExternalEntityParserCreate ( REG(a0,XML_Parser oldParser ), REG(a1,const XML_Char *context ),REG(a2,const XML_Char *encodingName));
void XMLCALL XML_ParserFree ( REG(a0,XML_Parser parser ));
void XMLCALL XML_UseParserAsHandlerArg ( REG(a0,XML_Parser parser ));
enum XML_Error XMLCALL XML_UseForeignDTD ( REG(a0,XML_Parser parser ), REG (d0,XML_Bool useDTD ));
void XMLCALL XML_SetReturnNSTriplet ( REG(a0,XML_Parser parser ), REG (d0,int do_nst ));
void XMLCALL XML_SetUserData ( REG(a0,XML_Parser parser ), REG(a1,void *p ));
enum XML_Status XMLCALL XML_SetBase ( REG(a0,XML_Parser parser ), REG(a1,const XML_Char *p ));
const XML_Char *XMLCALL XML_GetBase ( REG(a0,XML_Parser parser ));
int XMLCALL XML_GetSpecifiedAttributeCount ( REG(a0,XML_Parser parser ));
int XMLCALL XML_GetIdAttributeIndex ( REG(a0,XML_Parser parser ));
void XMLCALL XML_SetElementHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartElementHandler start ), REG(a2,XML_EndElementHandler end));
void XMLCALL XML_SetStartElementHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartElementHandler start ));
void XMLCALL XML_SetEndElementHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_EndElementHandler end ));
void XMLCALL XML_SetCharacterDataHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_CharacterDataHandler handler ));
void XMLCALL XML_SetProcessingInstructionHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_ProcessingInstructionHandler handler ));
void XMLCALL XML_SetCommentHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_CommentHandler handler ));
void XMLCALL XML_SetCdataSectionHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartCdataSectionHandler start ), REG(a2,XML_EndCdataSectionHandler end ));
void XMLCALL XML_SetStartCdataSectionHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartCdataSectionHandler start ));
void XMLCALL XML_SetEndCdataSectionHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_EndCdataSectionHandler end ));
void XMLCALL XML_SetDefaultHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_DefaultHandler handler ));
void XMLCALL XML_SetDefaultHandlerExpand ( REG(a0,XML_Parser parser ), REG(a1,XML_DefaultHandler handler ));
void XMLCALL XML_SetDoctypeDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartDoctypeDeclHandler start ), REG(a2,XML_EndDoctypeDeclHandler end));
void XMLCALL XML_SetStartDoctypeDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartDoctypeDeclHandler start ));
void XMLCALL XML_SetEndDoctypeDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_EndDoctypeDeclHandler end ));
void XMLCALL XML_SetUnparsedEntityDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_UnparsedEntityDeclHandler handler ));
void XMLCALL XML_SetNotationDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_NotationDeclHandler handler ));
void XMLCALL XML_SetNamespaceDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartNamespaceDeclHandler start ), REG (a2,XML_EndNamespaceDeclHandler end ));
void XMLCALL XML_SetStartNamespaceDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_StartNamespaceDeclHandler start ));
void XMLCALL XML_SetEndNamespaceDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_EndNamespaceDeclHandler end ));
void XMLCALL XML_SetNotStandaloneHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_NotStandaloneHandler handler ));
void XMLCALL XML_SetExternalEntityRefHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_ExternalEntityRefHandler handler ));
void XMLCALL XML_SetExternalEntityRefHandlerArg ( REG(a0,XML_Parser parser ), REG(a1,void *arg ));
void XMLCALL XML_SetSkippedEntityHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_SkippedEntityHandler handler ));
void XMLCALL XML_SetUnknownEncodingHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_UnknownEncodingHandler handler ), REG (a2,void *data ));
void XMLCALL XML_SetElementDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_ElementDeclHandler eldecl ));
void XMLCALL XML_SetAttlistDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_AttlistDeclHandler attdecl ));
void XMLCALL XML_SetEntityDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_EntityDeclHandler handler ));
void XMLCALL XML_SetXmlDeclHandler ( REG(a0,XML_Parser parser ), REG(a1,XML_XmlDeclHandler handler ));
int XMLCALL XML_SetParamEntityParsing ( REG(a0,XML_Parser parser ), REG (d0,enum XML_ParamEntityParsing peParsing ));
enum XML_Status XMLCALL XML_Parse ( REG(a0,XML_Parser parser ), REG(a1,const char *s ), REG (d0,int len ), REG (d1,int isFinal ));
enum XML_Status XMLCALL XML_ParseBuffer ( REG(a0,XML_Parser parser ), REG (d0,int len ), REG (d1,int isFinal ));
void *XMLCALL XML_GetBuffer ( REG(a0,XML_Parser parser ), REG (d0,int len ));
enum XML_Status XMLCALL XML_StopParser ( REG(a0,XML_Parser parser ), REG (d0,XML_Bool resumable ));
enum XML_Status XMLCALL XML_ResumeParser ( REG(a0,XML_Parser parser ));
void XMLCALL XML_GetParsingStatus ( REG(a0,XML_Parser parser ), REG(a1,XML_ParsingStatus *status ));
enum XML_Error XMLCALL XML_GetErrorCode ( REG(a0,XML_Parser parser ));
long XMLCALL XML_GetCurrentByteIndex ( REG(a0,XML_Parser parser ));
int XMLCALL XML_GetCurrentByteCount ( REG(a0,XML_Parser parser ));
const char *XMLCALL XML_GetInputContext ( REG(a0,XML_Parser parser ), REG(a1,int *offset ), REG (a2,int *size ));
XML_Size XMLCALL XML_GetCurrentLineNumber ( REG(a0,XML_Parser parser ));
XML_Size XMLCALL XML_GetCurrentColumnNumber ( REG(a0,XML_Parser parser ));
void XMLCALL XML_FreeContentModel ( REG(a0,XML_Parser parser ), REG(a1,XML_Content *model ));
void *XMLCALL XML_MemMalloc ( REG(a0,XML_Parser parser ), REG (d0,size_t size ));
void *XMLCALL XML_MemRealloc ( REG(a0,XML_Parser parser ), REG(a1,void *ptr ), REG (d0,size_t size ));
void XMLCALL XML_MemFree ( REG(a0,XML_Parser parser ), REG(a1,void *ptr ));
void XMLCALL XML_DefaultCurrent ( REG(a0,XML_Parser parser ));
const XML_LChar *XMLCALL XML_ErrorString ( REG (d0,enum XML_Error code ));
const XML_LChar *XMLCALL XML_ExpatVersion ( void );
XML_Expat_Version XMLCALL XML_ExpatVersionInfo ( void );
const XML_Feature *XMLCALL XML_GetFeatureList ( void );

#ifdef __MORPHOS__

XML_Parser XML_ParserCreate_SYSV ( const XML_Char *encodingName );
XML_Parser XML_ParserCreateNS_SYSV ( const XML_Char *encodingName , XML_Char nsSep );
XML_Parser XML_ParserCreate_MM_SYSV ( const XML_Char *encodingName , const XML_Memory_Handling_Suite *memsuite , const XML_Char *nameSep );

/* mosstubs.c */
XML_Parser LIB_XML_ParserCreate ( void );
XML_Parser LIB_XML_ParserCreateNS ( void );
XML_Parser LIB_XML_ParserCreate_MM ( void );
XML_Bool LIB_XML_ParserReset ( void );
enum XML_Status LIB_XML_SetEncoding ( void );
XML_Parser LIB_XML_ExternalEntityParserCreate ( void );
void LIB_XML_ParserFree ( void );
void LIB_XML_UseParserAsHandlerArg ( void );
enum XML_Error LIB_XML_UseForeignDTD ( void );
void LIB_XML_SetReturnNSTriplet ( void );
void LIB_XML_SetUserData ( void );
enum XML_Status LIB_XML_SetBase ( void );
const XML_Char *LIB_XML_GetBase ( void );
int LIB_XML_GetSpecifiedAttributeCount ( void );
int LIB_XML_GetIdAttributeIndex ( void );
void LIB_XML_SetElementHandler ( void );
void LIB_XML_SetStartElementHandler ( void );
void LIB_XML_SetEndElementHandler ( void );
void LIB_XML_SetCharacterDataHandler ( void );
void LIB_XML_SetProcessingInstructionHandler ( void );
void LIB_XML_SetCommentHandler ( void );
void LIB_XML_SetCdataSectionHandler ( void );
void LIB_XML_SetStartCdataSectionHandler ( void );
void LIB_XML_SetEndCdataSectionHandler ( void );
void LIB_XML_SetDefaultHandler ( void );
void LIB_XML_SetDefaultHandlerExpand ( void );
void LIB_XML_SetDoctypeDeclHandler ( void );
void LIB_XML_SetStartDoctypeDeclHandler ( void );
void LIB_XML_SetEndDoctypeDeclHandler ( void );
void LIB_XML_SetUnparsedEntityDeclHandler ( void );
void LIB_XML_SetNotationDeclHandler ( void );
void LIB_XML_SetNamespaceDeclHandler ( void );
void LIB_XML_SetStartNamespaceDeclHandler ( void );
void LIB_XML_SetEndNamespaceDeclHandler ( void );
void LIB_XML_SetNotStandaloneHandler ( void );
void LIB_XML_SetExternalEntityRefHandler ( void );
void LIB_XML_SetExternalEntityRefHandlerArg ( void );
void LIB_XML_SetSkippedEntityHandler ( void );
void LIB_XML_SetUnknownEncodingHandler ( void );
void LIB_XML_SetElementDeclHandler ( void );
void LIB_XML_SetAttlistDeclHandler ( void );
void LIB_XML_SetEntityDeclHandler ( void );
void LIB_XML_SetXmlDeclHandler ( void );
int LIB_XML_SetParamEntityParsing ( void );
enum XML_Status LIB_XML_Parse ( void );
enum XML_Status LIB_XML_ParseBuffer ( void );
void *LIB_XML_GetBuffer ( void );
enum XML_Status LIB_XML_StopParser ( void );
enum XML_Status LIB_XML_ResumeParser ( void );
void LIB_XML_GetParsingStatus ( void );
enum XML_Error LIB_XML_GetErrorCode ( void );
long LIB_XML_GetCurrentByteIndex ( void );
int LIB_XML_GetCurrentByteCount ( void );
const char *LIB_XML_GetInputContext ( void );
int LIB_XML_GetCurrentLineNumber ( void );
int LIB_XML_GetCurrentColumnNumber ( void );
void LIB_XML_FreeContentModel ( void );
void *LIB_XML_MemMalloc ( void );
void *LIB_XML_MemRealloc ( void );
void LIB_XML_MemFree ( void );
void LIB_XML_DefaultCurrent ( void );
const XML_LChar *LIB_XML_ErrorString ( void );
const XML_LChar *LIB_XML_ExpatVersion ( void );
XML_Expat_Version LIB_XML_ExpatVersionInfo ( void );
const XML_Feature *LIB_XML_GetFeatureList ( void );
#endif

