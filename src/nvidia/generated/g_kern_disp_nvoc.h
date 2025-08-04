
#ifndef _G_KERN_DISP_NVOC_H_
#define _G_KERN_DISP_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include "g_kern_disp_nvoc.h"

#ifndef KERN_DISP_H
#define KERN_DISP_H

/******************************************************************************
*
*       Kernel Display module header
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/disp/kern_disp_type.h"
#include "gpu/disp/kern_disp_max.h"
#include "gpu/mem_mgr/context_dma.h"
#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/disp/head/kernel_head.h"


#include "kernel/gpu/intr/intr_service.h"

#include "utils/nvbitvector.h"
TYPEDEF_BITVECTOR(MC_ENGINE_BITVECTOR);

#include "ctrl/ctrl2080/ctrl2080internal.h"

typedef NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS KernelDisplayStaticInfo;

typedef struct
{
    NvU32 kHeadVblankCount[OBJ_MAX_HEADS];
} KernelDisplaySharedMem;

// From DISP_v0501 NvDisplay HW IP has 4 interrupt vectors: high latency, low latency, GSP, PMU.
typedef enum
{
    DISP_INTERRUPT_VECTOR_HIGH_LATENCY = 0,
    DISP_INTERRUPT_VECTOR_LOW_LATENCY,
    DISP_INTERRUPT_VECTOR_PMU,
    DISP_INTERRUPT_VECTOR_GSP,
} DISP_INTERRUPT_VECTOR;

typedef struct
{
// Link configuration
    NvU32   linkBw;                     // Link Rate (270M unit)
    NvU32   dp2LinkBw;                  // Link Rate using DP2.x convention (10M unit)
    NvU32   laneCount;                  // Lane Count
    NvBool  bDP2xChannelCoding;         // If is using 128b/132b channel coding.
    NvBool  bEnhancedFraming;           // Enhanced Framing is enabled or not
    NvBool  bMultiStream;               // Multistream is enabled or not
    NvBool  bFecEnable;                 // FEC is enabled or not
    NvBool  bDisableEffBppSST8b10b;     // Disable effective bpp 8b10b coding or not

// DSC Information
    NvBool  bDscEnable;                 // DSC is enabled or not
    NvU32   sliceCount;
    NvU32   sliceWidth;
    NvU32   sliceHeight;
    NvU32   dscVersionMajor;
    NvU32   dscVersionMinor;

// Mode information.
    NvU64   PClkFreqHz;                 // Pixel clock in Hz
    NvU32   bpp;                        // Output bits per pixel
    NvU32   SetRasterSizeWidth;         // Raster Size width
    NvU32   SetRasterBlankStartX;       // Raster Blank Start X
    NvU32   SetRasterBlankEndX;         // Raster Blank End X
    NvU32   twoChannelAudioHz;
    NvU32   eightChannelAudioHz;
    NvU32   colorFormat;                // Same as DP_COLORFORMAT
} DPMODESETDATA;

// bpp in PPS is multiplied by 16 when DSC is enabled
#define DSC_BPP_FACTOR 16

typedef struct
{
    struct RsClient *pClient;
    NvHandle hChannel;
    NvU32    channelNum;
    NvBool   bInUse;
} KernelDisplayClientChannelMap;

typedef void (*OSVBLANKCALLBACKPROC)(NvP64 pParm1, NvP64 pParm2);

typedef struct _osvblankcallback {
    OSVBLANKCALLBACKPROC pProc;
    void * pParm1;
    void * pParm2;
    void * pCallback;
    void * pParm3;
} OSVBLANKCALLBACK, * POSVBLANKCALLBACK;

#define DISP_INTR_REG(reg)          NV_PDISP_FE_RM_INTR_##reg
#define DISP_INTR_REG_IDX(reg,i)    NV_PDISP_FE_RM_INTR_##reg(i)


struct DispChannel;

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */



struct RgLineCallback;

#ifndef __NVOC_CLASS_RgLineCallback_TYPEDEF__
#define __NVOC_CLASS_RgLineCallback_TYPEDEF__
typedef struct RgLineCallback RgLineCallback;
#endif /* __NVOC_CLASS_RgLineCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1c
#endif /* __nvoc_class_id_RgLineCallback */



#define KDISP_GET_HEAD(pKernelDisplay, headID)    (RMCFG_MODULE_KERNEL_HEAD ? kdispGetHead(pKernelDisplay, headID) : NULL)

/*!
 * KernelDisp is a logical abstraction of the GPU Display Engine. The
 * Public API of the Display Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Display hardware can be
 * managed by this object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_DISP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelDisplay;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__IntrService;
struct NVOC_VTABLE__KernelDisplay;


struct KernelDisplay {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelDisplay *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelDisplay *__nvoc_pbase_KernelDisplay;    // kdisp

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__kdispSelectClass__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // halified (2 hals) body
    NvS32 (*__kdispGetBaseOffset__)(OBJGPU *, struct KernelDisplay * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kdispGetChannelNum__)(struct KernelDisplay * /*this*/, DISPCHNCLASS, NvU32, NvU32 *);  // halified (2 hals) body
    void (*__kdispGetDisplayCapsBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kdispGetDisplaySfUserBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispGetDisplayChannelUserBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, DISPCHNCLASS, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispImportImpData__)(struct KernelDisplay * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kdispArbAndAllocDisplayBandwidth__)(OBJGPU *, struct KernelDisplay * /*this*/, enum DISPLAY_ICC_BW_CLIENT, NvU32, NvU32);  // halified (2 hals) body
    NvBool (*__kdispGetVgaWorkspaceBase__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU64 *);  // halified (3 hals) body
    void (*__kdispInvokeDisplayModesetCallback__)(struct KernelDisplay * /*this*/, NvBool, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispReadRgLineCountAndFrameCount__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispInitBrightcStateLoad__)(OBJGPU *, struct KernelDisplay * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kdispSetupAcpiEdid__)(OBJGPU *, struct KernelDisplay * /*this*/);  // halified (2 hals) body
    void (*__kdispRestoreOriginalLsrMinTime__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispComputeLsrMinTimeValue__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32, NvU32 *);  // halified (2 hals) body
    void (*__kdispSetSwapBarrierLsrMinTime__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispGetRgScanLock__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, OBJGPU *, NvU32, NvBool *, NvU32 *, NvBool *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispDetectSliLink__)(struct KernelDisplay * /*this*/, OBJGPU *, OBJGPU *, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispReadAwakenChannelNumMask__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32 *, DISPCHNCLASS, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    NvU32 (*__kdispGetPBTargetAperture__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kdispAllocateSharedMem__)(OBJGPU *, struct KernelDisplay * /*this*/);  // halified (2 hals) body
    NvBool (*__kdispReadPendingWinSemIntr__)(OBJGPU *, struct KernelDisplay * /*this*/, struct THREAD_STATE_NODE *, NvU32 *);  // halified (2 hals) body
    void (*__kdispHandleWinSemEvt__)(OBJGPU *, struct KernelDisplay * /*this*/, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__kdispIntrRetrigger__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    NV_STATUS (*__kdispComputeDpModeSettings__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, DPMODESETDATA *, DPIMPINFO *);  // halified (3 hals) body
    NvU32 (*__kdispServiceAwakenIntr__)(OBJGPU *, struct KernelDisplay * /*this*/, struct THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__kdispSetChannelTrashAndAbortAccel__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32, NvBool);  // halified (2 hals)
    NvBool (*__kdispIsChannelIdle__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (2 hals)
    void (*__kdispApplyChannelConnectDisconnect__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32, NvU32);  // halified (2 hals)
    NvBool (*__kdispIsChannelAllocatedHw__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (2 hals)

    // 8 PDB properties
//  NvBool PDB_PROP_KDISP_IS_MISSING inherited from OBJENGSTATE
    NvBool PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF;
    NvBool PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE;
    NvBool PDB_PROP_KDISP_IN_AWAKEN_INTR;
    NvBool PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE;
    NvBool PDB_PROP_KDISP_INTERNAL_PANEL_DISCONNECTED;
    NvBool PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE;
    NvBool PDB_PROP_KDISP_AGGRESSIVE_VBLANK_HANDLING;

    // Data members
    struct DisplayInstanceMemory *pInst;
    struct KernelHead *pKernelHead[8];
    const KernelDisplayStaticInfo *pStaticInfo;
    NvBool bWarPurgeSatellitesOnCoreFree;
    struct RgLineCallback *rgLineCallbackPerHead[8][2];
    NvU32 isrVblankHeads;
    NvBool bExtdevIntrSupported;
    NvU32 numHeads;
    NvU32 deferredVblankHeadMask;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hDispCommonHandle;
    MEMORY_DESCRIPTOR *pSharedMemDesc;
    KernelDisplaySharedMem *pSharedData;
    NvBool bFeatureStretchVblankCapable;
    volatile NvS32 lowLatencyLock;
    NvU32 vblankCallbackHeadMask;
    POSVBLANKCALLBACK pOsVblankCallback;
    NvU32 numDispChannels;
    KernelDisplayClientChannelMap *pClientChannelTable;
    NvBool bIsPanelReplayEnabled;
    void *pRgVblankCb;
};


// Vtable with 18 per-class function pointers
struct NVOC_VTABLE__KernelDisplay {
    NV_STATUS (*__kdispConstructEngine__)(OBJGPU *, struct KernelDisplay * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreInitLocked__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateInitLocked__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kdispStateDestroy__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateLoad__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateUnload__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kdispRegisterIntrService__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__kdispServiceInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv)
    void (*__kdispInitMissing__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreInitUnlocked__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStateInitUnlocked__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreLoad__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePostLoad__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreUnload__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePostUnload__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kdispIsPresent__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kdispClearInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NV_STATUS (*__kdispServiceNotificationInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelDisplay {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_VTABLE__KernelDisplay vtable;
};

#ifndef __NVOC_CLASS_KernelDisplay_TYPEDEF__
#define __NVOC_CLASS_KernelDisplay_TYPEDEF__
typedef struct KernelDisplay KernelDisplay;
#endif /* __NVOC_CLASS_KernelDisplay_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelDisplay
#define __nvoc_class_id_KernelDisplay 0x55952e
#endif /* __nvoc_class_id_KernelDisplay */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay;

#define __staticCast_KernelDisplay(pThis) \
    ((pThis)->__nvoc_pbase_KernelDisplay)

#ifdef __nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) ((KernelDisplay*) NULL)
#else //__nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) \
    ((KernelDisplay*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelDisplay)))
#endif //__nvoc_kern_disp_h_disabled

// Property macros
#define PDB_PROP_KDISP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KDISP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF_BASE_CAST
#define PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF_BASE_NAME PDB_PROP_KDISP_IMP_ALLOC_BW_IN_KERNEL_RM_DEF
#define PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE_BASE_CAST
#define PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE_BASE_NAME PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_CAST
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_NAME PDB_PROP_KDISP_IN_AWAKEN_INTR
#define PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE_BASE_CAST
#define PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE_BASE_NAME PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE
#define PDB_PROP_KDISP_INTERNAL_PANEL_DISCONNECTED_BASE_CAST
#define PDB_PROP_KDISP_INTERNAL_PANEL_DISCONNECTED_BASE_NAME PDB_PROP_KDISP_INTERNAL_PANEL_DISCONNECTED
#define PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE_BASE_CAST
#define PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE_BASE_NAME PDB_PROP_KDISP_ENABLE_INLINE_INTR_SERVICE
#define PDB_PROP_KDISP_AGGRESSIVE_VBLANK_HANDLING_BASE_CAST
#define PDB_PROP_KDISP_AGGRESSIVE_VBLANK_HANDLING_BASE_NAME PDB_PROP_KDISP_AGGRESSIVE_VBLANK_HANDLING


NV_STATUS __nvoc_objCreateDynamic_KernelDisplay(KernelDisplay**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelDisplay(KernelDisplay**, Dynamic*, NvU32);
#define __objCreate_KernelDisplay(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelDisplay((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros for implementation functions
void kdispDestruct_IMPL(struct KernelDisplay *pKernelDisplay);
#define __nvoc_kdispDestruct(pKernelDisplay) kdispDestruct_IMPL(pKernelDisplay)

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispServiceLowLatencyIntrs(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5, NvU32 *pIntrServicedHeadMask, MC_ENGINE_BITVECTOR *pIntrPending) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispServiceLowLatencyIntrs(pGpu, pKernelDisplay, arg3, arg4, arg5, pIntrServicedHeadMask, pIntrPending) kdispServiceLowLatencyIntrs_KERNEL(pGpu, pKernelDisplay, arg3, arg4, arg5, pIntrServicedHeadMask, pIntrPending)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispConstructInstMem(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispConstructInstMem(pKernelDisplay) kdispConstructInstMem_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestructInstMem(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispDestructInstMem(pKernelDisplay) kdispDestructInstMem_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

NV_STATUS kdispConstructKhead_IMPL(struct KernelDisplay *pKernelDisplay);
#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispConstructKhead(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispConstructKhead(pKernelDisplay) kdispConstructKhead_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

void kdispDestructKhead_IMPL(struct KernelDisplay *pKernelDisplay);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestructKhead(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispDestructKhead(pKernelDisplay) kdispDestructKhead_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispSetPushBufferParamsToPhysical(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvHandle hObjectBuffer, struct ContextDma *pBufferContextDma, NvU32 hClass, NvU32 channelInstance, DISPCHNCLASS internalDispChnClass, ChannelPBSize channelPBSize, NvU32 subDeviceId) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispSetPushBufferParamsToPhysical(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass, channelPBSize, subDeviceId) kdispSetPushBufferParamsToPhysical_IMPL(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass, channelPBSize, subDeviceId)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispAcquireDispChannelHw(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvU32 channelInstance, NvHandle hObjectBuffer, NvU32 initialGetPutOffset, NvBool allowGrabWithinSameClient, NvBool connectPbAtGrab) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispAcquireDispChannelHw(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab) kdispAcquireDispChannelHw_56cd7a(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispReleaseDispChannelHw(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispReleaseDispChannelHw(pKernelDisplay, pDispChannel) kdispReleaseDispChannelHw_56cd7a(pKernelDisplay, pDispChannel)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispMapDispChannel(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispMapDispChannel(pKernelDisplay, pDispChannel) kdispMapDispChannel_IMPL(pKernelDisplay, pDispChannel)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispUnbindUnmapDispChannel(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispUnbindUnmapDispChannel(pKernelDisplay, pDispChannel) kdispUnbindUnmapDispChannel_IMPL(pKernelDisplay, pDispChannel)
#endif // __nvoc_kern_disp_h_disabled

NV_STATUS kdispGetIntChnClsForHwCls_IMPL(struct KernelDisplay *pKernelDisplay, NvU32 hwClass, DISPCHNCLASS *pDispChnClass);
#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispGetIntChnClsForHwCls(struct KernelDisplay *pKernelDisplay, NvU32 hwClass, DISPCHNCLASS *pDispChnClass) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispGetIntChnClsForHwCls(pKernelDisplay, hwClass, pDispChnClass) kdispGetIntChnClsForHwCls_IMPL(pKernelDisplay, hwClass, pDispChnClass)
#endif // __nvoc_kern_disp_h_disabled

void kdispNotifyCommonEvent_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispNotifyCommonEvent(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispNotifyCommonEvent(pGpu, pKernelDisplay, notifyIndex, pNotifyParams) kdispNotifyCommonEvent_IMPL(pGpu, pKernelDisplay, notifyIndex, pNotifyParams)
#endif // __nvoc_kern_disp_h_disabled

void kdispNotifyEvent_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispNotifyEvent(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispNotifyEvent(pGpu, pKernelDisplay, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16) kdispNotifyEvent_IMPL(pGpu, pKernelDisplay, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16)
#endif // __nvoc_kern_disp_h_disabled

NV_STATUS kdispOptimizePerFrameOsCallbacks_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvBool clearIntr, struct THREAD_STATE_NODE *pThreadState, NvU32 *intrServicedHeadMask, MC_ENGINE_BITVECTOR *intrPending);
#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispOptimizePerFrameOsCallbacks(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvBool clearIntr, struct THREAD_STATE_NODE *pThreadState, NvU32 *intrServicedHeadMask, MC_ENGINE_BITVECTOR *intrPending) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispOptimizePerFrameOsCallbacks(pGpu, pKernelDisplay, clearIntr, pThreadState, intrServicedHeadMask, intrPending) kdispOptimizePerFrameOsCallbacks_IMPL(pGpu, pKernelDisplay, clearIntr, pThreadState, intrServicedHeadMask, intrPending)
#endif // __nvoc_kern_disp_h_disabled

NV_STATUS kdispSetupVBlank_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pProc, void *pParm1, void *pParm2, NvU32 Head, void *pParm3);
#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispSetupVBlank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pProc, void *pParm1, void *pParm2, NvU32 Head, void *pParm3) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispSetupVBlank(pGpu, pKernelDisplay, pProc, pParm1, pParm2, Head, pParm3) kdispSetupVBlank_IMPL(pGpu, pKernelDisplay, pProc, pParm1, pParm2, Head, pParm3)
#endif // __nvoc_kern_disp_h_disabled

void kdispDestroyVBlank_IMPL(struct KernelDisplay *pKernelDisplay);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestroyVBlank(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispDestroyVBlank(pKernelDisplay) kdispDestroyVBlank_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

void kdispHandleAggressiveVblank_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3, MC_ENGINE_BITVECTOR *arg4);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispHandleAggressiveVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3, MC_ENGINE_BITVECTOR *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispHandleAggressiveVblank(pGpu, pKernelDisplay, arg3, arg4) kdispHandleAggressiveVblank_IMPL(pGpu, pKernelDisplay, arg3, arg4)
#endif // __nvoc_kern_disp_h_disabled

void kdispApplyAggressiveVblankHandlingWar_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispApplyAggressiveVblankHandlingWar(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispApplyAggressiveVblankHandlingWar(pGpu, pKernelDisplay) kdispApplyAggressiveVblankHandlingWar_IMPL(pGpu, pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

void kdispSetWarPurgeSatellitesOnCoreFree_IMPL(struct KernelDisplay *pKernelDisplay, NvBool value);
#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispSetWarPurgeSatellitesOnCoreFree(struct KernelDisplay *pKernelDisplay, NvBool value) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispSetWarPurgeSatellitesOnCoreFree(pKernelDisplay, value) kdispSetWarPurgeSatellitesOnCoreFree_IMPL(pKernelDisplay, value)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispRegisterRgLineCallback(struct KernelDisplay *pKernelDisplay, struct RgLineCallback *pRgLineCallback, NvU32 head, NvU32 rgIntrLine, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispRegisterRgLineCallback(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable) kdispRegisterRgLineCallback_IMPL(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispInvokeRgLineCallback(struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 rgIntrLine, NvBool bIsIrqlIsr) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispInvokeRgLineCallback(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr) kdispInvokeRgLineCallback_KERNEL(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NvU32 kdispReadPendingVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispReadPendingVblank(pGpu, pKernelDisplay, arg3) kdispReadPendingVblank_IMPL(pGpu, pKernelDisplay, arg3)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispDsmMxmMxcbExecuteAcpi(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pInOutData, NvU16 *outDataSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispDsmMxmMxcbExecuteAcpi(pGpu, pKernelDisplay, pInOutData, outDataSize) kdispDsmMxmMxcbExecuteAcpi_92bfc3(pGpu, pKernelDisplay, pInOutData, outDataSize)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispAllocateCommonHandle(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispAllocateCommonHandle(pGpu, pKernelDisplay) kdispAllocateCommonHandle_IMPL(pGpu, pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestroyCommonHandle(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispDestroyCommonHandle(pKernelDisplay) kdispDestroyCommonHandle_IMPL(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispFreeSharedMem(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispFreeSharedMem(pGpu, pKernelDisplay) kdispFreeSharedMem_IMPL(pGpu, pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NvBool kdispIsDisplayConnected(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispIsDisplayConnected(pGpu, pKernelDisplay) kdispIsDisplayConnected_IMPL(pGpu, pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline NvU32 kdispGetSupportedDisplayMask(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else // __nvoc_kern_disp_h_disabled
#define kdispGetSupportedDisplayMask(pGpu, pKernelDisplay) kdispGetSupportedDisplayMask_IMPL(pGpu, pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispUpdatePdbAfterIpHalInit(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else // __nvoc_kern_disp_h_disabled
#define kdispUpdatePdbAfterIpHalInit(pKernelDisplay) kdispUpdatePdbAfterIpHalInit_b3696a(pKernelDisplay)
#endif // __nvoc_kern_disp_h_disabled


// Wrapper macros for halified functions
#define kdispConstructEngine_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispConstructEngine__
#define kdispConstructEngine(pGpu, pKernelDisplay, engDesc) kdispConstructEngine_DISPATCH(pGpu, pKernelDisplay, engDesc)
#define kdispStatePreInitLocked_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStatePreInitLocked__
#define kdispStatePreInitLocked(pGpu, pKernelDisplay) kdispStatePreInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateInitLocked_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateInitLocked__
#define kdispStateInitLocked(pGpu, pKernelDisplay) kdispStateInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateDestroy_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateDestroy__
#define kdispStateDestroy(pGpu, pKernelDisplay) kdispStateDestroy_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateLoad_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateLoad__
#define kdispStateLoad(pGpu, pKernelDisplay, flags) kdispStateLoad_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispStateUnload_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateUnload__
#define kdispStateUnload(pGpu, pKernelDisplay, flags) kdispStateUnload_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispServiceLowLatencyIntrs_HAL(pGpu, pKernelDisplay, arg3, arg4, arg5, pIntrServicedHeadMask, pIntrPending) kdispServiceLowLatencyIntrs(pGpu, pKernelDisplay, arg3, arg4, arg5, pIntrServicedHeadMask, pIntrPending)
#define kdispRegisterIntrService_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispRegisterIntrService__
#define kdispRegisterIntrService(pGpu, pKernelDisplay, pRecords) kdispRegisterIntrService_DISPATCH(pGpu, pKernelDisplay, pRecords)
#define kdispServiceInterrupt_FNPTR(pKernelDisplay) pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispServiceInterrupt__
#define kdispServiceInterrupt(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispServiceInterrupt_HAL(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispConstructInstMem_HAL(pKernelDisplay) kdispConstructInstMem(pKernelDisplay)
#define kdispDestructInstMem_HAL(pKernelDisplay) kdispDestructInstMem(pKernelDisplay)
#define kdispSelectClass_FNPTR(pKernelDisplay) pKernelDisplay->__kdispSelectClass__
#define kdispSelectClass(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispSelectClass_HAL(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispGetBaseOffset_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetBaseOffset__
#define kdispGetBaseOffset(pGpu, pKernelDisplay) kdispGetBaseOffset_DISPATCH(pGpu, pKernelDisplay)
#define kdispGetBaseOffset_HAL(pGpu, pKernelDisplay) kdispGetBaseOffset_DISPATCH(pGpu, pKernelDisplay)
#define kdispGetChannelNum_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetChannelNum__
#define kdispGetChannelNum(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetDisplayCapsBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplayCapsBaseAndSize__
#define kdispGetDisplayCapsBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayCapsBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplaySfUserBaseAndSize__
#define kdispGetDisplaySfUserBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplayChannelUserBaseAndSize__
#define kdispGetDisplayChannelUserBaseAndSize(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispImportImpData_FNPTR(pKernelDisplay) pKernelDisplay->__kdispImportImpData__
#define kdispImportImpData(pKernelDisplay) kdispImportImpData_DISPATCH(pKernelDisplay)
#define kdispImportImpData_HAL(pKernelDisplay) kdispImportImpData_DISPATCH(pKernelDisplay)
#define kdispArbAndAllocDisplayBandwidth_FNPTR(pKernelDisplay) pKernelDisplay->__kdispArbAndAllocDisplayBandwidth__
#define kdispArbAndAllocDisplayBandwidth(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispArbAndAllocDisplayBandwidth_DISPATCH(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#define kdispArbAndAllocDisplayBandwidth_HAL(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispArbAndAllocDisplayBandwidth_DISPATCH(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#define kdispSetPushBufferParamsToPhysical_HAL(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass, channelPBSize, subDeviceId) kdispSetPushBufferParamsToPhysical(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass, channelPBSize, subDeviceId)
#define kdispAcquireDispChannelHw_HAL(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab) kdispAcquireDispChannelHw(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab)
#define kdispReleaseDispChannelHw_HAL(pKernelDisplay, pDispChannel) kdispReleaseDispChannelHw(pKernelDisplay, pDispChannel)
#define kdispMapDispChannel_HAL(pKernelDisplay, pDispChannel) kdispMapDispChannel(pKernelDisplay, pDispChannel)
#define kdispUnbindUnmapDispChannel_HAL(pKernelDisplay, pDispChannel) kdispUnbindUnmapDispChannel(pKernelDisplay, pDispChannel)
#define kdispRegisterRgLineCallback_HAL(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable) kdispRegisterRgLineCallback(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable)
#define kdispInvokeRgLineCallback_HAL(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr) kdispInvokeRgLineCallback(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr)
#define kdispReadPendingVblank_HAL(pGpu, pKernelDisplay, arg3) kdispReadPendingVblank(pGpu, pKernelDisplay, arg3)
#define kdispGetVgaWorkspaceBase_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetVgaWorkspaceBase__
#define kdispGetVgaWorkspaceBase(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispGetVgaWorkspaceBase_HAL(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispInvokeDisplayModesetCallback_FNPTR(pKernelDisplay) pKernelDisplay->__kdispInvokeDisplayModesetCallback__
#define kdispInvokeDisplayModesetCallback(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispInvokeDisplayModesetCallback_DISPATCH(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#define kdispInvokeDisplayModesetCallback_HAL(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispInvokeDisplayModesetCallback_DISPATCH(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#define kdispReadRgLineCountAndFrameCount_FNPTR(pKernelDisplay) pKernelDisplay->__kdispReadRgLineCountAndFrameCount__
#define kdispReadRgLineCountAndFrameCount(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispReadRgLineCountAndFrameCount_HAL(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispDsmMxmMxcbExecuteAcpi_HAL(pGpu, pKernelDisplay, pInOutData, outDataSize) kdispDsmMxmMxcbExecuteAcpi(pGpu, pKernelDisplay, pInOutData, outDataSize)
#define kdispInitBrightcStateLoad_FNPTR(pKernelDisplay) pKernelDisplay->__kdispInitBrightcStateLoad__
#define kdispInitBrightcStateLoad(pGpu, pKernelDisplay) kdispInitBrightcStateLoad_DISPATCH(pGpu, pKernelDisplay)
#define kdispInitBrightcStateLoad_HAL(pGpu, pKernelDisplay) kdispInitBrightcStateLoad_DISPATCH(pGpu, pKernelDisplay)
#define kdispSetupAcpiEdid_FNPTR(pKernelDisplay) pKernelDisplay->__kdispSetupAcpiEdid__
#define kdispSetupAcpiEdid(pGpu, pKernelDisplay) kdispSetupAcpiEdid_DISPATCH(pGpu, pKernelDisplay)
#define kdispSetupAcpiEdid_HAL(pGpu, pKernelDisplay) kdispSetupAcpiEdid_DISPATCH(pGpu, pKernelDisplay)
#define kdispRestoreOriginalLsrMinTime_FNPTR(pKernelDisplay) pKernelDisplay->__kdispRestoreOriginalLsrMinTime__
#define kdispRestoreOriginalLsrMinTime(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispRestoreOriginalLsrMinTime_HAL(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispComputeLsrMinTimeValue_FNPTR(pKernelDisplay) pKernelDisplay->__kdispComputeLsrMinTimeValue__
#define kdispComputeLsrMinTimeValue(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispComputeLsrMinTimeValue_HAL(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime_FNPTR(pKernelDisplay) pKernelDisplay->__kdispSetSwapBarrierLsrMinTime__
#define kdispSetSwapBarrierLsrMinTime(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime_HAL(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispGetRgScanLock_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetRgScanLock__
#define kdispGetRgScanLock(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispGetRgScanLock_HAL(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispDetectSliLink_FNPTR(pKernelDisplay) pKernelDisplay->__kdispDetectSliLink__
#define kdispDetectSliLink(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispDetectSliLink_HAL(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispReadAwakenChannelNumMask_FNPTR(pKernelDisplay) pKernelDisplay->__kdispReadAwakenChannelNumMask__
#define kdispReadAwakenChannelNumMask(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispReadAwakenChannelNumMask_DISPATCH(pGpu, pKernelDisplay, arg3, arg4, arg5)
#define kdispReadAwakenChannelNumMask_HAL(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispReadAwakenChannelNumMask_DISPATCH(pGpu, pKernelDisplay, arg3, arg4, arg5)
#define kdispAllocateCommonHandle_HAL(pGpu, pKernelDisplay) kdispAllocateCommonHandle(pGpu, pKernelDisplay)
#define kdispDestroyCommonHandle_HAL(pKernelDisplay) kdispDestroyCommonHandle(pKernelDisplay)
#define kdispGetPBTargetAperture_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetPBTargetAperture__
#define kdispGetPBTargetAperture(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop) kdispGetPBTargetAperture_DISPATCH(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop)
#define kdispGetPBTargetAperture_HAL(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop) kdispGetPBTargetAperture_DISPATCH(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop)
#define kdispAllocateSharedMem_FNPTR(pKernelDisplay) pKernelDisplay->__kdispAllocateSharedMem__
#define kdispAllocateSharedMem(pGpu, pKernelDisplay) kdispAllocateSharedMem_DISPATCH(pGpu, pKernelDisplay)
#define kdispAllocateSharedMem_HAL(pGpu, pKernelDisplay) kdispAllocateSharedMem_DISPATCH(pGpu, pKernelDisplay)
#define kdispFreeSharedMem_HAL(pGpu, pKernelDisplay) kdispFreeSharedMem(pGpu, pKernelDisplay)
#define kdispIsDisplayConnected_HAL(pGpu, pKernelDisplay) kdispIsDisplayConnected(pGpu, pKernelDisplay)
#define kdispGetSupportedDisplayMask_HAL(pGpu, pKernelDisplay) kdispGetSupportedDisplayMask(pGpu, pKernelDisplay)
#define kdispUpdatePdbAfterIpHalInit_HAL(pKernelDisplay) kdispUpdatePdbAfterIpHalInit(pKernelDisplay)
#define kdispReadPendingWinSemIntr_FNPTR(pKernelDisplay) pKernelDisplay->__kdispReadPendingWinSemIntr__
#define kdispReadPendingWinSemIntr(pGpu, pKernelDisplay, arg3, arg4) kdispReadPendingWinSemIntr_DISPATCH(pGpu, pKernelDisplay, arg3, arg4)
#define kdispReadPendingWinSemIntr_HAL(pGpu, pKernelDisplay, arg3, arg4) kdispReadPendingWinSemIntr_DISPATCH(pGpu, pKernelDisplay, arg3, arg4)
#define kdispHandleWinSemEvt_FNPTR(pKernelDisplay) pKernelDisplay->__kdispHandleWinSemEvt__
#define kdispHandleWinSemEvt(pGpu, pKernelDisplay, arg3) kdispHandleWinSemEvt_DISPATCH(pGpu, pKernelDisplay, arg3)
#define kdispHandleWinSemEvt_HAL(pGpu, pKernelDisplay, arg3) kdispHandleWinSemEvt_DISPATCH(pGpu, pKernelDisplay, arg3)
#define kdispIntrRetrigger_FNPTR(pKernelDisplay) pKernelDisplay->__kdispIntrRetrigger__
#define kdispIntrRetrigger(pGpu, pKernelDisplay, index, arg4) kdispIntrRetrigger_DISPATCH(pGpu, pKernelDisplay, index, arg4)
#define kdispIntrRetrigger_HAL(pGpu, pKernelDisplay, index, arg4) kdispIntrRetrigger_DISPATCH(pGpu, pKernelDisplay, index, arg4)
#define kdispComputeDpModeSettings_FNPTR(pKernelDisplay) pKernelDisplay->__kdispComputeDpModeSettings__
#define kdispComputeDpModeSettings(pGpu, pKernelDisplay, headIndex, pDpModesetData, dpInfo) kdispComputeDpModeSettings_DISPATCH(pGpu, pKernelDisplay, headIndex, pDpModesetData, dpInfo)
#define kdispComputeDpModeSettings_HAL(pGpu, pKernelDisplay, headIndex, pDpModesetData, dpInfo) kdispComputeDpModeSettings_DISPATCH(pGpu, pKernelDisplay, headIndex, pDpModesetData, dpInfo)
#define kdispServiceAwakenIntr_FNPTR(pKernelDisplay) pKernelDisplay->__kdispServiceAwakenIntr__
#define kdispServiceAwakenIntr(pGpu, pKernelDisplay, pThreadState) kdispServiceAwakenIntr_DISPATCH(pGpu, pKernelDisplay, pThreadState)
#define kdispServiceAwakenIntr_HAL(pGpu, pKernelDisplay, pThreadState) kdispServiceAwakenIntr_DISPATCH(pGpu, pKernelDisplay, pThreadState)
#define kdispSetChannelTrashAndAbortAccel_FNPTR(arg_this) arg_this->__kdispSetChannelTrashAndAbortAccel__
#define kdispSetChannelTrashAndAbortAccel(arg1, arg_this, arg3, arg4, arg5) kdispSetChannelTrashAndAbortAccel_DISPATCH(arg1, arg_this, arg3, arg4, arg5)
#define kdispSetChannelTrashAndAbortAccel_HAL(arg1, arg_this, arg3, arg4, arg5) kdispSetChannelTrashAndAbortAccel_DISPATCH(arg1, arg_this, arg3, arg4, arg5)
#define kdispIsChannelIdle_FNPTR(arg_this) arg_this->__kdispIsChannelIdle__
#define kdispIsChannelIdle(arg1, arg_this, arg3, arg4) kdispIsChannelIdle_DISPATCH(arg1, arg_this, arg3, arg4)
#define kdispIsChannelIdle_HAL(arg1, arg_this, arg3, arg4) kdispIsChannelIdle_DISPATCH(arg1, arg_this, arg3, arg4)
#define kdispApplyChannelConnectDisconnect_FNPTR(arg_this) arg_this->__kdispApplyChannelConnectDisconnect__
#define kdispApplyChannelConnectDisconnect(arg1, arg_this, arg3, arg4, arg5) kdispApplyChannelConnectDisconnect_DISPATCH(arg1, arg_this, arg3, arg4, arg5)
#define kdispApplyChannelConnectDisconnect_HAL(arg1, arg_this, arg3, arg4, arg5) kdispApplyChannelConnectDisconnect_DISPATCH(arg1, arg_this, arg3, arg4, arg5)
#define kdispIsChannelAllocatedHw_FNPTR(arg_this) arg_this->__kdispIsChannelAllocatedHw__
#define kdispIsChannelAllocatedHw(arg1, arg_this, arg3, arg4) kdispIsChannelAllocatedHw_DISPATCH(arg1, arg_this, arg3, arg4)
#define kdispIsChannelAllocatedHw_HAL(arg1, arg_this, arg3, arg4) kdispIsChannelAllocatedHw_DISPATCH(arg1, arg_this, arg3, arg4)
#define kdispInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kdispInitMissing(pGpu, pEngstate) kdispInitMissing_DISPATCH(pGpu, pEngstate)
#define kdispStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kdispStatePreInitUnlocked(pGpu, pEngstate) kdispStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kdispStateInitUnlocked(pGpu, pEngstate) kdispStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kdispStatePreLoad(pGpu, pEngstate, arg3) kdispStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kdispStatePostLoad(pGpu, pEngstate, arg3) kdispStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kdispStatePreUnload(pGpu, pEngstate, arg3) kdispStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kdispStatePostUnload(pGpu, pEngstate, arg3) kdispStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kdispIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kdispIsPresent(pGpu, pEngstate) kdispIsPresent_DISPATCH(pGpu, pEngstate)
#define kdispClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservClearInterrupt__
#define kdispClearInterrupt(pGpu, pIntrService, pParams) kdispClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kdispServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservServiceNotificationInterrupt__
#define kdispServiceNotificationInterrupt(pGpu, pIntrService, pParams) kdispServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline NV_STATUS kdispConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispConstructEngine__(pGpu, pKernelDisplay, engDesc);
}

static inline NV_STATUS kdispStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStatePreInitLocked__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateInitLocked__(pGpu, pKernelDisplay);
}

static inline void kdispStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateDestroy__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateLoad__(pGpu, pKernelDisplay, flags);
}

static inline NV_STATUS kdispStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispStateUnload__(pGpu, pKernelDisplay, flags);
}

static inline void kdispRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[179]) {
    pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispRegisterIntrService__(pGpu, pKernelDisplay, pRecords);
}

static inline NvU32 kdispServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelDisplay->__nvoc_metadata_ptr->vtable.__kdispServiceInterrupt__(pGpu, pKernelDisplay, pParams);
}

static inline NV_STATUS kdispSelectClass_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return pKernelDisplay->__kdispSelectClass__(pGpu, pKernelDisplay, swClass);
}

static inline NvS32 kdispGetBaseOffset_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispGetBaseOffset__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispGetChannelNum_DISPATCH(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return pKernelDisplay->__kdispGetChannelNum__(pKernelDisplay, channelClass, channelInstance, pChannelNum);
}

static inline void kdispGetDisplayCapsBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplayCapsBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline void kdispGetDisplaySfUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplaySfUserBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return pKernelDisplay->__kdispGetDisplayChannelUserBaseAndSize__(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize);
}

static inline NV_STATUS kdispImportImpData_DISPATCH(struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispImportImpData__(pKernelDisplay);
}

static inline NV_STATUS kdispArbAndAllocDisplayBandwidth_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, enum DISPLAY_ICC_BW_CLIENT iccBwClient, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    return pKernelDisplay->__kdispArbAndAllocDisplayBandwidth__(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS);
}

static inline NvBool kdispGetVgaWorkspaceBase_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return pKernelDisplay->__kdispGetVgaWorkspaceBase__(pGpu, pKernelDisplay, pOffset);
}

static inline void kdispInvokeDisplayModesetCallback_DISPATCH(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    pKernelDisplay->__kdispInvokeDisplayModesetCallback__(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS);
}

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return pKernelDisplay->__kdispReadRgLineCountAndFrameCount__(pGpu, pKernelDisplay, head, pLineCount, pFrameCount);
}

static inline NV_STATUS kdispInitBrightcStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispInitBrightcStateLoad__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispSetupAcpiEdid_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispSetupAcpiEdid__(pGpu, pKernelDisplay);
}

static inline void kdispRestoreOriginalLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    pKernelDisplay->__kdispRestoreOriginalLsrMinTime__(pGpu, pKernelDisplay, head, origLsrMinTime);
}

static inline NV_STATUS kdispComputeLsrMinTimeValue_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return pKernelDisplay->__kdispComputeLsrMinTimeValue__(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime);
}

static inline void kdispSetSwapBarrierLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    pKernelDisplay->__kdispSetSwapBarrierLsrMinTime__(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime);
}

static inline NV_STATUS kdispGetRgScanLock_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    return pKernelDisplay->__kdispGetRgScanLock__(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin);
}

static inline NV_STATUS kdispDetectSliLink_DISPATCH(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    return pKernelDisplay->__kdispDetectSliLink__(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort);
}

static inline NV_STATUS kdispReadAwakenChannelNumMask_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5) {
    return pKernelDisplay->__kdispReadAwakenChannelNumMask__(pGpu, pKernelDisplay, arg3, arg4, arg5);
}

static inline NvU32 kdispGetPBTargetAperture_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop) {
    return pKernelDisplay->__kdispGetPBTargetAperture__(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop);
}

static inline NV_STATUS kdispAllocateSharedMem_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispAllocateSharedMem__(pGpu, pKernelDisplay);
}

static inline NvBool kdispReadPendingWinSemIntr_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3, NvU32 *arg4) {
    return pKernelDisplay->__kdispReadPendingWinSemIntr__(pGpu, pKernelDisplay, arg3, arg4);
}

static inline void kdispHandleWinSemEvt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3) {
    pKernelDisplay->__kdispHandleWinSemEvt__(pGpu, pKernelDisplay, arg3);
}

static inline void kdispIntrRetrigger_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 index, struct THREAD_STATE_NODE *arg4) {
    pKernelDisplay->__kdispIntrRetrigger__(pGpu, pKernelDisplay, index, arg4);
}

static inline NV_STATUS kdispComputeDpModeSettings_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 headIndex, DPMODESETDATA *pDpModesetData, DPIMPINFO *dpInfo) {
    return pKernelDisplay->__kdispComputeDpModeSettings__(pGpu, pKernelDisplay, headIndex, pDpModesetData, dpInfo);
}

static inline NvU32 kdispServiceAwakenIntr_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *pThreadState) {
    return pKernelDisplay->__kdispServiceAwakenIntr__(pGpu, pKernelDisplay, pThreadState);
}

static inline void kdispSetChannelTrashAndAbortAccel_DISPATCH(OBJGPU *arg1, struct KernelDisplay *arg_this, NvU32 arg3, NvU32 arg4, NvBool arg5) {
    arg_this->__kdispSetChannelTrashAndAbortAccel__(arg1, arg_this, arg3, arg4, arg5);
}

static inline NvBool kdispIsChannelIdle_DISPATCH(OBJGPU *arg1, struct KernelDisplay *arg_this, NvU32 arg3, NvU32 arg4) {
    return arg_this->__kdispIsChannelIdle__(arg1, arg_this, arg3, arg4);
}

static inline void kdispApplyChannelConnectDisconnect_DISPATCH(OBJGPU *arg1, struct KernelDisplay *arg_this, NvU32 arg3, NvU32 arg4, NvU32 arg5) {
    arg_this->__kdispApplyChannelConnectDisconnect__(arg1, arg_this, arg3, arg4, arg5);
}

static inline NvBool kdispIsChannelAllocatedHw_DISPATCH(OBJGPU *arg1, struct KernelDisplay *arg_this, NvU32 arg3, NvU32 arg4) {
    return arg_this->__kdispIsChannelAllocatedHw__(arg1, arg_this, arg3, arg4);
}

static inline void kdispInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kdispInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kdispIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kdispIsPresent__(pGpu, pEngstate);
}

static inline NvBool kdispClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__kdispClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kdispServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__kdispServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

void kdispServiceLowLatencyIntrs_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5, NvU32 *pIntrServicedHeadMask, MC_ENGINE_BITVECTOR *pIntrPending);


NV_STATUS kdispConstructInstMem_IMPL(struct KernelDisplay *pKernelDisplay);


void kdispDestructInstMem_IMPL(struct KernelDisplay *pKernelDisplay);


NV_STATUS kdispSetPushBufferParamsToPhysical_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvHandle hObjectBuffer, struct ContextDma *pBufferContextDma, NvU32 hClass, NvU32 channelInstance, DISPCHNCLASS internalDispChnClass, ChannelPBSize channelPBSize, NvU32 subDeviceId);


static inline NV_STATUS kdispAcquireDispChannelHw_56cd7a(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvU32 channelInstance, NvHandle hObjectBuffer, NvU32 initialGetPutOffset, NvBool allowGrabWithinSameClient, NvBool connectPbAtGrab) {
    return NV_OK;
}


static inline NV_STATUS kdispReleaseDispChannelHw_56cd7a(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    return NV_OK;
}


NV_STATUS kdispMapDispChannel_IMPL(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel);


void kdispUnbindUnmapDispChannel_IMPL(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel);


NV_STATUS kdispRegisterRgLineCallback_IMPL(struct KernelDisplay *pKernelDisplay, struct RgLineCallback *pRgLineCallback, NvU32 head, NvU32 rgIntrLine, NvBool bEnable);


void kdispInvokeRgLineCallback_KERNEL(struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 rgIntrLine, NvBool bIsIrqlIsr);


NvU32 kdispReadPendingVblank_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3);


static inline NV_STATUS kdispDsmMxmMxcbExecuteAcpi_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pInOutData, NvU16 *outDataSize) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


NV_STATUS kdispAllocateCommonHandle_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


void kdispDestroyCommonHandle_IMPL(struct KernelDisplay *pKernelDisplay);


void kdispFreeSharedMem_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


NvBool kdispIsDisplayConnected_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


NvU32 kdispGetSupportedDisplayMask_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


static inline void kdispUpdatePdbAfterIpHalInit_b3696a(struct KernelDisplay *pKernelDisplay) {
    return;
}


NV_STATUS kdispConstructEngine_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc);

NV_STATUS kdispStatePreInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

NV_STATUS kdispStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

void kdispStateDestroy_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

NV_STATUS kdispStateLoad_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

NV_STATUS kdispStateUnload_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

void kdispRegisterIntrService_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[179]);

NvU32 kdispServiceInterrupt_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams);

static inline NV_STATUS kdispSelectClass_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispSelectClass_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass);

NvS32 kdispGetBaseOffset_v04_02(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NvS32 kdispGetBaseOffset_4a4dee(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return 0;
}

static inline NV_STATUS kdispGetChannelNum_46f6a7(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetChannelNum_v03_00(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum);

static inline void kdispGetDisplayCapsBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplayCapsBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline void kdispGetDisplaySfUserBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplaySfUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetDisplayChannelUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize);

NV_STATUS kdispImportImpData_IMPL(struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispImportImpData_56cd7a(struct KernelDisplay *pKernelDisplay) {
    return NV_OK;
}

NV_STATUS kdispArbAndAllocDisplayBandwidth_v04_02(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, enum DISPLAY_ICC_BW_CLIENT iccBwClient, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS);

static inline NV_STATUS kdispArbAndAllocDisplayBandwidth_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, enum DISPLAY_ICC_BW_CLIENT iccBwClient, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool kdispGetVgaWorkspaceBase_v04_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset);

static inline NvBool kdispGetVgaWorkspaceBase_3dd2c9(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return NV_FALSE;
}

static inline NvBool kdispGetVgaWorkspaceBase_72a2e1(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    NV_ASSERT_PRECOMP(0);
    return NV_FALSE;
}

void kdispInvokeDisplayModesetCallback_KERNEL(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS);

static inline void kdispInvokeDisplayModesetCallback_b3696a(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    return;
}

void kdispInvokeDisplayModesetCallback_PHYSICAL(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS);

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_PHYSICAL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

NV_STATUS kdispInitBrightcStateLoad_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispInitBrightcStateLoad_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return NV_OK;
}

NV_STATUS kdispSetupAcpiEdid_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispSetupAcpiEdid_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return NV_OK;
}

void kdispRestoreOriginalLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime);

static inline void kdispRestoreOriginalLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    return;
}

NV_STATUS kdispComputeLsrMinTimeValue_v02_07(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime);

static inline NV_STATUS kdispComputeLsrMinTimeValue_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return NV_OK;
}

void kdispSetSwapBarrierLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime);

static inline void kdispSetSwapBarrierLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    return;
}

NV_STATUS kdispGetRgScanLock_v02_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin);

static inline NV_STATUS kdispGetRgScanLock_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispDetectSliLink_v04_00(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort);

static inline NV_STATUS kdispDetectSliLink_92bfc3(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadAwakenChannelNumMask_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5);

static inline NV_STATUS kdispReadAwakenChannelNumMask_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 kdispGetPBTargetAperture_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop);

NvU32 kdispGetPBTargetAperture_v05_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop);

static inline NvU32 kdispGetPBTargetAperture_15a734(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop) {
    return 0U;
}

NV_STATUS kdispAllocateSharedMem_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispAllocateSharedMem_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return NV_ERR_NOT_SUPPORTED;
}

NvBool kdispReadPendingWinSemIntr_v04_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3, NvU32 *arg4);

static inline NvBool kdispReadPendingWinSemIntr_3dd2c9(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3, NvU32 *arg4) {
    return NV_FALSE;
}

void kdispHandleWinSemEvt_v04_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3);

static inline void kdispHandleWinSemEvt_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3) {
    return;
}

void kdispIntrRetrigger_v05_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 index, struct THREAD_STATE_NODE *arg4);

static inline void kdispIntrRetrigger_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 index, struct THREAD_STATE_NODE *arg4) {
    return;
}

NV_STATUS kdispComputeDpModeSettings_v02_04(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 headIndex, DPMODESETDATA *pDpModesetData, DPIMPINFO *dpInfo);

NV_STATUS kdispComputeDpModeSettings_v05_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 headIndex, DPMODESETDATA *pDpModesetData, DPIMPINFO *dpInfo);

static inline NV_STATUS kdispComputeDpModeSettings_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 headIndex, DPMODESETDATA *pDpModesetData, DPIMPINFO *dpInfo) {
    return NV_OK;
}

NvU32 kdispServiceAwakenIntr_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *pThreadState);

static inline NvU32 kdispServiceAwakenIntr_4a4dee(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *pThreadState) {
    return 0;
}

void kdispSetChannelTrashAndAbortAccel_v03_00(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4, NvBool arg5);

static inline void kdispSetChannelTrashAndAbortAccel_b3696a(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4, NvBool arg5) {
    return;
}

NvBool kdispIsChannelIdle_v03_00(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4);

static inline NvBool kdispIsChannelIdle_3dd2c9(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4) {
    return NV_FALSE;
}

void kdispApplyChannelConnectDisconnect_v03_00(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5);

static inline void kdispApplyChannelConnectDisconnect_b3696a(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4, NvU32 arg5) {
    return;
}

NvBool kdispIsChannelAllocatedHw_v03_00(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4);

static inline NvBool kdispIsChannelAllocatedHw_3dd2c9(OBJGPU *arg1, struct KernelDisplay *arg2, NvU32 arg3, NvU32 arg4) {
    return NV_FALSE;
}

#undef PRIVATE_FIELD


/*!
 * Custom roll a conditional acquire spinlock.
 * To be replaced when nvport supports drop-in replacement
 */
void kdispAcquireLowLatencyLock(volatile NvS32 *pLowLatencyLock);
NvBool kdispAcquireLowLatencyLockConditional(volatile NvS32 *pLowLatencyLock);
void kdispReleaseLowLatencyLock(volatile NvS32 *pLowLatencyLock);

void
dispdeviceFillVgaSavedDisplayState( OBJGPU *pGpu,
    NvU64   vgaAddr,
    NvU8    vgaMemType,
    NvBool  vgaValid,
    NvU64   workspaceAddr,
    NvU8    workspaceMemType,
    NvBool  workspaceValid,
    NvBool  baseValid,
    NvBool  workspaceBaseValid
);

/*! PushBuffer Target Aperture Types */
typedef enum
{
    IOVA,
    PHYS_NVM,
    PHYS_PCI,
    PHYS_PCI_COHERENT
} PBTARGETAPERTURE;

static NV_INLINE struct KernelHead*
kdispGetHead
(
    struct KernelDisplay *pKernelDisplay,
    NvU32 head
)
{
    if (head >= OBJ_MAX_HEADS)
    {
        return NULL;
    }

    return pKernelDisplay->pKernelHead[head];
}

static NV_INLINE NvU32
kdispGetNumHeads(struct KernelDisplay *pKernelDisplay)
{
    NV_ASSERT(pKernelDisplay != NULL);
    return pKernelDisplay->numHeads;
}

static NV_INLINE NvU32
kdispGetDeferredVblankHeadMask(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->deferredVblankHeadMask;
}

static NV_INLINE void
kdispSetDeferredVblankHeadMask(struct KernelDisplay *pKernelDisplay, NvU32 vblankHeadMask)
{
    pKernelDisplay->deferredVblankHeadMask = vblankHeadMask;
}

static NV_INLINE NvHandle
kdispGetInternalClientHandle(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->hInternalClient;
}

static NV_INLINE NvHandle
kdispGetDispCommonHandle(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->hDispCommonHandle;
}

#endif // KERN_DISP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_DISP_NVOC_H_
