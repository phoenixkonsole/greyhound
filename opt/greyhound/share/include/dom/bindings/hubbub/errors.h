/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@greyhound-browser.org>
 */

#ifndef dom_hubbub_errors_h_
#define dom_hubbub_errors_h_

typedef enum {
	DOM_HUBBUB_OK           = 0,

	DOM_HUBBUB_NOMEM        = 1,

	DOM_HUBBUB_BADPARM      = 2, /**< Bad input parameter */

	DOM_HUBBUB_DOM          = 3, /**< DOM operation failed */

	DOM_HUBBUB_HUBBUB_ERR   = (1<<16),

	DOM_HUBBUB_HUBBUB_ERR_PAUSED = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_PAUSED),

	DOM_HUBBUB_HUBBUB_ERR_ENCODINGCHANGE = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_ENCODINGCHANGE),

	DOM_HUBBUB_HUBBUB_ERR_NOMEM = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_NOMEM),

	DOM_HUBBUB_HUBBUB_ERR_BADPARM = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_BADPARM),

	DOM_HUBBUB_HUBBUB_ERR_INVALID = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_INVALID),

	DOM_HUBBUB_HUBBUB_ERR_FILENOTFOUND = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_FILENOTFOUND),

	DOM_HUBBUB_HUBBUB_ERR_NEEDDATA = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_NEEDDATA),

	DOM_HUBBUB_HUBBUB_ERR_BADENCODING = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_BADENCODING),

	DOM_HUBBUB_HUBBUB_ERR_UNKNOWN = (DOM_HUBBUB_HUBBUB_ERR | HUBBUB_UNKNOWN),

} dom_hubbub_error;

#endif
