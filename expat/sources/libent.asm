
        SECTION expat.library,CODE

        NOLIST

        INCLUDE "expat.library_rev.i"
        INCLUDE "exec/libraries.i"
        INCLUDE "exec/resident.i"
        INCLUDE "exec/initializers.i"
        INCLUDE "exec/lists.i"
        INCLUDE "exec/semaphores.i"
        INCLUDE "utility/hooks.i"

        LIST

        XREF    _LinkerDB
        XREF    _lib_name
        XREF    ENDCODE
        XDEF    _ID

        XREF    _openLib
        XREF    _closeLib
        XREF    _expungeLib
        XREF    _initLib

	XREF	_XML_ParserCreate,
	XREF	_XML_ParserCreateNS,
	XREF	_XML_ParserCreate_MM,
	XREF	_XML_ExternalEntityParserCreate,
	XREF	_XML_ParserFree,
	XREF	_XML_Parse,
	XREF	_XML_ParseBuffer,
	XREF	_XML_GetBuffer,
	XREF	_XML_SetStartElementHandler,
	XREF	_XML_SetEndElementHandler,
	XREF	_XML_SetElementHandler,
	XREF	_XML_SetCharacterDataHandler,
	XREF	_XML_SetProcessingInstructionHandler,
	XREF	_XML_SetCommentHandler,
	XREF	_XML_SetStartCdataSectionHandler,
	XREF	_XML_SetEndCdataSectionHandler,
	XREF	_XML_SetCdataSectionHandler,
	XREF	_XML_SetDefaultHandler,
	XREF	_XML_SetDefaultHandlerExpand,
	XREF	_XML_SetExternalEntityRefHandler,
	XREF	_XML_SetExternalEntityRefHandlerArg,
	XREF	_XML_SetUnknownEncodingHandler,
	XREF	_XML_SetStartNamespaceDeclHandler,
	XREF	_XML_SetEndNamespaceDeclHandler,
	XREF	_XML_SetNamespaceDeclHandler,
	XREF	_XML_SetXmlDeclHandler,
	XREF	_XML_SetStartDoctypeDeclHandler,
	XREF	_XML_SetEndDoctypeDeclHandler,
	XREF	_XML_SetDoctypeDeclHandler,
	XREF	_XML_SetElementDeclHandler,
	XREF	_XML_SetAttlistDeclHandler,
	XREF	_XML_SetEntityDeclHandler,
	XREF	_XML_SetUnparsedEntityDeclHandler,
	XREF	_XML_SetNotationDeclHandler,
	XREF	_XML_SetNotStandaloneHandler,
	XREF	_XML_GetErrorCode,
	XREF	_XML_ErrorString,
	XREF	_XML_GetCurrentByteIndex,
	XREF	_XML_GetCurrentLineNumber,
	XREF	_XML_GetCurrentColumnNumber,
	XREF	_XML_GetCurrentByteCount,
	XREF	_XML_GetInputContext,
	XREF	_XML_SetUserData,
	XREF	_XML_DefaultCurrent,
	XREF	_XML_UseParserAsHandlerArg,
	XREF	_XML_SetBase,
	XREF	_XML_GetBase,
	XREF	_XML_GetSpecifiedAttributeCount,
	XREF	_XML_GetIdAttributeIndex,
	XREF	_XML_SetEncoding,
	XREF	_XML_SetParamEntityParsing,
	XREF	_XML_SetReturnNSTriplet,
	XREF	_XML_ExpatVersion,
	XREF	_XML_ExpatVersionInfo,
	XREF	_XML_ParserReset,
	XREF	_XML_SetSkippedEntityHandler,
	XREF	_XML_UseForeignDTD,
	XREF	_XML_GetFeatureList,
	XREF	_XML_StopParser,
	XREF	_XML_ResumeParser,
	XREF	_XML_GetParsingStatus,
	XREF	_XML_FreeContentModel,
	XREF	_XML_MemMalloc,
	XREF	_XML_MemRealloc,
	XREF	_XML_MemFree,

PRI     EQU 0

start:  moveq   #-1,d0
        rts

romtag:
        dc.w    RTC_MATCHWORD
        dc.l    romtag
        dc.l    ENDCODE
        dc.b    RTF_AUTOINIT
        dc.b    VERSION
        dc.b    NT_LIBRARY
        dc.b    PRI
        dc.l    _lib_name
        dc.l    _ID
        dc.l    init

_ID:    VSTRING

        CNOP    0,4

init:   dc.l    LIB_SIZE
        dc.l    funcTable
        dc.l    dataTable
        dc.l    _initLib

V_DEF   MACRO
    dc.w    \1+(*-funcTable)
    ENDM

funcTable:
        dc.l   _openLib
        dc.l   _closeLib
        dc.l   _expungeLib
        dc.l   nil

	dc.l   _XML_ParserCreate,
	dc.l   _XML_ParserCreateNS,
	dc.l   _XML_ParserCreate_MM,
	dc.l   _XML_ExternalEntityParserCreate,
	dc.l   _XML_ParserFree,
	dc.l   _XML_Parse,
	dc.l   _XML_ParseBuffer,
	dc.l   _XML_GetBuffer,
	dc.l   _XML_SetStartElementHandler,
	dc.l   _XML_SetEndElementHandler,
	dc.l   _XML_SetElementHandler,
	dc.l   _XML_SetCharacterDataHandler,
	dc.l   _XML_SetProcessingInstructionHandler,
	dc.l   _XML_SetCommentHandler,
	dc.l   _XML_SetStartCdataSectionHandler,
	dc.l   _XML_SetEndCdataSectionHandler,
	dc.l   _XML_SetCdataSectionHandler,
	dc.l   _XML_SetDefaultHandler,
	dc.l   _XML_SetDefaultHandlerExpand,
	dc.l   _XML_SetExternalEntityRefHandler,
	dc.l   _XML_SetExternalEntityRefHandlerArg,
	dc.l   _XML_SetUnknownEncodingHandler,
	dc.l   _XML_SetStartNamespaceDeclHandler,
	dc.l   _XML_SetEndNamespaceDeclHandler,
	dc.l   _XML_SetNamespaceDeclHandler,
	dc.l   _XML_SetXmlDeclHandler,
	dc.l   _XML_SetStartDoctypeDeclHandler,
	dc.l   _XML_SetEndDoctypeDeclHandler,
	dc.l   _XML_SetDoctypeDeclHandler,
	dc.l   _XML_SetElementDeclHandler,
	dc.l   _XML_SetAttlistDeclHandler,
	dc.l   _XML_SetEntityDeclHandler,
	dc.l   _XML_SetUnparsedEntityDeclHandler,
	dc.l   _XML_SetNotationDeclHandler,
	dc.l   _XML_SetNotStandaloneHandler,
	dc.l   _XML_GetErrorCode,
	dc.l   _XML_ErrorString,
	dc.l   _XML_GetCurrentByteIndex,
	dc.l   _XML_GetCurrentLineNumber,
	dc.l   _XML_GetCurrentColumnNumber,
	dc.l   _XML_GetCurrentByteCount,
	dc.l   _XML_GetInputContext,
	dc.l   _XML_SetUserData,
	dc.l   _XML_DefaultCurrent,
	dc.l   _XML_UseParserAsHandlerArg,
	dc.l   _XML_SetBase,
	dc.l   _XML_GetBase,
	dc.l   _XML_GetSpecifiedAttributeCount,
	dc.l   _XML_GetIdAttributeIndex,
	dc.l   _XML_SetEncoding,
	dc.l   _XML_SetParamEntityParsing,
	dc.l   _XML_SetReturnNSTriplet,
	dc.l   _XML_ExpatVersion,
	dc.l   _XML_ExpatVersionInfo,
	dc.l   _XML_ParserReset,
	dc.l   _XML_SetSkippedEntityHandler,
	dc.l   _XML_UseForeignDTD,
	dc.l   _XML_GetFeatureList,
	dc.l   _XML_StopParser,
	dc.l   _XML_ResumeParser,
	dc.l   _XML_GetParsingStatus,
	dc.l   _XML_FreeContentModel,
	dc.l   _XML_MemMalloc,
	dc.l   _XML_MemRealloc,
	dc.l   _XML_MemFree,
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil

        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil
        dc.l   nil

        dc.l   -1

dataTable:
        INITBYTE LN_TYPE,NT_LIBRARY
        INITLONG LN_NAME,_lib_name
        INITBYTE LIB_FLAGS,(LIBF_SUMUSED!LIBF_CHANGED)
        INITWORD LIB_VERSION,VERSION
        INITWORD LIB_REVISION,REVISION
        INITLONG LIB_IDSTRING,_ID
        dc.w     0

        CNOP    0,4

nil:    moveq   #0,d0
        rts

        END
