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
#include "expat.library_rev.h"
#include <exec/resident.h>


/****************************************************************************/
/*
** Globals
*/

UBYTE                  lib_name[] = PRG;
UBYTE                  lib_ver[] = VSTRING;
UBYTE                  lib_vrstring[] = VRSTRING;
ULONG                  lib_version = VERSION;
ULONG                  lib_revision = REVISION;

struct ExecBase        *SysBase = NULL;
struct DosLibrary      *DOSBase = NULL;
struct Library         *UtilityBase = NULL;
struct Library         *LocaleBase = NULL;

struct Library         *lib_base = NULL;
ULONG                  lib_segList = NULL;
struct SignalSemaphore lib_sem = { 0 };
struct SignalSemaphore lib_poolSem = { 0 };
APTR                   lib_pool = NULL;
struct Catalog         *lib_cat = NULL;
ULONG                  lib_flags = 0;

/****************************************************************************/
/*
** First function
*/


static ULONG nil(void)
{
        return -1;
}

/****************************************************************************/
/*
** Some declarations and protos
*/

extern const ULONG initTable[];
extern const APTR funcTable[];

#ifdef __MORPHOS__
static struct Library *initLib ( struct Library * , BPTR , struct ExecBase * );
static struct Library *openLib ( void );
static ULONG expungeLib ( void );
static ULONG closeLib ( void );
#else
struct Library *SAVEDS ASM initLib(REG(a0,ULONG segList),REG(a6,APTR sys),REG(d0,struct Library *base));
struct Library * SAVEDS ASM openLib(REG(a6,struct Library *base));
ULONG SAVEDS ASM expungeLib(REG(a6,struct Library *base));
ULONG SAVEDS ASM closeLib(REG(a6,struct Library *base));
#endif

/****************************************************************************/
/*
** Romtag
*/

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

struct dataTable
{
    UBYTE ln_Type_Init[4];
    UBYTE ln_Pri_Init[4];
    UBYTE ln_Name_Init[2];
    ULONG ln_Name_Content;
    UBYTE lib_Flags_Init[4];
    UBYTE lib_Version_Init[2];
    UWORD lib_Version_Content;
    UBYTE lib_Revision_Init[2];
    UWORD lib_Revision_Content;
    UBYTE lib_IdString_Init[2];
    ULONG lib_IdString_Content;
    UWORD EndMark;
};

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

extern const struct dataTable dataTable;

static const struct Resident romTag =
{
    RTC_MATCHWORD,
    (struct Resident *)&romTag,
    (struct Resident *)&romTag+1,
    #ifdef __MORPHOS__
    RTF_AUTOINIT|RTF_PPC|RTF_EXTENDED,
    #else
    RTF_AUTOINIT,
    #endif
    VERSION,
    NT_LIBRARY,
    0,
    (char *)lib_name,
    (char *)lib_ver,
    (APTR)initTable
    #ifdef __MORPHOS__
    ,REVISION,
    NULL
    #endif
};

/****************************************************************************/
/*
** Init table
*/

const ULONG initTable[] =
{
    sizeof(struct Library),
    (ULONG)funcTable,
    (ULONG)&dataTable,
    (ULONG)initLib
};

/****************************************************************************/
/*
** Data table
*/

const struct dataTable dataTable =
{
    0xa0,8,  NT_LIBRARY,0,
    0xa0,9,  0,0,
    0x80,10, (ULONG)lib_name,
    0xa0,14, LIBF_SUMUSED|LIBF_CHANGED,0,
    0x90,20, VERSION,
    0x90,22, REVISION,
    0x80,24, (ULONG)lib_ver,
    0
};

/****************************************************************************/
/*
** Vectors
*/

#ifdef __MORPHOS__
#define LIB(x) (APTR)LIB_ ## x
#else
#define LIB(x) (APTR)x
#endif

#define RESERVEDENTRY (APTR)nil

#define LIBVECTORS \
    LIB(XML_ParserCreate),\
    LIB(XML_ParserCreateNS),\
    LIB(XML_ParserCreate_MM),\
    LIB(XML_ExternalEntityParserCreate),\
    LIB(XML_ParserFree),\
    LIB(XML_Parse),\
    LIB(XML_ParseBuffer),\
    LIB(XML_GetBuffer),\
    LIB(XML_SetStartElementHandler),\
    LIB(XML_SetEndElementHandler),\
    LIB(XML_SetElementHandler),\
    LIB(XML_SetCharacterDataHandler),\
    LIB(XML_SetProcessingInstructionHandler),\
    LIB(XML_SetCommentHandler),\
    LIB(XML_SetStartCdataSectionHandler),\
    LIB(XML_SetEndCdataSectionHandler),\
    LIB(XML_SetCdataSectionHandler),\
    LIB(XML_SetDefaultHandler),\
    LIB(XML_SetDefaultHandlerExpand),\
    LIB(XML_SetExternalEntityRefHandler),\
    LIB(XML_SetExternalEntityRefHandlerArg),\
    LIB(XML_SetUnknownEncodingHandler),\
    LIB(XML_SetStartNamespaceDeclHandler),\
    LIB(XML_SetEndNamespaceDeclHandler),\
    LIB(XML_SetNamespaceDeclHandler),\
    LIB(XML_SetXmlDeclHandler),\
    LIB(XML_SetStartDoctypeDeclHandler),\
    LIB(XML_SetEndDoctypeDeclHandler),\
    LIB(XML_SetDoctypeDeclHandler),\
    LIB(XML_SetElementDeclHandler),\
    LIB(XML_SetAttlistDeclHandler),\
    LIB(XML_SetEntityDeclHandler),\
    LIB(XML_SetUnparsedEntityDeclHandler),\
    LIB(XML_SetNotationDeclHandler),\
    LIB(XML_SetNotStandaloneHandler),\
    LIB(XML_GetErrorCode),\
    LIB(XML_ErrorString),\
    LIB(XML_GetCurrentByteIndex),\
    LIB(XML_GetCurrentLineNumber),\
    LIB(XML_GetCurrentColumnNumber),\
    LIB(XML_GetCurrentByteCount),\
    LIB(XML_GetInputContext),\
    LIB(XML_SetUserData),\
    LIB(XML_DefaultCurrent),\
    LIB(XML_UseParserAsHandlerArg),\
    LIB(XML_SetBase),\
    LIB(XML_GetBase),\
    LIB(XML_GetSpecifiedAttributeCount),\
    LIB(XML_GetIdAttributeIndex),\
    LIB(XML_SetEncoding),\
    LIB(XML_SetParamEntityParsing),\
    LIB(XML_SetReturnNSTriplet),\
    LIB(XML_ExpatVersion),\
    LIB(XML_ExpatVersionInfo),\
    LIB(XML_ParserReset),\
    LIB(XML_SetSkippedEntityHandler),\
    LIB(XML_UseForeignDTD),\
    LIB(XML_GetFeatureList),\
    LIB(XML_StopParser),\
    LIB(XML_ResumeParser),\
    LIB(XML_GetParsingStatus),\
    LIB(XML_FreeContentModel),\
    LIB(XML_MemMalloc),\
    LIB(XML_MemRealloc),\
    LIB(XML_MemFree),\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY,\
    RESERVEDENTRY

/****************************************************************************/
/*
** Functions table
*/


const APTR funcTable[] =
{
    #ifdef __MORPHOS__
    (APTR)FUNCARRAY_BEGIN,
    (APTR)FUNCARRAY_32BIT_NATIVE,
    #endif

    (APTR)openLib,
    (APTR)closeLib,
    (APTR)expungeLib,
    (APTR)nil,
   LIBVECTORS,

    #ifdef __MORPHOS__
    (APTR)0xffffffff,
    (APTR)FUNCARRAY_32BIT_SYSTEMV,
    (APTR)XML_ParserCreate_SYSV,
    (APTR)XML_ParserCreateNS_SYSV,
    (APTR)XML_ParserCreate_MM_SYSV,
    (APTR)0xffffffff,
    (APTR)FUNCARRAY_END
    #else
    (APTR)(-1)
    #endif
};

#ifdef __MORPHOS__
const unsigned long __amigappc__ = 1;
const unsigned long __abox__     = 1;
#endif

 /****************************************************************************/
/*
** Init
*/

#ifdef __MORPHOS__
static struct Library *initLib(struct Library *base,BPTR segList,struct ExecBase *sys)
#else
struct Library *SAVEDS ASM initLib(REG(a0,ULONG segList),REG(a6,APTR sys),REG(d0,struct Library *base))
#endif
{
#define SysBase sys
    InitSemaphore(&lib_sem);
    InitSemaphore(&lib_poolSem);
#undef SysBase

    SysBase     = sys;
    lib_segList = segList;

    return lib_base = base;
}

/****************************************************************************/
/*
** Open
*/

#ifdef __MORPHOS__
static struct Library *openLib(void)
#else
struct Library * SAVEDS ASM openLib(REG(a6,struct Library *base))
#endif
{
#ifdef __MORPHOS__
    struct Library *base = (struct Library *)REG_A6;
#endif
    register struct Library *res;

    ObtainSemaphore(&lib_sem);

    base->lib_OpenCnt++;
    base->lib_Flags &= ~LIBF_DELEXP;

    if (!(lib_flags & BASEFLG_Init) && !initBase())
    {
        base->lib_OpenCnt--;
        res = NULL;
    }
    else res = base;

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/
/*
** Expunge
*/

#ifdef __MORPHOS__
static ULONG expungeLib(void)
#else
ULONG SAVEDS ASM expungeLib(REG(a6,struct Library *base))
#endif
{
#ifdef __MORPHOS__
    struct Library *base = (struct Library *)REG_A6;
#endif
    register ULONG res;

    ObtainSemaphore(&lib_sem);

    if (!base->lib_OpenCnt)
    {
        Remove((struct Node *)base);
        FreeMem((UBYTE *)base-base->lib_NegSize,base->lib_NegSize+base->lib_PosSize);

        res = lib_segList;
    }
    else
    {
        base->lib_Flags |= LIBF_DELEXP;
        res = NULL;
    }

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/
/*
** Close
*/

#ifdef __MORPHOS__
static ULONG closeLib(void)
#else
ULONG SAVEDS ASM closeLib(REG(a6,struct Library *base))
#endif
{
#ifdef __MORPHOS__
    struct Library *base = (struct Library *)REG_A6;
#endif
    register ULONG res = NULL;

    ObtainSemaphore(&lib_sem);

    if (!--base->lib_OpenCnt)
    {
        freeBase();

        if (base->lib_Flags & LIBF_DELEXP)
    {
            Remove((struct Node *)base);
            FreeMem((UBYTE *)base-base->lib_NegSize,base->lib_NegSize+base->lib_PosSize);

            res = lib_segList;
        }
    }

    ReleaseSemaphore(&lib_sem);

    return res;
}

/****************************************************************************/

