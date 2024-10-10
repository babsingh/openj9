################################################################################
# Copyright IBM Corp. and others 1991
#
# This program and the accompanying materials are made available under
# the terms of the Eclipse Public License 2.0 which accompanies this
# distribution and is available at https://www.eclipse.org/legal/epl-2.0/
# or the Apache License, Version 2.0 which accompanies this distribution and
# is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# This Source Code may also be made available under the following
# Secondary Licenses when the conditions for such availability set
# forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
# General Public License, version 2 with the GNU Classpath
# Exception [1] and GNU General Public License, version 2 with the
# OpenJDK Assembly Exception [2].
#
# [1] https://www.gnu.org/software/classpath/license.html
# [2] https://openjdk.org/legal/assembly-exception.html
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0-only WITH Classpath-exception-2.0 OR GPL-2.0-only WITH OpenJDK-assembly-exception-1.0
################################################################################

#TODO Platform hacks
set(J9VM_ENV_HAS_FPU ON CACHE BOOL "")

set(OMR_OPT_CUDA OFF CACHE BOOL "")

set(OMR_GC_DYNAMIC_CLASS_UNLOADING ON CACHE BOOL "")

#TODO these options to be properly configured like the rest
set(OMR_JIT ON CACHE BOOL "")
set(OMR_COMPILER ON CACHE BOOL "")
set(OMR_PORT_ASYNC_HANDLER ON CACHE BOOL "")
set(OMR_PORT_CAN_RESERVE_SPECIFIC_ADDRESS ON CACHE BOOL "")
set(OMR_PORT_NUMA_SUPPORT OFF CACHE BOOL "")
set(OMR_THR_JLM ON CACHE BOOL "")
set(OMR_THR_SPIN_WAKE_CONTROL ON CACHE BOOL "")
set(OMR_THR_CUSTOM_SPIN_OPTIONS ON CACHE BOOL "")
set(OMR_NOTIFY_POLICY_CONTROL ON CACHE BOOL "")
set(OMR_THR_THREE_TIER_LOCKING ON CACHE BOOL "")
set(J9VM_BUILD_JAVA7 ON CACHE BOOL "")

set(J9VM_MODULE_ALGORITHM_TEST ON CACHE BOOL "")
set(J9VM_MODULE_BCUTIL ON CACHE BOOL "")
set(J9VM_MODULE_BCVERIFY ON CACHE BOOL "")
set(J9VM_MODULE_CASSUME ON CACHE BOOL "")
set(J9VM_MODULE_CFDUMPER ON CACHE BOOL "")
set(J9VM_MODULE_CODEGEN_COMMON ON CACHE BOOL "")
set(J9VM_MODULE_CODEGEN_ILGEN ON CACHE BOOL "")
set(J9VM_MODULE_CODEGEN_OPT ON CACHE BOOL "")
set(J9VM_MODULE_CODERT_COMMON ON CACHE BOOL "")
set(J9VM_MODULE_CODERT_VM ON CACHE BOOL "")
set(J9VM_MODULE_DBGEXT ON CACHE BOOL "")
set(J9VM_MODULE_EXELIB ON CACHE BOOL "")

set(J9VM_MODULE_GC ON CACHE BOOL "")
set(J9VM_MODULE_GC_API ON CACHE BOOL "")
set(J9VM_MODULE_GC_BASE ON CACHE BOOL "")
set(J9VM_MODULE_GC_CHECK ON CACHE BOOL "")
set(J9VM_MODULE_GC_COMMON OFF CACHE BOOL "")
set(J9VM_MODULE_GC_MODRON_BASE ON CACHE BOOL "")
set(J9VM_MODULE_GC_MODRON_EPROF ON CACHE BOOL "")
set(J9VM_MODULE_GC_MODRON_STANDARD ON CACHE BOOL "")
set(J9VM_MODULE_GC_MODRON_STARTUP ON CACHE BOOL "")
set(J9VM_MODULE_GC_REALTIME ON CACHE BOOL "")
set(J9VM_MODULE_GC_STATS ON CACHE BOOL "")
set(J9VM_MODULE_GC_STRUCTS ON CACHE BOOL "")
set(J9VM_MODULE_GC_TRACE ON CACHE BOOL "")
set(J9VM_MODULE_GCCHK ON CACHE BOOL "")
set(J9VM_MODULE_GDB ON CACHE BOOL "")
set(J9VM_MODULE_GDB_PLUGIN ON CACHE BOOL "")
set(J9VM_MODULE_GPTEST ON CACHE BOOL "")
set(J9VM_MODULE_HOOKABLE ON CACHE BOOL "")
set(J9VM_MODULE_J9JIT_VM ON CACHE BOOL "")
set(J9VM_MODULE_J9VM ON CACHE BOOL "")
set(J9VM_MODULE_J9VMTEST ON CACHE BOOL "")
set(J9VM_MODULE_JEXTRACTNATIVES ON CACHE BOOL "")

set(J9VM_MODULE_JIT_COMMON ON CACHE BOOL "")
set(J9VM_MODULE_JIT_VM ON CACHE BOOL "")
set(J9VM_MODULE_JITRT_COMMON ON CACHE BOOL "")
set(J9VM_MODULE_JNIARGTESTS ON CACHE BOOL "")
set(J9VM_MODULE_JNICHK ON CACHE BOOL "")
set(J9VM_MODULE_JNIINV ON CACHE BOOL "")
set(J9VM_MODULE_JNITEST ON CACHE BOOL "")
set(J9VM_MODULE_JVMTI ON CACHE BOOL "")
set(J9VM_MODULE_JVMTITST ON CACHE BOOL "")
set(J9VM_MODULE_LIFECYCLE_TESTS ON CACHE BOOL "")
set(J9VM_MODULE_PORT ON CACHE BOOL "")
set(J9VM_MODULE_PORTTEST ON CACHE BOOL "")
set(J9VM_MODULE_RASDUMP ON CACHE BOOL "")
set(J9VM_MODULE_RASTRACE ON CACHE BOOL "")
set(J9VM_MODULE_SHARED ON CACHE BOOL "")
set(J9VM_MODULE_SHARED_COMMON ON CACHE BOOL "")
set(J9VM_MODULE_SHARED_TEST ON CACHE BOOL "")
set(J9VM_MODULE_SHARED_UTIL ON CACHE BOOL "")
set(J9VM_MODULE_SIMPLEPOOL ON CACHE BOOL "")
set(J9VM_MODULE_STACKMAP ON CACHE BOOL "")
set(J9VM_MODULE_THREAD ON CACHE BOOL "")
set(J9VM_MODULE_UTIL ON CACHE BOOL "")
set(J9VM_MODULE_VERBOSE ON CACHE BOOL "")
set(J9VM_MODULE_VERUTIL ON CACHE BOOL "")
set(J9VM_MODULE_VM ON CACHE BOOL "")
set(J9VM_MODULE_ZIP ON CACHE BOOL "")
set(J9VM_MODULE_ZLIB ON CACHE BOOL "")

set(J9VM_GC_ADAPTIVE_TENURING ON CACHE BOOL "")
set(J9VM_GC_ALLOCATION_TAX ON CACHE BOOL "")
set(J9VM_GC_BATCH_CLEAR_TLH ON CACHE BOOL "")
set(J9VM_GC_COMBINATION_SPEC ON CACHE BOOL "")
set(J9VM_GC_CONCURRENT_SWEEP ON CACHE BOOL "")
set(J9VM_GC_DEBUG_ASSERTS ON CACHE BOOL "")
set(J9VM_GC_DYNAMIC_CLASS_UNLOADING ON CACHE BOOL "")
set(J9VM_GC_DYNAMIC_NEW_SPACE_SIZING ON CACHE BOOL "")
set(J9VM_GC_FINALIZATION ON CACHE BOOL "")
set(J9VM_GC_FRAGMENTED_HEAP ON CACHE BOOL "")
set(J9VM_GC_GENERATIONAL ON CACHE BOOL "")
set(J9VM_GC_HEAP_CARD_TABLE ON CACHE BOOL "")
set(J9VM_GC_INLINED_ALLOC_FIELDS ON CACHE BOOL "")
set(J9VM_GC_JNI_ARRAY_CACHE ON CACHE BOOL "")
set(J9VM_GC_LARGE_OBJECT_AREA ON CACHE BOOL "")
set(J9VM_GC_LEAF_BITS ON CACHE BOOL "")
set(J9VM_GC_MINIMUM_OBJECT_SIZE ON CACHE BOOL "")
set(J9VM_GC_MODRON_COMPACTION ON CACHE BOOL "")
set(J9VM_GC_MODRON_CONCURRENT_MARK ON CACHE BOOL "")
set(J9VM_GC_MODRON_GC ON CACHE BOOL "")
set(J9VM_GC_MODRON_SCAVENGER ON CACHE BOOL "")
set(J9VM_GC_MODRON_STANDARD ON CACHE BOOL "")
set(J9VM_GC_MODRON_TRACE ON CACHE BOOL "")
set(J9VM_GC_MODRON_VERBOSE ON CACHE BOOL "")
set(J9VM_GC_NON_ZERO_TLH ON CACHE BOOL "")
set(J9VM_GC_OBJECT_ACCESS_BARRIER ON CACHE BOOL "")
set(J9VM_GC_REALTIME ON CACHE BOOL "")
set(J9VM_GC_SEGREGATED_HEAP ON CACHE BOOL "")
set(J9VM_GC_THREAD_LOCAL_HEAP ON CACHE BOOL "")
set(J9VM_GC_TILTED_NEW_SPACE ON CACHE BOOL "")
set(J9VM_GC_TLH_PREFETCH_FTA ON CACHE BOOL "")
set(J9VM_GC_USE_INLINE_ALLOCATE ON CACHE BOOL "")
set(J9VM_GC_VLHGC ON CACHE BOOL "")
set(J9VM_INTERP_AOT_COMPILE_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_AOT_RUNTIME_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_BYTECODE_PREVERIFICATION ON CACHE BOOL "")
set(J9VM_INTERP_BYTECODE_VERIFICATION ON CACHE BOOL "")
set(J9VM_INTERP_CUSTOM_SPIN_OPTIONS ON CACHE BOOL "")
set(J9VM_INTERP_DEBUG_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_ENABLE_JIT_ON_DESKTOP ON CACHE BOOL "")
set(J9VM_INTERP_FLAGS_IN_CLASS_SLOT ON CACHE BOOL "")
set(J9VM_INTERP_FLOAT_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_GP_HANDLER ON CACHE BOOL "")
set(J9VM_INTERP_GROWABLE_STACKS ON CACHE BOOL "")
set(J9VM_INTERP_HOT_CODE_REPLACEMENT ON CACHE BOOL "")
set(J9VM_INTERP_JIT_ON_BY_DEFAULT ON CACHE BOOL "")
set(J9VM_INTERP_JNI_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_NATIVE_SUPPORT ON CACHE BOOL "")
set(J9VM_INTERP_NEW_HEADER_SHAPE ON CACHE BOOL "")
set(J9VM_INTERP_PROFILING_BYTECODES ON CACHE BOOL "")
set(J9VM_INTERP_SIG_QUIT_THREAD ON CACHE BOOL "")
set(J9VM_INTERP_SIG_QUIT_THREAD_USES_SEMAPHORES ON CACHE BOOL "")
set(J9VM_INTERP_SIG_USR2 ON CACHE BOOL "")
set(J9VM_INTERP_USE_SPLIT_SIDE_TABLES ON CACHE BOOL "")
set(J9VM_INTERP_USE_UNSAFE_HELPER ON CACHE BOOL "")
set(J9VM_INTERP_VERBOSE ON CACHE BOOL "")
set(J9VM_IVE_JXE_FILE_RELOCATOR ON CACHE BOOL "")
set(J9VM_IVE_JXE_IN_PLACE_RELOCATOR ON CACHE BOOL "")
set(J9VM_IVE_JXE_NATIVES ON CACHE BOOL "")
set(J9VM_IVE_JXE_OERELOCATOR ON CACHE BOOL "")
set(J9VM_IVE_JXE_STREAMING_RELOCATOR ON CACHE BOOL "")
set(J9VM_IVE_ROM_IMAGE_HELPERS ON CACHE BOOL "")
set(J9VM_JIT_C_HELPERS ON CACHE BOOL "")
set(J9VM_JIT_CLASS_UNLOAD_RWMONITOR ON CACHE BOOL "")
set(J9VM_JIT_DYNAMIC_LOOP_TRANSFER ON CACHE BOOL "")
set(J9VM_JIT_FULL_SPEED_DEBUG ON CACHE BOOL "")
set(J9VM_JIT_GC_ON_RESOLVE_SUPPORT ON CACHE BOOL "")
set(J9VM_JIT_NEW_DUAL_HELPERS ON CACHE BOOL "")
set(J9VM_JIT_NEW_INSTANCE_PROTOTYPE ON CACHE BOOL "")
set(J9VM_JIT_SUPPORTS_DIRECT_JNI ON CACHE BOOL "")
set(J9VM_OPT_ANNOTATIONS ON CACHE BOOL "")
set(J9VM_OPT_BIG_INTEGER ON CACHE BOOL "")
set(J9VM_OPT_DEBUG_INFO_SERVER ON CACHE BOOL "")
set(J9VM_OPT_DEBUG_JSR45_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_DEPRECATED_METHODS ON CACHE BOOL "")
set(J9VM_OPT_DYNAMIC_LOAD_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_FRAGMENT_RAM_CLASSES ON CACHE BOOL "")
set(J9VM_OPT_INLINE_JSRS ON CACHE BOOL "")
set(J9VM_OPT_INVARIANT_INTERNING ON CACHE BOOL "")
set(J9VM_OPT_JITSERVER OFF CACHE BOOL "Enable JITServer")
set(J9VM_OPT_JVMTI ON CACHE BOOL "")
set(J9VM_OPT_JXE_LOAD_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_MEMORY_CHECK_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_METHOD_HANDLE ON CACHE BOOL "")
set(J9VM_OPT_METHOD_HANDLE_COMMON ON CACHE BOOL "")
set(J9VM_OPT_MULTI_VM ON CACHE BOOL "")
set(J9VM_OPT_NEW_OBJECT_HASH ON CACHE BOOL "")
set(J9VM_OPT_NEW_ROM_CLASS_BUILDER ON CACHE BOOL "")
set(J9VM_OPT_OPENJDK_METHODHANDLE OFF CACHE BOOL "")
set(J9VM_OPT_REFLECT ON CACHE BOOL "")
set(J9VM_OPT_ROM_IMAGE_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_SHARED_CLASSES ON CACHE BOOL "")
set(J9VM_OPT_SNAPSHOTS ON CACHE BOOL "")
set(J9VM_OPT_SIDECAR ON CACHE BOOL "")
set(J9VM_OPT_SRP_AVL_TREE_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_STRING_COMPRESSION ON CACHE BOOL "")
set(J9VM_OPT_USE_FFI ON CACHE BOOL "")
set(J9VM_OPT_USE_FFI_ONLY ON CACHE BOOL "")
set(J9VM_OPT_VM_LOCAL_STORAGE ON CACHE BOOL "")
set(J9VM_OPT_ZIP_SUPPORT ON CACHE BOOL "")
set(J9VM_OPT_ZLIB_COMPRESSION ON CACHE BOOL "")
set(J9VM_OPT_ZLIB_SUPPORT ON CACHE BOOL "")
set(J9VM_PORT_OMRSIG_SUPPORT ON CACHE BOOL "")
set(J9VM_PORT_SIGNAL_SUPPORT ON CACHE BOOL "")
set(J9VM_PROF_EVENT_REPORTING ON CACHE BOOL "")
set(J9VM_SIZE_OPTIMIZE_SEND_TARGETS ON CACHE BOOL "")
set(J9VM_THR_ASYNC_NAME_UPDATE ON CACHE BOOL "")
set(J9VM_THR_LOCK_NURSERY ON CACHE BOOL "")
set(J9VM_THR_LOCK_RESERVATION ON CACHE BOOL "")
set(J9VM_THR_PREEMPTIVE ON CACHE BOOL "")
set(J9VM_THR_SMART_DEFLATION ON CACHE BOOL "")
set(J9VM_OPT_OPENJDK_FFI ON CACHE BOOL "")
