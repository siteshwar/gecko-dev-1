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

/* jband Sept 1998 - **NOT original file** - copied here for simplicity */

package netscape.security;

import java.lang.RuntimeException;

/**
 * This exception is thrown when a privilege request is denied.
 * @see netscape.security.PrivilegeManager
 */
public
class ForbiddenTargetException extends java.lang.RuntimeException {
    /**
     * Constructs an IllegalArgumentException with no detail message.
     * A detail message is a String that describes this particular exception.
     */
    public ForbiddenTargetException() {
	super();
    }

    /**
     * Constructs an ForbiddenTargetException with the specified detail message.
     * A detail message is a String that describes this particular exception.
     * @param s the detail message
     */
    public ForbiddenTargetException(String s) {
	super(s);
    }
}

