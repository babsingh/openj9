/*******************************************************************************
 * Copyright (c) 2018, 2022 IBM Corp. and others
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

#include "jvmtiHelpers.h"
#include "jvmti_internal.h"
#include "VMAccess.hpp"

extern "C" {

jvmtiError
suspendThread(J9VMThread *currentThread, jthread thread, UDATA allowNull, BOOLEAN *currentThreadSuspended)
{
	J9VMThread *targetThread = NULL;
	jvmtiError rc = JVMTI_ERROR_NONE;

	*currentThreadSuspended = FALSE;
	rc = getVMThread(currentThread, thread, &targetThread, allowNull, TRUE);
	if (rc == JVMTI_ERROR_NONE) {
#if JAVA_SPEC_VERSION >= 19
		j9object_t threadObject = (NULL == thread) ? currentThread->threadObject : J9_JNI_UNWRAP_REFERENCE(thread);
		if (NULL != targetThread)
#endif /* JAVA_SPEC_VERSION >= 19 */
		{
			if (OMR_ARE_ANY_BITS_SET(targetThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND)) {
				rc = JVMTI_ERROR_THREAD_SUSPENDED;
			} else {
				if (OMR_ARE_ANY_BITS_SET(targetThread->publicFlags, J9_PUBLIC_FLAGS_STOPPED)) {
					/* Do not suspend dead threads. Mirrors SUN behaviour. */
					rc = JVMTI_ERROR_THREAD_NOT_ALIVE;
				} else {
					if (currentThread == targetThread) {
						*currentThreadSuspended = TRUE;
					} else {
						currentThread->javaVM->internalVMFunctions->internalExitVMToJNI(currentThread);
						omrthread_monitor_enter(targetThread->publicFlagsMutex);
						VM_VMAccess::setHaltFlagForVMAccessRelease(targetThread, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND);
						if (VM_VMAccess::mustWaitForVMAccessRelease(targetThread)) {
							while (J9_ARE_ALL_BITS_SET(targetThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND | J9_PUBLIC_FLAGS_VM_ACCESS)) {
								omrthread_monitor_wait(targetThread->publicFlagsMutex);
							}
						}
						omrthread_monitor_exit(targetThread->publicFlagsMutex);
						currentThread->javaVM->internalVMFunctions->internalEnterVMFromJNI(currentThread);
					}
					Trc_JVMTI_threadSuspended(targetThread);
				}
			}
		}
#if JAVA_SPEC_VERSION >= 19
		else {
			/* targetThread is NULL only for virtual threads, as per the assertion in getVMThread,
			 * when mustBeAlive is TRUE.
			 */
			jint vthreadState = J9VMJAVALANGVIRTUALTHREAD_STATE(currentThread, threadObject);
			if (OMR_ARE_ANY_BITS_SET(vthreadState, JVMTI_VTHREAD_STATE_SUSPENDED)) {
				rc = JVMTI_ERROR_THREAD_SUSPENDED;
			} else {
				J9VMJAVALANGVIRTUALTHREAD_SET_STATE(currentThread, threadObject, vthreadState | JVMTI_VTHREAD_STATE_SUSPENDED);
			}
		}
		if (IS_VIRTUAL_THREAD(currentThread, threadObject)) {
			jint vthreadState = J9VMJAVALANGVIRTUALTHREAD_STATE(currentThread, threadObject);
			if (NULL == targetThread) {
				printf("suspend vthread: threadObj=0x%p, targetThread=0x%p, state=0x%x, publicFlags=%zx\n", threadObject, targetThread, vthreadState, (UDATA)0);
			} else {
				printf("suspend vthread: threadObj=0x%p, targetThread=0x%p, state=0x%x, publicFlags=%zx\n", threadObject, targetThread, vthreadState, targetThread->publicFlags);
			}
		}
#endif /* JAVA_SPEC_VERSION >= 19 */
		releaseVMThread(currentThread, targetThread, thread);
	}
	return rc;
}

} /* extern "C" */
