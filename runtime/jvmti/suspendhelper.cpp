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
suspendThread(J9VMThread *currentThread, jthread thread, UDATA allowNull, UDATA *currentThreadSuspended)
{
	J9VMThread *targetThread = NULL;
	jvmtiError rc = JVMTI_ERROR_NONE;

	*currentThreadSuspended = FALSE;
	printf("enter suspendThread %p\n", J9_JNI_UNWRAP_REFERENCE(thread));
	rc = getVMThread(currentThread, thread, &targetThread, allowNull, TRUE);
	if (rc == JVMTI_ERROR_NONE) {
		printf("enter suspendThread getVMThread %p\n", J9_JNI_UNWRAP_REFERENCE(thread));
#if JAVA_SPEC_VERSION >= 19
		j9object_t threadObject = (NULL == thread) ? currentThread->threadObject : J9_JNI_UNWRAP_REFERENCE(thread);
		BOOLEAN isVirtual = IS_VIRTUAL_THREAD(currentThread, threadObject);
		jint vthreadState = J9VMJAVALANGVIRTUALTHREAD_STATE(currentThread, threadObject);
		printf("suspendThread vthread_state=0x%X, targetThread=%p\n", vthreadState, targetThread);
		if ((NULL != targetThread) && OMR_ARE_ANY_BITS_SET(vthreadState, JVMTI_VTHREAD_STATE_RUNNING))
#endif /* JAVA_SPEC_VERSION >= 19 */
		{
			if (targetThread->publicFlags & J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND) {
				rc = JVMTI_ERROR_THREAD_SUSPENDED;
			} else {
				if (targetThread->publicFlags & J9_PUBLIC_FLAGS_STOPPED) {
					/* Do not suspend dead threads. Mirrors SUN behaviour. */
					rc = JVMTI_ERROR_THREAD_NOT_ALIVE;
				} else {
					if (currentThread == targetThread) {
						*currentThreadSuspended = TRUE;
					} else {
						printf("enter suspendThread halt\n");
						currentThread->javaVM->internalVMFunctions->internalExitVMToJNI(currentThread);
						omrthread_monitor_enter(targetThread->publicFlagsMutex);
						VM_VMAccess::setHaltFlagForVMAccessRelease(targetThread, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND);
						if (VM_VMAccess::mustWaitForVMAccessRelease(targetThread)) {
							printf("enter suspendThread while loop\n");
							while (J9_ARE_ALL_BITS_SET(targetThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND | J9_PUBLIC_FLAGS_VM_ACCESS)) {
								omrthread_monitor_wait(targetThread->publicFlagsMutex);
							}
							printf("exit suspendThread while loop\n");
						}
						omrthread_monitor_exit(targetThread->publicFlagsMutex);
						currentThread->javaVM->internalVMFunctions->internalEnterVMFromJNI(currentThread);
						printf("exit suspendThread halt\n");
					}
					Trc_JVMTI_threadSuspended(targetThread);
				}
			}
		}
#if JAVA_SPEC_VERSION >= 19
		else if (isVirtual) {
			if (OMR_ARE_ANY_BITS_SET(vthreadState, JVMTI_VTHREAD_STATE_SUSPENDED)) {
				rc = JVMTI_ERROR_THREAD_SUSPENDED;
			} else {
				J9VMJAVALANGVIRTUALTHREAD_SET_STATE(currentThread, threadObject, vthreadState | JVMTI_VTHREAD_STATE_SUSPENDED);
			}
		}
#endif /* JAVA_SPEC_VERSION >= 19 */
		printf("enter suspendThread releaseVMThread %p\n", J9_JNI_UNWRAP_REFERENCE(thread));
		releaseVMThread(currentThread, targetThread, thread);
		printf("exit suspendThread releaseVMThread %p\n", J9_JNI_UNWRAP_REFERENCE(thread));
	}
	printf("exit suspendThread %p\n", J9_JNI_UNWRAP_REFERENCE(thread));
	return rc;
}

} /* extern "C" */
