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
#include <proto/locale.h>
#define CATCOMP_ARRAY
#include "loc.h"

/***********************************************************************/

void
initStrings(void)
{
    if ((LocaleBase = OpenLibrary("locale.library",36)) &&
        (lib_cat = OpenCatalogA(NULL,"expat.catalog",NULL)))
    {
        register struct CatCompArrayType *cca;
        register int                     cnt;

        for (cnt = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1, cca = (struct CatCompArrayType *)CatCompArray+cnt;
             cnt>=0;
             cnt--, cca--)
        {
            register STRPTR s;

            if (s = GetCatalogStr(lib_cat,cca->cca_ID,cca->cca_Str)) cca->cca_Str = s;
        }
    }
}

/***********************************************************************/

STRPTR
getString(ULONG id)
{
    register int low, high;

    for (low = 0, high = (sizeof(CatCompArray)/sizeof(struct CatCompArrayType))-1; low<=high; )
    {
        register int                     mid = (low+high)>>1, cond;
        register struct CatCompArrayType *cca = (struct CatCompArrayType *)CatCompArray+mid;

        if ((cond = id-cca->cca_ID)==0)
            return lib_cat ? GetCatalogStr(lib_cat,id,cca->cca_Str) : cca->cca_Str;

        if (cond<0) high = mid-1;
        else low = mid+1;
    }

    return (STRPTR)"";
}

/***********************************************************************/
