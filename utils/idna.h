/*
 * Copyright 2014 Chris Young <chris@unsatisfactorysoftware.co.uk>
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
 * Greyhound international domain name handling (interface).
 */

#ifndef _GREYHOUND_UTILS_IDNA_H_
#define _GREYHOUND_UTILS_IDNA_H_

/**
 * Convert a hostname to an ACE version suitable for DNS lookup
 *
 * \param host	String containing host
 * \param len	Length of host string
 * \param ace_host	Pointer to update with the output
 * \param ace_len	Pointer to update with length of ace_host
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, output will be left untouched.
 */
nserror idna_encode(const char *host, size_t len, char **ace_host, size_t *ace_len);


/**
 * Convert a hostname from ACE to UTF-8 suitable for display
 *
 * \param ace_host	String containing host
 * \param ace_len	Length of host string
 * \param host	Pointer to update with the output
 * \param host_len	Pointer to update with length of host
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, output will be left untouched.
 */
nserror idna_decode(const char *ace_host, size_t ace_len, char **host, size_t *host_len);

#endif

