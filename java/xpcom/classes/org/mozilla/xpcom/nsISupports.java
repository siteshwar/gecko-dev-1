/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Frank
 * Mitchell. Portions created by Frank Mitchell are
 * Copyright (C) 1999 Frank Mitchell. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *    Frank Mitchell (frank.mitchell@sun.com)
 */
/*
 * ************* DO NOT EDIT THIS FILE ***********
 *
 * This file was automatically generated from /home/frankm/mozilla/dist/idl/nsISupports.idl.
 */
package org.mozilla.xpcom;

import org.mozilla.xpcom.nsID;

/**
 * Interface nsISupports
 *
 * IID: 0x00000000-0000-0000-c000-000000000046
 */

public interface nsISupports 
{
    public static final String NS_ISUPPORTS_IID_STRING =
        "00000000-0000-0000-c000-000000000046";

    public static final nsID NS_ISUPPORTS_IID =
        new nsID(NS_ISUPPORTS_IID_STRING);


    /* void QueryInterface (in nsIIDRef uuid, [iid_is (uuid), retval] out nsQIResult result); */
    public Object QueryInterface(nsID uuid);

}

/*
 * end
 */
