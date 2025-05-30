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
#include <stdarg.h>

#ifdef DEBUGMEM
ULONG g_tot = 0;
#endif

/***********************************************************************/

void __chkabort(void){}

/***********************************************************************/

APTR
allocVecPooled(APTR pool,ULONG size)
{
    register ULONG *mem;

    if (mem = AllocPooled(pool,size = size+sizeof(ULONG))) *mem++ = size;

    #ifdef DEBUGMEM
    g_tot += size;
    NewRawDoFmt("alloc %08lu %08lu\n",1,1,size+4,g_tot);
    #endif

    return mem;
}

/****************************************************************************/

void
freeVecPooled(APTR pool,APTR mem)
{
    #ifdef DEBUGMEM
    g_tot -= *((ULONG *)mem-1);
    NewRawDoFmt("free %08lu %08lu\n",1,1,*((ULONG *)mem-1),g_tot);
    #endif
    FreePooled(pool,(ULONG *)mem-1,*((ULONG *)mem-1));
}

/****************************************************************************/

APTR
allocArbitratePooled(ULONG size)
{
    if (size)
    {
        register APTR mem;

        ObtainSemaphore(&lib_poolSem);
        mem = AllocPooled(lib_pool,size);
        ReleaseSemaphore(&lib_poolSem);

        return mem;

    }

    return NULL;
}

/***********************************************************************/

void
freeArbitratePooled(APTR mem,ULONG size)
{
    if (mem && size)
    {
        ObtainSemaphore(&lib_poolSem);
        FreePooled(lib_pool,mem,size);
        ReleaseSemaphore(&lib_poolSem);
    }
}

/***********************************************************************/

APTR
allocArbitrateVecPooled(ULONG size)
{
    if (size)
    {
        register APTR mem;

        ObtainSemaphore(&lib_poolSem);
        mem = allocVecPooled(lib_pool,size);
        ReleaseSemaphore(&lib_poolSem);

        return mem;
    }

    return NULL;
}

/***********************************************************************/

void
freeArbitrateVecPooled(APTR mem)
{
    if (mem)
    {
        ObtainSemaphore(&lib_poolSem);
        freeVecPooled(lib_pool,mem);
        ReleaseSemaphore(&lib_poolSem);
    }
}

/***********************************************************************/

void *
palloc(size_t size)
{
    return allocArbitrateVecPooled(size);
}

/***********************************************************************/

void *
prealloc(void *ptr,size_t size)
{
    register APTR  new;
    register ULONG sold = 0; //sasc

    if (size==0) return NULL;

    if (ptr)
    {
        sold = *((ULONG *)ptr-1);

        if (sold-sizeof(ULONG)>=size) return ptr;
    }

    if (new = allocArbitrateVecPooled(size))
    {
        if (ptr) memcpy(new,ptr,sold);
    }

    /* Not unix comptabile! */
    if (ptr) freeArbitrateVecPooled(ptr);

    return new;
}

/***********************************************************************/

void
pfree(void *ptr)
{
    if (ptr) freeArbitrateVecPooled(ptr);
}

/***********************************************************************/

#ifdef __MORPHOS__
#include <emul/emulregs.h>

ULONG
callPkt(APTR func,ULONG numArgs,...)
{
    va_list va;
    ULONG   res;
    int	    i;

    va_start(va,numArgs);

    for (i = numArgs; i>0; i--)
	((ULONG *)REG_A7)[-i] = (ULONG)va_arg(va,ULONG);

    REG_A7 -= (numArgs)*4;

    if (*(UWORD *)func>=(UWORD)0xFF00) REG_A7 -= 4;

    res = MyEmulHandle->EmulCallDirect68k(func);

    if (*(UWORD *)func>=(UWORD)0xFF00) REG_A7 += 4;

    REG_A7 += (numArgs)*4;

    va_end(va);

    return res;
}
#endif

/***************************************************************************/

