/*
 * Copyright 2008 Daniel Silverstone <dsilvers@greyhound-browser.org>
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <proto/dos.h>

#include "utils/filepath.h"
#include "framebuffer/findfile.h"

char **respaths; /** resource search path vector */

/** Create an array of valid paths to search for resources.
 *
 * The idea is that all the complex path computation to find resources
 * is performed here, once, rather than every time a resource is
 * searched for.
 */
char **
fb_init_resource(const char *resource_path)
{
	char **pathv; /* resource path string vector */
	char **respath; /* resource paths vector */	
	const char *lang = NULL;

	pathv = filepath_path_to_strvec(resource_path);
	
	
	respath = filepath_generate(pathv, &lang);

	filepath_free_strvec(pathv);
	

	return respath;
}


/**
 * Return the filename part of a full path
 *
 * \param path full path and filename
 * \return filename (will be freed with free())
 */
static char *filename_from_path(char *path)
{
	char *leafname;

	leafname = strrchr(path, '/');
	if (!leafname)
		leafname = path;
	else
		leafname += 1;

	return strdup(leafname);
}

/**
 * Add a path component/filename to an existing path
 *
 * \param path buffer containing path + free space
 * \param length length of buffer "path"
 * \param newpart string containing path component to add to path
 * \return true on success
 */

static bool path_add_part(char *path, int length, const char *newpart)
{
	if(path[strlen(path) - 1] != '/')
		strncat(path, "/", length);

	strncat(path, newpart, length);

	return true;
}

/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */

