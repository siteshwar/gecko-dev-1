/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
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

#include "nsFFObject.h"
#include "prlog.h"

nsFFObject::nsFFObject(PRUint32 i_ID, PRUint32 i_Offset, PRUint32 i_Size):
    m_ID(i_ID), 
    m_Offset(i_Offset),
    m_Size(i_Size),
    m_pNext(0)
{
}

nsFFObject::~nsFFObject()
{
}


PRBool nsFFObject::Add(const nsFFObject* i_object)
{
    PR_ASSERT(i_object);
    if (!i_object)
        return PR_FALSE;
    if (m_pNext)
        return m_pNext->Add(i_object);
    m_pNext = (nsFFObject*) i_object;
    return PR_TRUE;
}