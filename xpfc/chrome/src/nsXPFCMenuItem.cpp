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
#include "nsXPFCMenuItem.h"
#include "nsIXMLParserObject.h"
#include "nsXPFCActionCommand.h"
#include "nsRepository.h"
#include "nsxpfcCIID.h"

static NS_DEFINE_IID(kIXMLParserObjectIID, NS_IXML_PARSER_OBJECT_IID);

nsXPFCMenuItem::nsXPFCMenuItem()
{
  NS_INIT_REFCNT();
  mName = "Item";
  mLabel = "Label";
  mParent = nsnull;
  mSeparator = PR_FALSE;
  mAlignmentStyle = eAlignmentStyle_none;
  mID = 0;
  mReceiver = nsnull;
  mCommand = "Command";
  mEnabled = PR_TRUE;
}

nsXPFCMenuItem::~nsXPFCMenuItem()
{
}

NS_DEFINE_IID(kIXPFCMenuItemIID, NS_IXPFCMENUITEM_IID);

NS_IMPL_ADDREF(nsXPFCMenuItem)
NS_IMPL_RELEASE(nsXPFCMenuItem)

nsresult nsXPFCMenuItem::QueryInterface(REFNSIID aIID, void** aInstancePtr)      
{                                                                        

  if (NULL == aInstancePtr) {                                            
    return NS_ERROR_NULL_POINTER;                                        
  }                                                                      
  static NS_DEFINE_IID(kISupportsIID, NS_ISUPPORTS_IID);                 
  static NS_DEFINE_IID(kClassIID, kIXPFCMenuItemIID);                         

  if (aIID.Equals(kClassIID)) {                                          
    *aInstancePtr = (void*) this;                                        
    AddRef();                                                            
    return NS_OK;                                                        
  }                                                                      
  if (aIID.Equals(kISupportsIID)) {                                      
    *aInstancePtr = (void*) (this);                        
    AddRef();                                                            
    return NS_OK;                                                        
  }                                                                      
  if (aIID.Equals(kIXMLParserObjectIID)) {                                      
    *aInstancePtr = (nsIXMLParserObject*) (this);                        
    AddRef();                                                            
    return NS_OK;                                                        
  }                                                                      

  return NS_NOINTERFACE;

}

nsresult nsXPFCMenuItem::Init()
{
  return NS_OK;
}

nsresult nsXPFCMenuItem :: SetParameter(nsString& aKey, nsString& aValue)
{
  if (aKey == "name") {

    SetName(aValue);

  } else if (aKey == "align") {

    if (aValue == "left")
      mAlignmentStyle = eAlignmentStyle_left;
    else if (aValue == "right")
      mAlignmentStyle = eAlignmentStyle_right;
    
  } else if (aKey == "enable") {

    if (aValue == "False")
      mEnabled = PR_FALSE;
    else if (aValue == "True")
      mEnabled = PR_TRUE;
    
  } else if (aKey == "type") {

    if (aValue == "separator")
      mSeparator = PR_TRUE;

  } else if (aKey == "label") {
    
    SetLabel(aValue);

  } else if (aKey.EqualsIgnoreCase("command")) {

    if (aValue.Find("LoadUrl?") != -1)
    {
      nsString command;

      aValue.Mid(command,8,aValue.Length()-8);

      SetCommand(command);    

    } else {

      SetCommand(aValue);    

    }
    
  }


  return NS_OK;
}

PRBool nsXPFCMenuItem::IsSeparator()
{
  return (mSeparator);
}

nsAlignmentStyle nsXPFCMenuItem::GetAlignmentStyle()
{
  return (mAlignmentStyle);
}

nsresult nsXPFCMenuItem::SetAlignmentStyle(nsAlignmentStyle aAlignmentStyle)
{
  mAlignmentStyle = aAlignmentStyle;
  return NS_OK;
}

PRBool nsXPFCMenuItem::GetEnabled()
{
  return (mEnabled);
}

nsresult nsXPFCMenuItem::SetEnabled(PRBool aEnable)
{
  mEnabled = aEnable;
  return NS_OK;
}

nsresult nsXPFCMenuItem :: SetName(nsString& aName)
{
  mName = aName;
  return NS_OK;
}

nsString& nsXPFCMenuItem :: GetName()
{
  return (mName);
}

nsresult nsXPFCMenuItem :: SetCommand(nsString& aCommand)
{
  mCommand = aCommand;
  return NS_OK;
}

nsString& nsXPFCMenuItem :: GetCommand()
{
  return (mCommand);
}

nsresult nsXPFCMenuItem :: SetLabel(nsString& aLabel)
{
  mLabel = aLabel;
  return NS_OK;
}

nsString& nsXPFCMenuItem :: GetLabel()
{
  return (mLabel);
}

nsIXPFCMenuContainer * nsXPFCMenuItem::GetParent()
{
  return (mParent);
}

nsresult nsXPFCMenuItem::SetParent(nsIXPFCMenuContainer * aParent)
{
  mParent = aParent;
  return NS_OK;
}

nsresult nsXPFCMenuItem :: SetMenuID(PRUint32 aID)
{
  mID = aID;
  return NS_OK;
}

PRUint32 nsXPFCMenuItem :: GetMenuID()
{
  return (mID);
}

nsresult nsXPFCMenuItem::SetReceiver(nsIXPFCCommandReceiver * aReceiver)
{
  mReceiver = aReceiver;
  return NS_OK;
}

nsresult nsXPFCMenuItem :: SendCommand()
{
  if (mReceiver == nsnull)
    return NS_OK;

  nsXPFCActionCommand * command;

  static NS_DEFINE_IID(kCXPFCActionCommandCID, NS_XPFC_ACTION_COMMAND_CID);
  static NS_DEFINE_IID(kXPFCCommandIID, NS_IXPFC_COMMAND_IID);

  nsresult res = nsRepository::CreateInstance(kCXPFCActionCommandCID, 
                                              nsnull, 
                                              kXPFCCommandIID, 
                                              (void **)&command);

  if (NS_OK != res)
    return res ;

  command->Init();

  command->mAction = mCommand;

  mReceiver->Action(command);

  NS_RELEASE(command);

  return (NS_OK);
}

