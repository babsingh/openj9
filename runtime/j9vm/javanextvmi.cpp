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
#include <assert.h>
#include <jni.h>

#include "bcverify_api.h"
#include "j9.h"
#include "j9cfg.h"
#include "rommeth.h"
#include "ut_j9scar.h"

#if JAVA_SPEC_VERSION >= 19
#include "jvmtiInternal.h"
#include "VMHelpers.hpp"
#endif /* JAVA_SPEC_VERSION >= 19 */

extern "C" {

#if JAVA_SPEC_VERSION >= 19
extern J9JavaVM *BFUjavaVM;

/* These come from jvm.c */
extern IDATA (*f_monitorEnter)(omrthread_monitor_t monitor);
extern IDATA (*f_monitorExit)(omrthread_monitor_t monitor);
extern IDATA (*f_monitorWait)(omrthread_monitor_t monitor);
extern IDATA (*f_monitorNotifyAll)(omrthread_monitor_t monitor);
#endif /* JAVA_SPEC_VERSION >= 19 */

/* Define for debug
#define DEBUG_BCV
*/

#if JAVA_SPEC_VERSION >= 16
JNIEXPORT void JNICALL
JVM_DefineArchivedModules(JNIEnv *env, jobject obj1, jobject obj2)
{
	assert(!"JVM_DefineArchivedModules unimplemented");
}

JNIEXPORT void JNICALL
JVM_LogLambdaFormInvoker(JNIEnv *env, jstring str)
{
	assert(!"JVM_LogLambdaFormInvoker unimplemented");
}

JNIEXPORT jboolean JNICALL
JVM_IsDumpingClassList(JNIEnv *env)
{
	return JNI_FALSE;
}
#endif /* JAVA_SPEC_VERSION >= 16 */

#if JAVA_SPEC_VERSION >= 11
JNIEXPORT void JNICALL
JVM_InitializeFromArchive(JNIEnv *env, jclass clz)
{
	/* A no-op implementation is ok. */
}
#endif /* JAVA_SPEC_VERSION >= 11 */

#if JAVA_SPEC_VERSION >= 14
typedef struct GetStackTraceElementUserData {
	J9ROMClass *romClass;
	J9ROMMethod *romMethod;
	UDATA bytecodeOffset;
} GetStackTraceElementUserData;

static UDATA
getStackTraceElementIterator(J9VMThread *vmThread, void *voidUserData, UDATA bytecodeOffset, J9ROMClass *romClass, J9ROMMethod *romMethod, J9UTF8 *fileName, UDATA lineNumber, J9ClassLoader *classLoader, J9Class* ramClass)
{
	UDATA result = J9_STACKWALK_STOP_ITERATING;

	if ((NULL != romMethod)
		&& J9_ARE_ALL_BITS_SET(romMethod->modifiers, J9AccMethodFrameIteratorSkip)
	) {
		/* Skip methods with java.lang.invoke.FrameIteratorSkip / jdk.internal.vm.annotation.Hidden / java.lang.invoke.LambdaForm$Hidden annotation */
		result = J9_STACKWALK_KEEP_ITERATING;
	} else {
		GetStackTraceElementUserData *userData = (GetStackTraceElementUserData *)voidUserData;

		/* We are done, first non-hidden stack frame is found. */
		userData->romClass = romClass;
		userData->romMethod = romMethod;
		userData->bytecodeOffset = bytecodeOffset;
	}
	return result;
}

#if defined(DEBUG_BCV)
static void cfdumpBytecodePrintFunction(void *userData, char *format, ...)
{
	PORT_ACCESS_FROM_PORT((J9PortLibrary*)userData);
	va_list args;
	char outputBuffer[512] = {0};

	va_start(args, format);
	j9str_vprintf(outputBuffer, 512, format, args);
	va_end(args);
	j9tty_printf(PORTLIB, "%s", outputBuffer);
}
#endif /* defined(DEBUG_BCV) */

JNIEXPORT jstring JNICALL
JVM_GetExtendedNPEMessage(JNIEnv *env, jthrowable throwableObj)
{
	J9VMThread *vmThread = (J9VMThread *)env;
	J9JavaVM *vm = vmThread->javaVM;
	jobject msgObjectRef = NULL;

	Trc_SC_GetExtendedNPEMessage_Entry(vmThread, throwableObj);
	if (J9_ARE_ANY_BITS_SET(vm->extendedRuntimeFlags2, J9_EXTENDED_RUNTIME2_SHOW_EXTENDED_NPEMSG)) {
		J9InternalVMFunctions const * const vmFuncs = vm->internalVMFunctions;
		char *npeMsg = NULL;
		GetStackTraceElementUserData userData = {0};
		/* If -XX:+ShowHiddenFrames option has not been set, skip hidden method frames */
		UDATA skipHiddenFrames = J9_ARE_NO_BITS_SET(vm->runtimeFlags, J9_RUNTIME_SHOW_HIDDEN_FRAMES);

		Trc_SC_GetExtendedNPEMessage_Entry2(vmThread, throwableObj);
		vmFuncs->internalEnterVMFromJNI(vmThread);
		userData.bytecodeOffset = UDATA_MAX;
		vmFuncs->iterateStackTrace(vmThread, (j9object_t*)throwableObj, getStackTraceElementIterator, &userData, TRUE, skipHiddenFrames);
		if ((NULL != userData.romClass)
			&& (NULL != userData.romMethod)
			&& (UDATA_MAX != userData.bytecodeOffset)
		) {
			PORT_ACCESS_FROM_VMC(vmThread);
			J9NPEMessageData npeMsgData = {0};
#if defined(DEBUG_BCV)
			{
				U_8 *bytecodes = J9_BYTECODE_START_FROM_ROM_METHOD(userData.romMethod);
				U_32 flags = 0;

#if defined(J9VM_ENV_LITTLE_ENDIAN)
				flags |= BCT_LittleEndianOutput;
#else /* defined(J9VM_ENV_LITTLE_ENDIAN) */
				flags |= BCT_BigEndianOutput;
#endif /* defined(J9VM_ENV_LITTLE_ENDIAN) */
				j9bcutil_dumpBytecodes(PORTLIB, userData.romClass, bytecodes, 0, userData.bytecodeOffset, flags, (void *)cfdumpBytecodePrintFunction, PORTLIB, "");
			}
#endif /* defined(DEBUG_BCV) */
			npeMsgData.npePC = userData.bytecodeOffset;
			npeMsgData.vmThread = vmThread;
			npeMsgData.romClass = userData.romClass;
			npeMsgData.romMethod = userData.romMethod;
			npeMsg = vmFuncs->getNPEMessage(&npeMsgData);
			if (NULL != npeMsg) {
				j9object_t msgObject = vm->memoryManagerFunctions->j9gc_createJavaLangString(vmThread, (U_8 *)npeMsg, strlen(npeMsg), 0);
				if (NULL != msgObject) {
					msgObjectRef = vmFuncs->j9jni_createLocalRef(env, msgObject);
				}
				j9mem_free_memory(npeMsg);
			}
			j9mem_free_memory(npeMsgData.liveStack);
			j9mem_free_memory(npeMsgData.bytecodeOffset);
			j9mem_free_memory(npeMsgData.bytecodeMap);
			j9mem_free_memory(npeMsgData.stackMaps);
			j9mem_free_memory(npeMsgData.unwalkedQueue);
		} else {
			Trc_SC_GetExtendedNPEMessage_Null_NPE_MSG(vmThread, userData.romClass, userData.romMethod, userData.bytecodeOffset);
		}
		vmFuncs->internalExitVMToJNI(vmThread);
	}
	Trc_SC_GetExtendedNPEMessage_Exit(vmThread, msgObjectRef);

	return (jstring)msgObjectRef;
}
#endif /* JAVA_SPEC_VERSION >= 14 */

#if JAVA_SPEC_VERSION >= 17
JNIEXPORT void JNICALL
JVM_DumpClassListToFile(JNIEnv *env, jstring str)
{
	assert(!"JVM_DumpClassListToFile unimplemented");
}

JNIEXPORT void JNICALL
JVM_DumpDynamicArchive(JNIEnv *env, jstring str)
{
	assert(!"JVM_DumpDynamicArchive unimplemented");
}
#endif /* JAVA_SPEC_VERSION >= 17 */

#if JAVA_SPEC_VERSION >= 18
JNIEXPORT jboolean JNICALL
JVM_IsFinalizationEnabled(JNIEnv *env)
{
	jboolean isFinalizationEnabled = JNI_TRUE;
	J9VMThread *currentThread = (J9VMThread*)env;
	if (J9_ARE_ANY_BITS_SET(currentThread->javaVM->extendedRuntimeFlags2, J9_EXTENDED_RUNTIME2_DISABLE_FINALIZATION)) {
		isFinalizationEnabled = JNI_FALSE;
	}
	return isFinalizationEnabled;
}

JNIEXPORT void JNICALL
JVM_ReportFinalizationComplete(JNIEnv *env, jobject obj)
{
	assert(!"JVM_ReportFinalizationComplete unimplemented");
}
#endif /* JAVA_SPEC_VERSION >= 18 */

#if JAVA_SPEC_VERSION >= 19
JNIEXPORT void JNICALL
JVM_LoadZipLibrary(void)
{
	assert(!"JVM_LoadZipLibrary unimplemented");
}

JNIEXPORT void JNICALL
JVM_RegisterContinuationMethods(JNIEnv *env, jclass clz)
{
	assert(!"JVM_RegisterContinuationMethods unimplemented");
}

JNIEXPORT jboolean JNICALL
JVM_IsContinuationsSupported(void)
{
	return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
JVM_IsPreviewEnabled(void)
{
	jboolean isPreviewEnabled = JNI_FALSE;
	J9VMThread *currentThread = BFUjavaVM->internalVMFunctions->currentVMThread(BFUjavaVM);
	if (J9_ARE_ANY_BITS_SET(currentThread->javaVM->extendedRuntimeFlags2, J9_EXTENDED_RUNTIME2_ENABLE_PREVIEW)) {
		isPreviewEnabled = JNI_TRUE;
	}
	return isPreviewEnabled;
}

JNIEXPORT void JNICALL
JVM_VirtualThreadMountBegin(JNIEnv *env, jobject thread, jboolean firstMount)
{
	J9VMThread *currentThread = (J9VMThread *)env;
	J9JavaVM *vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;
	BOOLEAN releasedVirtualThreadInspectorMutex = FALSE;

	vmFuncs->internalEnterVMFromJNI(currentThread);
	j9object_t threadObj = J9_JNI_UNWRAP_REFERENCE(thread);
	assert(IS_VIRTUAL_THREAD(currentThread, threadObj));

	if (firstMount) {
		f_monitorEnter(vm->virtualThreadListMutex);
		while (vm->inspectVirtualThreadList) {
			/* Virtual thread list is being inspected, wait. */
			VM_VMHelpers::pushObjectInSpecialFrame(currentThread, threadObj);
			vmFuncs->internalExitVMToJNI(currentThread);
			f_monitorWait(vm->virtualThreadListMutex);
			vmFuncs->internalEnterVMFromJNI(currentThread);
			threadObj = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
		}
		if (NULL == vm->virtualThreadList) {
			J9Class *virtualThreadClass = J9OBJECT_CLAZZ(currentThread, J9_JNI_UNWRAP_REFERENCE(thread));
			J9MemoryManagerFunctions *mmFuncs = vm->memoryManagerFunctions;

			/* Allocate empty virtual thread and create a global reference to it as root for the linked list.
			 * This prevents the root reference from becoming stale if the GC moves the object.
			 */
			j9object_t rootVirtualThread = mmFuncs->J9AllocateObject(currentThread, virtualThreadClass, J9_GC_ALLOCATE_OBJECT_NON_INSTRUMENTABLE);
			if (NULL != rootVirtualThread) {
				/* The global ref will be freed at vm death. */
				jobject globalRef = vmFuncs->j9jni_createGlobalRef((JNIEnv *)currentThread, rootVirtualThread, JNI_FALSE);
				if (NULL != globalRef) {
					vm->virtualThreadList = (j9object_t *)globalRef;

					/* Set linkNext/linkPrevious to itself. */
					J9OBJECT_OBJECT_STORE(currentThread, rootVirtualThread, vm->virtualThreadLinkNextOffset, rootVirtualThread);
					J9OBJECT_OBJECT_STORE(currentThread, rootVirtualThread, vm->virtualThreadLinkPreviousOffset, rootVirtualThread);
				} else {
					vmFuncs->setNativeOutOfMemoryError(currentThread, 0, 0);
				}
			} else {
				vmFuncs->setHeapOutOfMemoryError(currentThread);
			}
		}
		if (NULL != vm->virtualThreadList) {
			j9object_t root = *(vm->virtualThreadList);
			j9object_t rootPrev = J9OBJECT_OBJECT_LOAD(currentThread, root, vm->virtualThreadLinkPreviousOffset);

			/* Add virtual thread to the end of the list. */
			J9OBJECT_OBJECT_STORE(currentThread, threadObj, vm->virtualThreadLinkNextOffset, root);
			J9OBJECT_OBJECT_STORE(currentThread, threadObj, vm->virtualThreadLinkPreviousOffset, rootPrev);
			J9OBJECT_OBJECT_STORE(currentThread, rootPrev, vm->virtualThreadLinkNextOffset, threadObj);
			J9OBJECT_OBJECT_STORE(currentThread, root, vm->virtualThreadLinkPreviousOffset, threadObj);
		}
		f_monitorExit(vm->virtualThreadListMutex);
	}

retry:
	f_monitorEnter(vm->virtualThreadInspectorMutex);
	while (0 != J9OBJECT_I64_LOAD(currentThread, threadObj, vm->virtualThreadInspectorCountOffset)) {
		/* Virtual thread is being inspected or unmounted, wait. */
		VM_VMHelpers::pushObjectInSpecialFrame(currentThread, threadObj);
		vmFuncs->internalExitVMToJNI(currentThread);
		f_monitorWait(vm->virtualThreadInspectorMutex);
		if (OMR_ARE_ANY_BITS_SET(currentThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND)) {
			f_monitorExit(vm->virtualThreadInspectorMutex);
			releasedVirtualThreadInspectorMutex = TRUE;
		}
		vmFuncs->internalEnterVMFromJNI(currentThread);
		if (releasedVirtualThreadInspectorMutex) {
			f_monitorEnter(vm->virtualThreadInspectorMutex);
			releasedVirtualThreadInspectorMutex = FALSE;
		}
		threadObj = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
	}

	jint vthreadState = J9VMJAVALANGVIRTUALTHREAD_STATE(currentThread, threadObj);
	if (OMR_ARE_ANY_BITS_SET(vthreadState, JVMTI_VTHREAD_STATE_SUSPENDED)) {
		j9object_t oldCarrierThread = J9VMJAVALANGVIRTUALTHREAD_CARRIERTHREAD(currentThread, threadObj);
		J9VMJAVALANGVIRTUALTHREAD_SET_STATE(currentThread, threadObj, vthreadState & ~JVMTI_VTHREAD_STATE_SUSPENDED);
		J9VMJAVALANGVIRTUALTHREAD_SET_CARRIERTHREAD(currentThread, threadObj, currentThread->carrierThreadObject);
		vmFuncs->setHaltFlag(currentThread, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND);
		f_monitorExit(vm->virtualThreadInspectorMutex);
		VM_VMHelpers::pushObjectInSpecialFrame(currentThread, threadObj);
		VM_VMHelpers::pushObjectInSpecialFrame(currentThread, oldCarrierThread);
		vmFuncs->internalExitVMToJNI(currentThread);
		vmFuncs->internalEnterVMFromJNI(currentThread);
		oldCarrierThread = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
		J9VMJAVALANGVIRTUALTHREAD_SET_CARRIERTHREAD(currentThread, threadObj, oldCarrierThread);
		threadObj = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
		goto retry;
	}

	/* Prevent inspectors from inspecting this virtual thread during stack swap and mount by locking from
	 * notifyJvmtiMountBegin to notifyJvmtiMountEnd. See getVMThread() in jvmtiHelpers.c.
	 */
	J9OBJECT_I64_STORE(currentThread, threadObj, vm->virtualThreadInspectorCountOffset, -1);
	f_monitorExit(vm->virtualThreadInspectorMutex);

	vmFuncs->internalExitVMToJNI(currentThread);
}

JNIEXPORT void JNICALL
JVM_VirtualThreadMountEnd(JNIEnv *env, jobject thread, jboolean firstMount)
{
	J9VMThread *currentThread = (J9VMThread *)env;
	J9JavaVM *vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;

	vmFuncs->internalEnterVMFromJNI(currentThread);
	j9object_t threadObj = J9_JNI_UNWRAP_REFERENCE(thread);

	f_monitorEnter(vm->virtualThreadInspectorMutex);
	if (firstMount) {
		TRIGGER_J9HOOK_VM_VIRTUAL_THREAD_STARTED(vm->hookInterface, currentThread);
	}

	/* Allow virtual thread to be inspected again. */
	assert(-1 == J9OBJECT_I64_LOAD(currentThread, threadObj, vm->virtualThreadInspectorCountOffset));
	J9OBJECT_I64_STORE(currentThread, threadObj, vm->virtualThreadInspectorCountOffset, 0);
	f_monitorNotifyAll(vm->virtualThreadInspectorMutex);
	f_monitorExit(vm->virtualThreadInspectorMutex);

	vmFuncs->internalExitVMToJNI(currentThread);
}

JNIEXPORT void JNICALL
JVM_VirtualThreadUnmountBegin(JNIEnv *env, jobject thread, jboolean lastUnmount)
{
	J9VMThread *currentThread = (J9VMThread *)env;
	J9JavaVM *vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;
	BOOLEAN releasedVirtualThreadInspectorMutex = FALSE;

	vmFuncs->internalEnterVMFromJNI(currentThread);
	j9object_t threadObj = J9_JNI_UNWRAP_REFERENCE(thread);

	if (lastUnmount) {
		TRIGGER_J9HOOK_VM_VIRTUAL_THREAD_END(vm->hookInterface, currentThread);
	}

	f_monitorEnter(vm->virtualThreadInspectorMutex);
	while (0 != J9OBJECT_I64_LOAD(currentThread, threadObj, vm->virtualThreadInspectorCountOffset)) {
		/* Virtual thread is being inspected, wait. */
		VM_VMHelpers::pushObjectInSpecialFrame(currentThread, threadObj);
		vmFuncs->internalExitVMToJNI(currentThread);
		f_monitorWait(vm->virtualThreadInspectorMutex);
		if (OMR_ARE_ANY_BITS_SET(currentThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND)) {
			f_monitorExit(vm->virtualThreadInspectorMutex);
			releasedVirtualThreadInspectorMutex = TRUE;
		}
		vmFuncs->internalEnterVMFromJNI(currentThread);
		if (releasedVirtualThreadInspectorMutex) {
			f_monitorEnter(vm->virtualThreadInspectorMutex);
			releasedVirtualThreadInspectorMutex = FALSE;
		}
		threadObj = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
	}

	/* Prevent inspectors from inspecting this virtual thread during stack swap and unmount by locking from
	 * notifyJvmtiUnmountBegin to notifyJvmtiUnmountEnd. See getVMThread() in jvmtiHelpers.c.
	 */
	J9OBJECT_I64_STORE(currentThread, threadObj, vm->virtualThreadInspectorCountOffset, -1);
	f_monitorExit(vm->virtualThreadInspectorMutex);

	vmFuncs->internalExitVMToJNI(currentThread);
}

JNIEXPORT void JNICALL
JVM_VirtualThreadUnmountEnd(JNIEnv *env, jobject thread, jboolean lastUnmount)
{
	J9VMThread *currentThread = (J9VMThread *)env;
	J9JavaVM *vm = currentThread->javaVM;
	J9InternalVMFunctions *vmFuncs = vm->internalVMFunctions;
	BOOLEAN releasedVirtualThreadListMutex = FALSE;

	vmFuncs->internalEnterVMFromJNI(currentThread);

	f_monitorEnter(vm->virtualThreadInspectorMutex);
	j9object_t threadObj = J9_JNI_UNWRAP_REFERENCE(thread);

	if (lastUnmount) {
		vmFuncs->freeTLS(currentThread, threadObj);
		/* CleanupContinuation */
		j9object_t contObj = (j9object_t)J9VMJAVALANGVIRTUALTHREAD_CONT(currentThread, threadObj);
		vmFuncs->freeContinuation(currentThread, contObj);
	}

	/* Allow thread to be inspected again. */
	assert(-1 == J9OBJECT_I64_LOAD(currentThread, threadObj, vm->virtualThreadInspectorCountOffset));
	J9OBJECT_I64_STORE(currentThread, threadObj, vm->virtualThreadInspectorCountOffset, 0);
	f_monitorNotifyAll(vm->virtualThreadInspectorMutex);
	f_monitorExit(vm->virtualThreadInspectorMutex);

	if (lastUnmount) {
		f_monitorEnter(vm->virtualThreadListMutex);
		while (vm->inspectVirtualThreadList) {
			/* Virtual thread list is being inspected, wait. */
			VM_VMHelpers::pushObjectInSpecialFrame(currentThread, threadObj);
			vmFuncs->internalExitVMToJNI(currentThread);
			f_monitorWait(vm->virtualThreadListMutex);
			if (OMR_ARE_ANY_BITS_SET(currentThread->publicFlags, J9_PUBLIC_FLAGS_HALT_THREAD_JAVA_SUSPEND)) {
				f_monitorExit(vm->virtualThreadListMutex);
				releasedVirtualThreadListMutex = TRUE;
			}
			vmFuncs->internalEnterVMFromJNI(currentThread);
			if (releasedVirtualThreadListMutex) {
				f_monitorEnter(vm->virtualThreadListMutex);
				releasedVirtualThreadListMutex = FALSE;
			}
			threadObj = VM_VMHelpers::popObjectInSpecialFrame(currentThread);
		}
		if (NULL != vm->virtualThreadList) {
			j9object_t threadPrev = J9OBJECT_OBJECT_LOAD(currentThread, threadObj, vm->virtualThreadLinkPreviousOffset);
			j9object_t threadNext = J9OBJECT_OBJECT_LOAD(currentThread, threadObj, vm->virtualThreadLinkNextOffset);

			/* Remove virtual thread from list. The root will never be removed. */
			J9OBJECT_OBJECT_STORE(currentThread, threadPrev, vm->virtualThreadLinkNextOffset, threadNext);
			J9OBJECT_OBJECT_STORE(currentThread, threadNext, vm->virtualThreadLinkPreviousOffset, threadPrev);
		}
		f_monitorExit(vm->virtualThreadListMutex);
	}

	vmFuncs->internalExitVMToJNI(currentThread);
}
#endif /* JAVA_SPEC_VERSION >= 19 */

#if JAVA_SPEC_VERSION >= 20
JNIEXPORT jint JNICALL
JVM_GetClassFileVersion(JNIEnv *env, jclass cls)
{
	jint version = 0;
	J9VMThread *currentThread = (J9VMThread *)env;
	J9InternalVMFunctions const * const vmFuncs = currentThread->javaVM->internalVMFunctions;

	vmFuncs->internalEnterVMFromJNI(currentThread);

	if (NULL == cls) {
		vmFuncs->setCurrentException(currentThread, J9VMCONSTANTPOOL_JAVALANGNULLPOINTEREXCEPTION, NULL);
	} else {
		J9Class *clazz = J9VM_J9CLASS_FROM_JCLASS(currentThread, cls);
		J9ROMClass *romClass = clazz->romClass;

		if (J9ROMCLASS_IS_PRIMITIVE_OR_ARRAY(romClass)) {
			version = BCT_JavaMajorVersionShifted(JAVA_SPEC_VERSION) >> BCT_MajorClassFileVersionMaskShift;
		} else {
			version = (jint)((romClass->minorVersion << 16) + romClass->majorVersion);
		}
	}

	vmFuncs->internalExitVMToJNI(currentThread);

	return version;
}
#endif /* JAVA_SPEC_VERSION >= 20 */

} /* extern "C" */
