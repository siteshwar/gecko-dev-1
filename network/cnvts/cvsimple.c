/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */
/**********************************************************************
 cvsimple.c
 By Daniel Malmer <malmer@netscape.com>
 1/14/98

 Simple converter that just saves the data in a buffer.
 
**********************************************************************/

#include "cvsimple.h"
#include "xp.h"
#include "prmem.h"

typedef void (*simple_complete_t)(void* bytes, int32 bytes_written);

typedef struct {
	unsigned char* bytes;
	int32 bytes_written;
	int32 max_bytes;
	simple_complete_t complete;
} NET_SimpleStreamData;


/*
 * simple_complete
 */
PRIVATE void
simple_complete(NET_StreamClass *stream)
{
	NET_SimpleStreamData* obj = (NET_SimpleStreamData*) stream->data_object;	

	if ( obj && obj->complete ) {
		(obj->complete)(obj->bytes, obj->bytes_written);
	}

	if ( obj && obj->bytes ) PR_Free(obj->bytes);
	if ( obj ) PR_Free(obj);
}


/*
 * simple_abort
 */
PRIVATE void
simple_abort(NET_StreamClass *stream, int status)
{
	NET_SimpleStreamData* obj = (NET_SimpleStreamData*) stream->data_object;	

	if ( obj && obj->bytes ) PR_Free(obj->bytes);
	if ( obj ) PR_Free(obj);
}


/*
 * simple_write
 */
PRIVATE int
simple_write(NET_StreamClass *stream, const char* str, int32 len)
{
	NET_SimpleStreamData* obj = (NET_SimpleStreamData*) stream->data_object;	

	if ( obj->bytes_written + len > obj->max_bytes ) {
		/* Round to nearest 1024 */
		obj->max_bytes = ( ( ( (obj->max_bytes + len) >> 10) + 1) << 10);
		obj->bytes = PR_Realloc(obj->bytes, obj->max_bytes);
	}

	memcpy(obj->bytes + obj->bytes_written, str, len);
	obj->bytes_written+= len;

	return MK_DATA_LOADED;
}


/*
 * simple_write_ready
 */
PRIVATE unsigned int
simple_write_ready(NET_StreamClass *stream)
{	
	return MAX_WRITE_READY;
}


/*
 * NET_SimpleStream
 * Simple stream constructor.
 */
MODULE_PRIVATE NET_StreamClass*
NET_SimpleStream(int fmt, void* data_obj, URL_Struct* URL_s, MWContext* w)
{
	NET_SimpleStreamData* obj;

	if ( (obj = PR_NEWZAP(NET_SimpleStreamData)) == NULL ) {
		return NULL;
	}

	obj->bytes = NULL;
	obj->bytes_written = 0;
	obj->max_bytes = 0;
	obj->complete = (simple_complete_t) data_obj;

	return NET_NewStream("SimpleStream", simple_write, simple_complete,
							simple_abort, simple_write_ready, obj, w);
}


