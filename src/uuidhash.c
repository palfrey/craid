/*  -*- linux-c -*-
 *
 *  These functions are from qstream, downloaded from
 *  http://qstream.org/viewcvs/branches/live/qstream/common/util.c?rev=239&view=auto
 *  on 2006-08-01.
 *   -- Tom Parker
 *  --
 *
 *  util.c - Utility functions for header manipulation, part of
 *               qstream.
 *
 *     Copyright (C) Charles 'Buck' Krasic - July 2002
 *
 *  qstream is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your
 *  option) any later version.
 *   
 *  qstream is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 *  The qstream homepage is http://qstream.sourceforge.net/
 */

#include <glib.h>
#include <uuid/uuid.h>

extern guint 
qsf_uuid_hash(gconstpointer key)
{
        gint    i;
        guint   result = 0;
        guint  *a = (guint *)key;

        for(i=0; i<(sizeof(uuid_t)/sizeof(guint)); i++) {
                result += a[i];
        } /* for */

        return(result);
}

gint 
qsf_uuid_compare(uuid_t a, uuid_t b)
{
        return(!uuid_compare(a, b));
} 
