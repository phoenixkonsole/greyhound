/*
 * Copyright 2007 Daniel Silverstone <dsilvers@digital-scurf.org>
 * Copyright 2007 Rob Kendrick <rjek@greyhound-browser.org>
 *
 * This file is part of Greyhound, http://www.greyhound-browser.org/
 *
 * Greyhound is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Greyhound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/config.h"
#include "utils/utsname.h"
#include "desktop/greyhound.h"
#include "utils/log.h"
#include "utils/useragent.h"
#include "utils/nsoption.h"

char *core_user_agent_string = NULL;

#undef NETSURF_UA_FORMAT_STRING
#define NETSURF_UA_FORMAT_STRING "Greyhound/%d.%d (%s; %s)"
//#define NETSURF_UA_FORMAT_STRING "Android"

/**
 * Prepare core_user_agent_string with a string suitable for use as a
 * user agent in HTTP requests.
 */

void
user_agent_build_string(void)
{
        struct utsname un;
        const char *sysname = "Unknown";
        const char *machine = "Unknown";
        char *ua_string;
        int len;

        if (uname(&un) >= 0) {
                sysname = un.sysname;
                machine = un.machine;
        }

        len = snprintf(NULL, 0, NETSURF_UA_FORMAT_STRING,
                       greyhound_version_major,
                       greyhound_version_minor,
                       sysname,
                       machine);
        ua_string = malloc(len + 1);
        if (!ua_string) {
                /** \todo this needs handling better */
                return;
        }
        snprintf(ua_string, len + 1,
                 NETSURF_UA_FORMAT_STRING,
                 greyhound_version_major,
                 greyhound_version_minor,
                 sysname,
                 machine);

		if (nsoption_int(mobile_mode) == 1)
			core_user_agent_string = strdup("Android");
		else			 
			core_user_agent_string = ua_string;

        LOG(("Built user agent \"%s\"", core_user_agent_string));
}

/* This is a function so that later we can override it trivially */
const char *
user_agent_string(void)
{
        if (core_user_agent_string == NULL)
                user_agent_build_string();
	return core_user_agent_string;
}
