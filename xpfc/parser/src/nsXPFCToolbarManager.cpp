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

#include "nsXPFCToolbarManager.h"
#include "nsxpfcCIID.h"
#include "nsxpfcutil.h"
#include "nsIXPFCToolbar.h"

static NS_DEFINE_IID(kISupportsIID, NS_ISUPPORTS_IID);
static NS_DEFINE_IID(kCXPFCToolbarManagerCID, NS_XPFCTOOLBAR_MANAGER_CID);
static NS_DEFINE_IID(kIXPFCToolbarManagerIID, NS_IXPFCTOOLBAR_MANAGER_IID);

nsXPFCToolbarManager :: nsXPFCToolbarManager()
{
  NS_INIT_REFCNT();
  mToolbars = nsnull;
}

nsXPFCToolbarManager :: ~nsXPFCToolbarManager()
{
  if (mToolbars != nsnull) {

	  nsIIterator * iterator;

	  mToolbars->CreateIterator(&iterator);
	  iterator->Init();

    nsIXPFCToolbar * item;

	  while(!(iterator->IsDone()))
	  {
		  item = (nsIXPFCToolbar *) iterator->CurrentItem();
		  NS_RELEASE(item);
		  iterator->Next();
	  }
	  NS_RELEASE(iterator);

    mToolbars->RemoveAll();
    NS_RELEASE(mToolbars);
  }

}

NS_IMPL_QUERY_INTERFACE(nsXPFCToolbarManager, kIXPFCToolbarManagerIID)
NS_IMPL_ADDREF(nsXPFCToolbarManager)
NS_IMPL_RELEASE(nsXPFCToolbarManager)

nsresult nsXPFCToolbarManager :: Init()
{
  static NS_DEFINE_IID(kCVectorCID, NS_ARRAY_CID);
  nsresult res = nsRepository::CreateInstance(kCVectorCID, 
                                     nsnull, 
                                     kCVectorCID, 
                                     (void **)&mToolbars);

  if (NS_OK != res)
    return res ;

  mToolbars->Init();

  return NS_OK ;
}

nsresult nsXPFCToolbarManager :: AddToolbar(nsIXPFCToolbar * aToolbar)
{
  mToolbars->Append(aToolbar);
  NS_ADDREF(aToolbar);
  return NS_OK ;
}

