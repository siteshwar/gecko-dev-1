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

#include "nscore.h"
#include "nsXPFCCommand.h"
#include "nsxpfcCIID.h"
#include "nsIXPFCCommandReceiver.h"

static NS_DEFINE_IID(kISupportsIID,  NS_ISUPPORTS_IID);
static NS_DEFINE_IID(kXPFCCommandIID, NS_IXPFC_COMMAND_IID);

nsXPFCCommand :: nsXPFCCommand()
{
  NS_INIT_REFCNT();
  mReceiver = nsnull;
}

nsXPFCCommand :: ~nsXPFCCommand()  
{
  mReceiver = nsnull;
}

NS_IMPL_ADDREF(nsXPFCCommand)
NS_IMPL_RELEASE(nsXPFCCommand)
NS_IMPL_QUERY_INTERFACE(nsXPFCCommand, kXPFCCommandIID)

nsresult nsXPFCCommand::Init()
{
  return NS_OK;
}

nsresult nsXPFCCommand::Execute()
{
  mReceiver->Action(this);  
  return (NS_OK);
}

nsIXPFCCommandReceiver * nsXPFCCommand::GetReceiver()
{
  return (mReceiver);
}

nsresult nsXPFCCommand::SetReceiver(nsIXPFCCommandReceiver * aReceiver)
{
  mReceiver = aReceiver;
  return (NS_OK);
}
