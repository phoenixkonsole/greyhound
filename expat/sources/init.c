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

/****************************************************************************/

void
freeBase(void)
{
    if (lib_pool)
    {
        DeletePool(lib_pool);
        lib_pool = NULL;
    }

    if (LocaleBase)
    {
        CloseLibrary(LocaleBase);
        LocaleBase = NULL;
    }

    if (UtilityBase)
    {
        CloseLibrary(UtilityBase);
        UtilityBase = NULL;
    }

    if (DOSBase)
    {
        CloseLibrary((struct Library *)DOSBase);
        DOSBase = NULL;
    }

    lib_flags &= ~BASEFLG_Init;
}

/***********************************************************************/

ULONG
initBase(void)
{
    if ((DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37)) &&
        (UtilityBase = OpenLibrary("utility.library",37)) &&
        (lib_pool = CreatePool(MEMF_ANY,4096,1024)))
    {
        lib_flags |= BASEFLG_Init;

	initStrings();

        return TRUE;
    }

    freeBase();

    return FALSE;
}

/***********************************************************************/
