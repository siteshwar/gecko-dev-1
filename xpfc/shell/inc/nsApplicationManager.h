/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
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

#ifndef __nsApplicationManager_h
#define __nsApplicationManager_h


#include "nscore.h"
#include "plstr.h"
#include "prtypes.h"
#include "prmon.h"
#include "plstr.h"
#include "nsCom.h"
#include "nsxpfc.h"
#include "nsHashtable.h"
#include "nsIShellInstance.h"
#include "nsIApplicationShell.h"


/**
 * ModalMessage enumerations
 */
enum nsModalMessageType 
{   
  eModalMessage_ok,
  eModalMessage_ok_cancel,
}; 


// Application Manager - Manages Application Instances
class NS_XPFC nsApplicationManager 
{
public:
   
  /**
   * Initialize the ApplicationManager
   * @result The result of the initialization, NS_OK if no errors
   */
  static nsresult Initialize();

  /**
   * Get the ShellInstance associated with an Application
   * @param aApplicationShell - The Application Shell
   * @param aShellInstance = The returned shell instance for this application
   * @result The result of the initialization, NS_OK if no errors
   */
  static nsresult GetShellInstance(nsIApplicationShell * aApplicationShell,
                                   nsIShellInstance **aShellInstance);

  /**
   * Set the ShellInstance associated with an Application
   * @param aApplicationShell - The Application Shell
   * @param aShellInstance =The Shell instance for this application
   * @result The result of the initialization, NS_OK if no errors
   */
  static nsresult SetShellAssociation(nsIApplicationShell * aApplicationShell,
                                      nsIShellInstance *aShellInstance);

  /**
   * Delete the Association of the ShellInstance with the Application
   * @param aApplicationShell - The Application Shell
   * @param aShellInstance =The Shell instance for this application
   * @result The result of the initialization, NS_OK if no errors
   */
  static nsresult DeleteShellAssociation(nsIApplicationShell * aApplicationShell,
                                         nsIShellInstance *aShellInstance);

  /**
   * Display an Application-wide Modal Message Box
   * @param aMessage The Message  String to display
   * @param aTitle The String to display in title area
   * @param aModalMessageType The type of Modal Message
   * @result The result of the initialization, NS_OK if no errors
   */
  static nsresult ModalMessage(const nsString &aMessage, const nsString &aTitle, nsModalMessageType aModalMessageType);

private:

  static nsresult checkInitialized();

private:

  static PRMonitor *    monitor;
  static nsHashtable *  applications;


};

#endif
