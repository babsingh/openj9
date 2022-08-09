/*******************************************************************************
 * Copyright (c) 1991, 2022 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef jvmtiHelpers_h
#define jvmtiHelpers_h

#include <string.h>

#include "jvmtiInternal.h"
#include "j9consts.h"
#include "j9protos.h"
#include "zip_api.h"
#include "j9cp.h"
#include "rommeth.h"
#include "vmhook.h"
#include "mmhook.h"
#include "mmomrhook.h"
#include "stackwalk.h"
#include "ut_j9jvmti.h"
#include "omrlinkedlist.h"
#include "jvmti_internal.h"

#if JAVA_SPEC_VERSION >= 19
/* See VirtualThreadTests.test_verifyJVMTIMacros for validation of below macro values. */
#define JVMTI_VTHREAD_STATE_NEW 0
#define JVMTI_VTHREAD_STATE_TERMINATED 99
#endif /* JAVA_SPEC_VERSION >= 19 */

#endif			/* jvmtiHelpers_h */

