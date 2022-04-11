// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XSAXPY_H
#define XSAXPY_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xsaxpy_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XSaxpy_Config;
#endif

typedef struct {
    u32 Control_BaseAddress;
    u32 IsReady;
} XSaxpy;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XSaxpy_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XSaxpy_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XSaxpy_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XSaxpy_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XSaxpy_Initialize(XSaxpy *InstancePtr, u16 DeviceId);
XSaxpy_Config* XSaxpy_LookupConfig(u16 DeviceId);
int XSaxpy_CfgInitialize(XSaxpy *InstancePtr, XSaxpy_Config *ConfigPtr);
#else
int XSaxpy_Initialize(XSaxpy *InstancePtr, const char* InstanceName);
int XSaxpy_Release(XSaxpy *InstancePtr);
#endif

void XSaxpy_Start(XSaxpy *InstancePtr);
u32 XSaxpy_IsDone(XSaxpy *InstancePtr);
u32 XSaxpy_IsIdle(XSaxpy *InstancePtr);
u32 XSaxpy_IsReady(XSaxpy *InstancePtr);
void XSaxpy_EnableAutoRestart(XSaxpy *InstancePtr);
void XSaxpy_DisableAutoRestart(XSaxpy *InstancePtr);

void XSaxpy_Set_x(XSaxpy *InstancePtr, u64 Data);
u64 XSaxpy_Get_x(XSaxpy *InstancePtr);
void XSaxpy_Set_y(XSaxpy *InstancePtr, u64 Data);
u64 XSaxpy_Get_y(XSaxpy *InstancePtr);
void XSaxpy_Set_a(XSaxpy *InstancePtr, u32 Data);
u32 XSaxpy_Get_a(XSaxpy *InstancePtr);

void XSaxpy_InterruptGlobalEnable(XSaxpy *InstancePtr);
void XSaxpy_InterruptGlobalDisable(XSaxpy *InstancePtr);
void XSaxpy_InterruptEnable(XSaxpy *InstancePtr, u32 Mask);
void XSaxpy_InterruptDisable(XSaxpy *InstancePtr, u32 Mask);
void XSaxpy_InterruptClear(XSaxpy *InstancePtr, u32 Mask);
u32 XSaxpy_InterruptGetEnabled(XSaxpy *InstancePtr);
u32 XSaxpy_InterruptGetStatus(XSaxpy *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
