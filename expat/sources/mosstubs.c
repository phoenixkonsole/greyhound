/*
**  Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
**  See the file COPYING for copying permission.
**
**  Written by Alfonso [alfie] Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2.
*/

#include "amigaconfig.h"

XML_Parser LIB_XML_ParserCreate(void)
{
    return XML_ParserCreate((const XML_Char *)REG_A0);
}

XML_Parser LIB_XML_ParserCreateNS(void)
{
    return XML_ParserCreateNS((const XML_Char *)REG_A0,(XML_Char)REG_D0);
}

XML_Parser LIB_XML_ParserCreate_MM(void)
{
    return XML_ParserCreate_MM((const XML_Char *)REG_A0,
                    	       (const XML_Memory_Handling_Suite *)REG_A1,
	                       (const XML_Char *)REG_A2);
}

XML_Bool LIB_XML_ParserReset(void)
{
    return XML_ParserReset((XML_Parser)REG_A0,(const XML_Char *)REG_A1);
}

enum XML_Status LIB_XML_SetEncoding(void)
{
    return XML_SetEncoding((XML_Parser)REG_A0,(const XML_Char *)REG_A1);
}

XML_Parser LIB_XML_ExternalEntityParserCreate(void)
{
    return XML_ExternalEntityParserCreate((XML_Parser)REG_A0,
                               		  (const XML_Char *)REG_A1,
                               		  (const XML_Char *)REG_A2);
}

void LIB_XML_ParserFree(void)
{
    XML_ParserFree((XML_Parser)REG_A0);
}

void LIB_XML_UseParserAsHandlerArg(void)
{
    XML_UseParserAsHandlerArg((XML_Parser)REG_A0);
}

enum XML_Error LIB_XML_UseForeignDTD(void)
{
    return XML_UseForeignDTD((XML_Parser)REG_A0,(XML_Bool)REG_D0);
}

void LIB_XML_SetReturnNSTriplet(void)
{
    XML_SetReturnNSTriplet((XML_Parser)REG_A0,(int)REG_D0);
}

void LIB_XML_SetUserData(void)
{
    XML_SetUserData((XML_Parser)REG_A0,(void *)REG_A1);
}

enum XML_Status LIB_XML_SetBase(void)
{
    return XML_SetBase((XML_Parser)REG_A0,(const XML_Char *)REG_A1);
}

const XML_Char * LIB_XML_GetBase(void)
{
    return XML_GetBase((XML_Parser)REG_A0);
}

int LIB_XML_GetSpecifiedAttributeCount(void)
{
    return XML_GetSpecifiedAttributeCount((XML_Parser)REG_A0);
}

int LIB_XML_GetIdAttributeIndex(void)
{
    return XML_GetIdAttributeIndex((XML_Parser)REG_A0);
}

void LIB_XML_SetElementHandler(void)
{
    XML_SetElementHandler((XML_Parser)REG_A0,(XML_StartElementHandler)REG_A1,(XML_EndElementHandler)REG_A2);
}

void LIB_XML_SetStartElementHandler(void)
{
    XML_SetStartElementHandler((XML_Parser)REG_A0,(XML_StartElementHandler)REG_A1);
}

void LIB_XML_SetEndElementHandler(void)
{
    XML_SetEndElementHandler((XML_Parser)REG_A0,(XML_EndElementHandler)REG_A1);
}

void LIB_XML_SetCharacterDataHandler(void)
{
    XML_SetCharacterDataHandler((XML_Parser)REG_A0,(XML_CharacterDataHandler)REG_A1);
}

void LIB_XML_SetProcessingInstructionHandler(void)
{
    XML_SetProcessingInstructionHandler((XML_Parser)REG_A0,(XML_ProcessingInstructionHandler)REG_A1);
}

void LIB_XML_SetCommentHandler(void)
{
    XML_SetCommentHandler((XML_Parser)REG_A0,(XML_CommentHandler)REG_A1);
}

void LIB_XML_SetCdataSectionHandler(void)
{
    return XML_SetCdataSectionHandler((XML_Parser)REG_A0,
    		                      (XML_StartCdataSectionHandler)REG_A1,
                           	      (XML_EndCdataSectionHandler)REG_A2);
}

void LIB_XML_SetStartCdataSectionHandler(void)
{
    XML_SetStartCdataSectionHandler((XML_Parser)REG_A0,(XML_StartCdataSectionHandler)REG_A1);
}

void LIB_XML_SetEndCdataSectionHandler(void)
{
    XML_SetEndCdataSectionHandler((XML_Parser)REG_A0,(XML_EndCdataSectionHandler)REG_A1);
}

void LIB_XML_SetDefaultHandler(void)
{
    XML_SetDefaultHandler((XML_Parser)REG_A0,(XML_DefaultHandler)REG_A1);
}

void LIB_XML_SetDefaultHandlerExpand(void)
{
    XML_SetDefaultHandlerExpand((XML_Parser)REG_A0,(XML_DefaultHandler)REG_A1);
}

void LIB_XML_SetDoctypeDeclHandler(void)
{
    XML_SetDoctypeDeclHandler((XML_Parser)REG_A0,
    			      (XML_StartDoctypeDeclHandler)REG_A1,
                              (XML_EndDoctypeDeclHandler)REG_A2);
}

void LIB_XML_SetStartDoctypeDeclHandler(void)
{
    XML_SetStartDoctypeDeclHandler((XML_Parser)REG_A0,(XML_StartDoctypeDeclHandler)REG_A1);
}

void LIB_XML_SetEndDoctypeDeclHandler(void)
{
    XML_SetEndDoctypeDeclHandler((XML_Parser)REG_A0,(XML_EndDoctypeDeclHandler)REG_A1);
}

void LIB_XML_SetUnparsedEntityDeclHandler(void)
{
    XML_SetUnparsedEntityDeclHandler((XML_Parser)REG_A0,(XML_UnparsedEntityDeclHandler)REG_A1);
}

void LIB_XML_SetNotationDeclHandler(void)
{
    XML_SetNotationDeclHandler((XML_Parser)REG_A0,(XML_NotationDeclHandler)REG_A1);
}

void LIB_XML_SetNamespaceDeclHandler(void)
{
    XML_SetNamespaceDeclHandler((XML_Parser)REG_A0,
    				(XML_StartNamespaceDeclHandler)REG_A1,
                                (XML_EndNamespaceDeclHandler)REG_A2);
}

void LIB_XML_SetStartNamespaceDeclHandler(void)
{
    XML_SetStartNamespaceDeclHandler((XML_Parser)REG_A0,(XML_StartNamespaceDeclHandler)REG_A1);
}

void LIB_XML_SetEndNamespaceDeclHandler(void)
{
    XML_SetEndNamespaceDeclHandler((XML_Parser)REG_A0,(XML_EndNamespaceDeclHandler)REG_A1);
}

void LIB_XML_SetNotStandaloneHandler(void)
{
    XML_SetNotStandaloneHandler((XML_Parser)REG_A0,(XML_NotStandaloneHandler)REG_A1);
}

void LIB_XML_SetExternalEntityRefHandler(void)
{
    XML_SetExternalEntityRefHandler((XML_Parser)REG_A0,(XML_ExternalEntityRefHandler)REG_A1);
}

void LIB_XML_SetExternalEntityRefHandlerArg(void)
{
    XML_SetExternalEntityRefHandlerArg((XML_Parser)REG_A0,(void *)REG_A1);
}

void LIB_XML_SetSkippedEntityHandler(void)
{
    XML_SetSkippedEntityHandler((XML_Parser)REG_A0,(XML_SkippedEntityHandler)REG_A1);
}

void LIB_XML_SetUnknownEncodingHandler(void)
{
    XML_SetUnknownEncodingHandler((XML_Parser)REG_A0,(XML_UnknownEncodingHandler)REG_A1,(void *)REG_A2);
}

void LIB_XML_SetElementDeclHandler(void)
{
    XML_SetElementDeclHandler((XML_Parser)REG_A0,(XML_ElementDeclHandler)REG_A1);
}

void LIB_XML_SetAttlistDeclHandler(void)
{
    XML_SetAttlistDeclHandler((XML_Parser)REG_A0,(XML_AttlistDeclHandler)REG_A1);
}

void LIB_XML_SetEntityDeclHandler(void)
{
    XML_SetEntityDeclHandler((XML_Parser)REG_A0,(XML_EntityDeclHandler)REG_A1);
}

void LIB_XML_SetXmlDeclHandler(void)
{
    XML_SetXmlDeclHandler((XML_Parser)REG_A0,(XML_XmlDeclHandler)REG_A1);
}

int LIB_XML_SetParamEntityParsing(void)
{
    return XML_SetParamEntityParsing((XML_Parser)REG_A0,(enum XML_ParamEntityParsing)REG_D0);
}

enum XML_Status LIB_XML_Parse(void)
{
    return XML_Parse((XML_Parser)REG_A0,(const char *)REG_A1,(int)REG_D0,(int)REG_D1);
}

enum XML_Status LIB_XML_ParseBuffer(void)
{
    return XML_ParseBuffer((XML_Parser)REG_A0,(int)REG_D0,(int)REG_D1);
}

void * LIB_XML_GetBuffer(void)
{
    return XML_GetBuffer((XML_Parser)REG_A0,(int)REG_D0);
}

enum XML_Status LIB_XML_StopParser(void)
{
    return XML_StopParser((XML_Parser)REG_A0,(XML_Bool)REG_D0);
}

enum XML_Status LIB_XML_ResumeParser(void)
{
    return XML_ResumeParser((XML_Parser)REG_A0);
}

void LIB_XML_GetParsingStatus(void)
{
    XML_GetParsingStatus((XML_Parser)REG_A0,(XML_ParsingStatus *)REG_A1);
}

enum XML_Error LIB_XML_GetErrorCode(void)
{
    return XML_GetErrorCode((XML_Parser)REG_A0);
}

long LIB_XML_GetCurrentByteIndex(void)
{
    return XML_GetCurrentByteIndex((XML_Parser)REG_A0);
}

int LIB_XML_GetCurrentByteCount(void)
{
    return XML_GetCurrentByteCount((XML_Parser)REG_A0);
}

const char * LIB_XML_GetInputContext(void)
{
    return XML_GetInputContext((XML_Parser)REG_A0,(int *)REG_A1,(int *)REG_A2);
}

int LIB_XML_GetCurrentLineNumber(void)
{
    return XML_GetCurrentLineNumber((XML_Parser)REG_A0);
}

int LIB_XML_GetCurrentColumnNumber(void)
{
    return XML_GetCurrentColumnNumber((XML_Parser)REG_A0);
}

void LIB_XML_FreeContentModel(void)
{
    XML_FreeContentModel((XML_Parser)REG_A0,(XML_Content *)REG_A1);
}

void * LIB_XML_MemMalloc(void)
{
    return XML_MemMalloc((XML_Parser)REG_A0,(size_t)REG_D0);
}

void * LIB_XML_MemRealloc(void)
{
    return XML_MemRealloc((XML_Parser)REG_A0,(void *)REG_A1,(size_t)REG_D0);
}

void LIB_XML_MemFree(void)
{
    XML_MemFree((XML_Parser)REG_A0,(void *)REG_A1);
}

void LIB_XML_DefaultCurrent(void)
{
    XML_DefaultCurrent((XML_Parser)REG_A0);
}

const XML_LChar * LIB_XML_ErrorString(void)
{
    return XML_ErrorString((enum XML_Error)REG_D0);
}

const XML_LChar * LIB_XML_ExpatVersion(void)
{
    return XML_ExpatVersion();
}

XML_Expat_Version LIB_XML_ExpatVersionInfo(void)
{
    return XML_ExpatVersionInfo();
}

const XML_Feature * LIB_XML_GetFeatureList(void)
{
    return XML_GetFeatureList();
}

