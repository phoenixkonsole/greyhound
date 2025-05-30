/*
**  Copyright (c) 1998, 1999, 2000 Thai Open Source Software Center Ltd
**  See the file COPYING for copying permission.
**
**  Written by Alfonso [alfie] Ranieri <alforan@tin.it>.
**
**  Released under the terms of the
**  GNU Public Licence version 2.
*/

#define __NOLIBBASE__
#define __USE_SYSBASE

#ifndef AMIGACONFIG_H
#define AMIGACONFIG_H

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include <string.h>
#include <stdlib.h>

/***************************************************************************/

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
#define BYTEORDER 4321

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* Define to 1 if you have the <check.h> header file. */
/* #undef HAVE_CHECK_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you hawe the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "expat-bugs@mail.libexpat.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "expat"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "expat 2.0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "expat"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* whether byteorder is bigendian */
#define WORDS_BIGENDIAN 1

/* Define to specify how much context to retain around the current parse
   point. */
#define XML_CONTEXT_BYTES 1024

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD 1

/* Define to make XML Namespaces functionality available. */
#define XML_NS 1

/***************************************************************************/

#undef SAVEDS
#undef ASM
#undef REGARGS
#undef STDARGS
#undef INLINE
#undef REG
#undef REGARRAY
#undef XMLCALL

#ifdef __MORPHOS__
	#define SAVEDS   __saveds
	#define ASM
	#define REGARGS
	#define STDARGS
	#define INLINE   __inline
	#define REG(x,p) p
	#define REGARRAY
	#define XMLCALL
#elif defined( __SASC)
	#define SAVEDS   __saveds
	#define ASM      __asm
	#define REGARGS  __regargs
	#define STDARGS  __stdargs
	#define INLINE   __inline
	#define REG(x,p) register __ ## x p
	#define REGARRAY register
	#define __attribute(a)
	#define XMLCALL SAVEDS ASM
#elif defined(__GNUC__)
	#define SAVEDS   __saveds
	#define ASM      
	#define REGARGS  __regargs
	#define STDARGS  __stdargs
	#define INLINE   __inline
	#define REG(x,p) register p __asm(#x)
	#define REGARRAY register
	#define __attribute(a)
	#define XMLCALL  SAVEDS 
#endif

/***************************************************************************/

extern UBYTE                    lib_name[];
extern UBYTE                    lib_ver[];
extern UBYTE                    lib_vrstring[];
extern ULONG                    lib_version;
extern ULONG                    lib_revision;

extern struct ExecBase          *SysBase;
extern struct DosLibrary        *DOSBase;
extern struct Library           *UtilityBase;
extern struct Library           *LocaleBase;

extern struct Library           *lib_base;
extern ULONG                    lib_segList;
extern struct SignalSemaphore   lib_sem;
extern struct SignalSemaphore   lib_poolSem;
extern APTR                     lib_pool;
extern struct Catalog	        *lib_cat;
extern ULONG                    lib_flags;

enum
{
    BASEFLG_Init = 1<<0,
};

/***************************************************************************/

#ifdef __MORPHOS__

#define CALLPKT1(sysv,func,p0)        			    ((sysv) ? (ULONG)(*func)(p0)       	     		    : callPkt(func,1,(ULONG)(p0)))
#define CALLPKT2(sysv,func,p0,p1)     			    ((sysv) ? (ULONG)(*func)(p0,p1)    	     		    : callPkt(func,2,(ULONG)(p0),(ULONG)(p1)))
#define CALLPKT3(sysv,func,p0,p1,p2)  			    ((sysv) ? (ULONG)(*func)(p0,p1,p2) 			    : callPkt(func,3,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2)))
#define CALLPKT4(sysv,func,p0,p1,p2,p3)  		    ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3) 		    : callPkt(func,4,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3)))
#define CALLPKT5(sysv,func,p0,p1,p2,p3,p4)  		    ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3,p4) 		    : callPkt(func,5,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4)))
#define CALLPKT6(sysv,func,p0,p1,p2,p3,p4,p5)  		    ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3,p4,p5) 	    : callPkt(func,6,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5)))
#define CALLPKT7(sysv,func,p0,p1,p2,p3,p4,p5,p6)  	    ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3,p4,p5,p6) 	    : callPkt(func,7,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6)))
#define CALLPKT8(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7)  	    ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3,p4,p5,p6,p7) 	    : callPkt(func,8,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6),(ULONG)(p7)))
#define CALLPKT9(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7,p8)      ((sysv) ? (ULONG)(*func)(p0,p1,p2,p3,p4,p5,p6,p7,p8)    : callPkt(func,9,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6),(ULONG)(p7),(ULONG)(p8)))

#define NCALLPKT1(sysv,func,p0)        			     ((sysv) ? (*func)(p0)             		      : (void)callPkt(func,1,(ULONG)(p0)))
#define NCALLPKT2(sysv,func,p0,p1)     			     ((sysv) ? (*func)(p0,p1)          		      : (void)callPkt(func,2,(ULONG)(p0),(ULONG)(p1)))
#define NCALLPKT3(sysv,func,p0,p1,p2)  			     ((sysv) ? (*func)(p0,p1,p2) 		      : (void)callPkt(func,3,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2)))
#define NCALLPKT4(sysv,func,p0,p1,p2,p3)  		     ((sysv) ? (*func)(p0,p1,p2,p3) 		      : (void)callPkt(func,4,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3)))
#define NCALLPKT5(sysv,func,p0,p1,p2,p3,p4)  		     ((sysv) ? (*func)(p0,p1,p2,p3,p4) 		      : (void)callPkt(func,5,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4)))
#define NCALLPKT6(sysv,func,p0,p1,p2,p3,p4,p5)  	     ((sysv) ? (*func)(p0,p1,p2,p3,p4,p5) 	      : (void)callPkt(func,6,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5)))
#define NCALLPKT7(sysv,func,p0,p1,p2,p3,p4,p5,p6)  	     ((sysv) ? (*func)(p0,p1,p2,p3,p4,p5,p6) 	      : (void)callPkt(func,7,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6)))
#define NCALLPKT8(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7)  	     ((sysv) ? (*func)(p0,p1,p2,p3,p4,p5,p6,p7)       : (void)callPkt(func,8,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6),(ULONG)(p7)))
#define NCALLPKT9(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7,p8)      ((sysv) ? (*func)(p0,p1,p2,p3,p4,p5,p6,p7,p8)    : (void)callPkt(func,9,(ULONG)(p0),(ULONG)(p1),(ULONG)(p2),(ULONG)(p3),(ULONG)(p4),(ULONG)(p5),(ULONG)(p6),(ULONG)(p7),(ULONG)(p8)))

#else

#define CALLPKT1(sysv,func,p0)        			    (*func)(p0)
#define CALLPKT2(sysv,func,p0,p1)     			    (*func)(p0,p1)
#define CALLPKT3(sysv,func,p0,p1,p2)  			    (*func)(p0,p1,p2)
#define CALLPKT4(sysv,func,p0,p1,p2,p3)  		    (*func)(p0,p1,p2,p3)
#define CALLPKT5(sysv,func,p0,p1,p2,p3,p4)  		    (*func)(p0,p1,p2,p3,p4)
#define CALLPKT6(sysv,func,p0,p1,p2,p3,p4,p5)  		    (*func)(p0,p1,p2,p3,p4,p5)
#define CALLPKT7(sysv,func,p0,p1,p2,p3,p4,p5,p6)  	    (*func)(p0,p1,p2,p3,p4,p5,p6)
#define CALLPKT8(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7)  	    (*func)(p0,p1,p2,p3,p4,p5,p6,p7)
#define CALLPKT9(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7,p8)      (*func)(p0,p1,p2,p3,p4,p5,p6,p7,p8)

#define NCALLPKT1(sysv,func,p0)                             (*func)(p0)                         
#define NCALLPKT2(sysv,func,p0,p1)                          (*func)(p0,p1)                      
#define NCALLPKT3(sysv,func,p0,p1,p2)                       (*func)(p0,p1,p2)                   
#define NCALLPKT4(sysv,func,p0,p1,p2,p3)                    (*func)(p0,p1,p2,p3)                
#define NCALLPKT5(sysv,func,p0,p1,p2,p3,p4)                 (*func)(p0,p1,p2,p3,p4)             
#define NCALLPKT6(sysv,func,p0,p1,p2,p3,p4,p5)              (*func)(p0,p1,p2,p3,p4,p5)          
#define NCALLPKT7(sysv,func,p0,p1,p2,p3,p4,p5,p6)           (*func)(p0,p1,p2,p3,p4,p5,p6)       
#define NCALLPKT8(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7)        (*func)(p0,p1,p2,p3,p4,p5,p6,p7)    
#define NCALLPKT9(sysv,func,p0,p1,p2,p3,p4,p5,p6,p7,p8)     (*func)(p0,p1,p2,p3,p4,p5,p6,p7,p8) 

#endif

/***************************************************************************/

#include "expat.h"
#include "lib_protos.h"

/***************************************************************************/

#endif /* AMIGACONFIG_H */
