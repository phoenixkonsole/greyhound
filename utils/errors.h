/*
 * Copyright 2009 John-Mark Bell <jmb@greyhound-browser.org>
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
 * Error codes
 */

#ifndef GREYHOUND_UTILS_ERRORS_H_
#define GREYHOUND_UTILS_ERRORS_H_

/**
 * Enumeration of error codes
 */
typedef enum {
	NSERROR_OK,			/**< No error */

	NSERROR_UNKNOWN,		/**< Unknown error - DO *NOT* USE */

	NSERROR_NOMEM,			/**< Memory exhaustion */

	NSERROR_NO_FETCH_HANDLER,	/**< No fetch handler for URL scheme */

	NSERROR_NOT_FOUND,		/**< Requested item not found */

	NSERROR_NOT_DIRECTORY,           /**< Missing directory */

	NSERROR_SAVE_FAILED,		/**< Failed to save data */

	NSERROR_CLONE_FAILED,		/**< Failed to clone handle */

	NSERROR_INIT_FAILED,		/**< Initialisation failed */

	NSERROR_MNG_ERROR,		/**< An MNG error occurred */

	NSERROR_BAD_ENCODING,		/**< The character set is unknown */

	NSERROR_NEED_DATA,		/**< More data needed */

	NSERROR_ENCODING_CHANGE,	/**< The character changed */

	NSERROR_BAD_PARAMETER,		/**< Bad Parameter */

	NSERROR_INVALID,		/**< Invalid data */

	NSERROR_BOX_CONVERT,		/**< Box conversion failed */

	NSERROR_STOPPED,		/**< Content conversion stopped */

	NSERROR_DOM,	                /**< DOM call returned error */

	NSERROR_CSS,	                /**< CSS call returned error */

	NSERROR_CSS_BASE,               /**< CSS base sheet failed */

	NSERROR_BAD_URL,		/**< Bad URL */

	NSERROR_BAD_CONTENT,		/**< Bad Content */

	NSERROR_FRAME_DEPTH,            /**< Exceeded frame depth */

	NSERROR_PERMISSION,             /**< Permission error */

	NSERROR_NOSPACE,		/**< Insufficient space */

	NSERROR_BAD_SIZE,               /**< Bad size */

	NSERROR_NOT_IMPLEMENTED,        /**< Functionality is not implemented */
} nserror;

#endif

