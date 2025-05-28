/*
 * Copyright 2006 Richard Wilson <info@tinct.net>
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

/** \file
 * Provides a central method of obtaining unique filenames.
 *
 * A maximum of 2^24 files can be allocated at any point in time.
 */
#include <proto/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h> // Für FileInfoBlock
#include <assert.h>
#include <sys/types.h>
#include <proto/dos.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <exec/memory.h>
#include <proto/exec.h>


#include "utils/config.h"
#include "utils/filename.h"
#include "utils/log.h"
#include "utils/utils.h"

#define FULL_WORD (unsigned int)0xffffffffu
#define START_PREFIX ('0' + '0' * 10)

struct directory {
	int numeric_prefix;		/** numeric representation of prefix */
	char prefix[10];		/** directory prefix, eg '00/11/52/' */
	unsigned int low_used;		/** first 32 files, 1 bit per file */
	unsigned int high_used;		/** last 32 files, 1 bit per file */
	struct directory *next;		/** next directory (sorted by prefix) */
};


static struct directory *root = NULL;
static char filename_buffer[12];
static char filename_directory[256];

static struct directory *filename_create_directory(const char *prefix);
static bool filename_flush_directory(const char *folder, int depth);
static bool filename_delete_recursive(char *folder);

/**
 * Request a new, unique, filename.
 *
 * \return a pointer to a shared buffer containing the new filename,
 *         NULL on failure
 */
const char *filename_request(void)
{
	struct directory *dir;
	int i = -1;

	for (dir = root; dir; dir = dir->next) {
		if ((dir->low_used & dir->high_used) != FULL_WORD) {
			if (dir->low_used != FULL_WORD) {
				for (i = 0; (dir->low_used & (1 << i)); i++);
			} else {
				for (i = 0; (dir->high_used & (1 << i)); i++);
				i += 32;
			}
			break;
		}
	}

	if (i == -1) {
		/* no available slots - create a new directory */
		dir = filename_create_directory(NULL);
		if (dir == NULL) {
			LOG(("Failed to create a new directory."));
			return NULL;
		}
		i = 63;
	}

	if (i < 32)
		dir->low_used |= (1 << i);
	else
		dir->high_used |= (1 << (i - 32));

	sprintf(filename_buffer, "%s%.2i", dir->prefix, i);

	return filename_buffer;
}


/**
 * Claim a specific filename.
 *
 * \param  filename  the filename to claim
 * \return whether the claim was successful
 */
bool filename_claim(const char *filename)
{
	char dir_prefix[9];
	int file;
	struct directory *dir;

	/* filename format is always '01/23/45/XX' */
	strncpy(dir_prefix, filename, 9);
	dir_prefix[8] = '\0';
	file = (filename[10] + filename[9] * 10 - START_PREFIX);

	/* create the directory */
	dir = filename_create_directory(dir_prefix);
	if (dir == NULL)
		return false;

	/* update the entry */
	if (file < 32) {
		if (dir->low_used & (1 << file))
			return false;
		dir->low_used |= (1 << file);
	} else {
		if (dir->high_used & (1 << (file - 32)))
			return false;
		dir->high_used |= (1 << (file - 32));
	}

	return true;
}


/**
 * Releases a filename for future use.
 *
 * \param  filename  the filename to release
 */
void filename_release(const char *filename)
{
	struct directory *dir;
	int index, file;

	/* filename format is always '01/23/45/XX' */
	index = ((filename[7] + filename[6] * 10 - START_PREFIX) |
		((filename[4] + filename[3] * 10 - START_PREFIX) << 6) |
		((filename[1] + filename[0] * 10 - START_PREFIX) << 12));
	file = (filename[10] + filename[9] * 10 - START_PREFIX);

	/* modify the correct directory entry */
	for (dir = root; dir; dir = dir->next) {
		if (dir->numeric_prefix == index) {
			if (file < 32)
				dir->low_used &= ~(1 << file);
			else
				dir->high_used &= ~(1 << (file - 32));
			return;
		}
	}
}


/**
 * Initialise the filename provider.
 */
bool filename_initialise(void)
{
	char *directory, *start;
	int ret;

	directory = strdup(TEMP_FILENAME_PREFIX);
	if (directory == NULL)
		return false;

	for (start = directory; *start != '\0'; start++) {
		if (*start == '/') {
			*start = '\0';
			LOG(("Creating \"%s\"", directory));
			ret = nsmkdir(directory, S_IRWXU);
			if (ret != 0 && errno != EEXIST) {
				LOG(("Failed to create directory \"%s\"",
						directory));
				free(directory);
				return false;
			}

			*start = '/';
		}
	}

	LOG(("Temporary directory location: %s", directory));
	ret = nsmkdir(directory, S_IRWXU);

	free(directory);

	if (ret != 0) {
		return false;
	}
	return true;
}


/**
 * Deletes all files in the cache directory that are not accounted for.
 */
void filename_flush(void)
{
	while (filename_flush_directory(TEMP_FILENAME_PREFIX, 0));
}


/**
 * Deletes some files in a directory that are not accounted for.
 *
 * A single call to this function may not delete all the files in
 * a directory. It should be called until it returns false.
 *
 * \param folder	the folder to search
 * \param depth		the folder depth
 * \returns whether further calls may be needed
 */
bool filename_flush_directory(const char *folder, int depth)
{
	struct AnchorPath *ap;
	struct FileInfoBlock *fib;
	bool changed = false;

	ap = AllocVec(sizeof(struct AnchorPath) + 256, MEMF_CLEAR);
	if (!ap)
		return false;

	fib = AllocDosObject(DOS_FIB, NULL);
	if (!fib) {
		FreeVec(ap);
		return false;
	}

	snprintf(ap->ap_Buf, 256, "%s/#?", folder);
	ap->ap_Strlen = strlen(folder);

	if (!MatchFirst(ap->ap_Buf, ap)) {
		do {
			char child[256];
			snprintf(child, sizeof(child), "%s/%s", folder, ap->ap_Info.fib_FileName);

			if (ap->ap_Info.fib_DirEntryType > 0) {
				// Eintrag ist ein Verzeichnis
				if (depth < 3) {
					// ggf. rekursiv prüfen oder löschen
					if (filename_flush_directory(child, depth + 1)) {
						changed = true;
					}
				} else {
					// Tiefer als erlaubt – löschen
					if (!DeleteFile(child)) {
						LOG(("Failed to remove dir: %s", child));
					} else {
						changed = true;
					}
				}
			} else {
				// Eintrag ist eine Datei
				// Hier kannst du entscheiden, ob du sie löschen willst
				if (!DeleteFile(child)) {
					LOG(("Failed to remove file: %s", child));
				} else {
					changed = true;
				}
			}

		} while (!MatchNext(ap));
		MatchEnd(ap);
	}

	FreeDosObject(DOS_FIB, fib);
	FreeVec(ap);
	return changed;
}

/**
 * Recursively deletes the contents of a directory
 *
 * \param directory	the directory to delete
 * \return true on success, false otherwise
 */
bool filename_delete_recursive(char *folder)
{
	BPTR lock;
	struct FileInfoBlock *fib;

	// Speicher für FileInfoBlock reservieren
	fib = AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR);
	if (fib == NULL) {
		LOG(("Failed to allocate FileInfoBlock"));
		return false;
	}

	lock = Lock(folder, ACCESS_READ);
	if (lock == 0) {
		LOG(("Failed to lock folder: %s", folder));
		FreeVec(fib);
		return false;
	}

	if (Examine(lock, fib) == FALSE) {
		LOG(("Examine failed: %s", folder));
		UnLock(lock);
		FreeVec(fib);
		return false;
	}

	if (fib->fib_DirEntryType < 0) {
		// Kein Verzeichnis
		UnLock(lock);
		FreeVec(fib);
		return false;
	}

	if (ExNext(lock, fib)) {
		do {
			if (strcmp(fib->fib_FileName, ".") == 0 ||
			    strcmp(fib->fib_FileName, "..") == 0)
				continue;

			char child[256];
			snprintf(child, sizeof(child), "%s/%s", folder, fib->fib_FileName);

			if (fib->fib_DirEntryType > 0) {
				// Verzeichnis
				if (!filename_delete_recursive(child)) {
					UnLock(lock);
					FreeVec(fib);
					return false;
				}
			}

			if (DeleteFile(child) == FALSE) {
				LOG(("Failed to delete: %s", child));
				UnLock(lock);
				FreeVec(fib);
				return false;
			}

		} while (ExNext(lock, fib));
	}

	UnLock(lock);
	FreeVec(fib);
	return true;
}


/**
 * Creates a new directory.
 *
 * \param  prefix  the prefix to use, or NULL to allocate a new one
 * \return a new directory structure, or NULL on memory exhaustion or
 * creation failure
 *
 * Empty directories are never deleted, except by an explicit call to
 * filename_flush().
 */
static struct directory *filename_create_directory(const char *prefix)
{
	char *last_1, *last_2;
	int index;
	struct directory *old_dir, *new_dir, *prev_dir = NULL;
	char dir_prefix[16];
	int i;

	/* get the lowest unique prefix, or use the provided one */
	if (prefix == NULL) {
		for (index = 0, old_dir = root; old_dir; 
				index++, old_dir = old_dir->next) {
			if (old_dir->numeric_prefix != index)
				break;

			prev_dir = old_dir;
		}

		sprintf(dir_prefix, "%.2i/%.2i/%.2i/",
				((index >> 12) & 63),
				((index >> 6) & 63),
				((index >> 0) & 63));

		prefix = dir_prefix;
	} else {
		/* prefix format is always '01/23/45/' */
		index = ((prefix[7] + prefix[6] * 10 - START_PREFIX) |
			((prefix[4] + prefix[3] * 10 - START_PREFIX) << 6) |
			((prefix[1] + prefix[0] * 10 - START_PREFIX) << 12));

		for (old_dir = root; old_dir; old_dir = old_dir->next) {
			if (old_dir->numeric_prefix == index)
				return old_dir;

			else if (old_dir->numeric_prefix > index)
				break;

			prev_dir = old_dir;
		}
	}

	/* allocate a new directory */
	new_dir = malloc(sizeof(struct directory));
	if (new_dir == NULL) {
		LOG(("No memory for malloc()"));
		return NULL;
	}

	strncpy(new_dir->prefix, prefix, 9);
	new_dir->prefix[9] = '\0';
	new_dir->low_used = new_dir->high_used = 0;
	new_dir->numeric_prefix = index;

	if (prev_dir == NULL) {
		new_dir->next = root;
		root = new_dir;
	} else {
		new_dir->next = prev_dir->next;
		prev_dir->next = new_dir;
	}

	/* if the previous directory has the same parent then we can simply
	 * create the child. */
	if (prev_dir && strncmp(prev_dir->prefix, new_dir->prefix, 6) == 0) {
		new_dir->prefix[8] = '\0';
		sprintf(filename_directory, "%s/%s",
				TEMP_FILENAME_PREFIX,
				new_dir->prefix);
		new_dir->prefix[8] = '/';

		if (!is_dir(filename_directory)) {
			if (!nsmkdir(filename_directory, S_IRWXU))
				return new_dir;

			/* the user has probably deleted the parent directory
			 * whilst we are running if there is an error, so we
			 * don't report this yet and try to create the
			 * structure normally. */
			LOG(("Failed to create optimised structure '%s'",
					filename_directory));
		}
	}

	/* create the directory structure */
	sprintf(filename_directory, "%s/", TEMP_FILENAME_PREFIX);
	last_1 = filename_directory + SLEN(TEMP_FILENAME_PREFIX) + 1;
	last_2 = new_dir->prefix;

	/* create each subdirectory, up to the maximum depth of 3 */
	for (i = 0; i < 3 && *last_2; i++) {
		*last_1++ = *last_2++;
		while (*last_2 && *last_2 != '/')
			*last_1++ = *last_2++;

		if (*last_2) {
			last_1[0] = '\0';

			if (!is_dir(filename_directory)) {
				if (nsmkdir(filename_directory, S_IRWXU)) {
					LOG(("Failed to create directory '%s'",
							filename_directory));
					return NULL;
				}
			}
		}
	}

	return new_dir;
}
