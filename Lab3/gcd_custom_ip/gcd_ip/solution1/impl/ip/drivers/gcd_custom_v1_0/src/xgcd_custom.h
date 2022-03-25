// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XGCD_CUSTOM_H
#define XGCD_CUSTOM_H

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
#include "xgcd_custom_hw.h"

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
} XGcd_custom_Config;
#endif

typedef struct {
    u32 Control_BaseAddress;
    u32 IsReady;
} XGcd_custom;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XGcd_custom_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XGcd_custom_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XGcd_custom_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XGcd_custom_ReadReg(BaseAddress, RegOffset) \
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
int XGcd_custom_Initialize(XGcd_custom *InstancePtr, u16 DeviceId);
XGcd_custom_Config* XGcd_custom_LookupConfig(u16 DeviceId);
int XGcd_custom_CfgInitialize(XGcd_custom *InstancePtr, XGcd_custom_Config *ConfigPtr);
#else
int XGcd_custom_Initialize(XGcd_custom *InstancePtr, const char* InstanceName);
int XGcd_custom_Release(XGcd_custom *InstancePtr);
#endif


void XGcd_custom_Set_a(XGcd_custom *InstancePtr, u32 Data);
u32 XGcd_custom_Get_a(XGcd_custom *InstancePtr);
void XGcd_custom_Set_b(XGcd_custom *InstancePtr, u32 Data);
u32 XGcd_custom_Get_b(XGcd_custom *InstancePtr);
u32 XGcd_custom_Get_gcd(XGcd_custom *InstancePtr);
u32 XGcd_custom_Get_gcd_vld(XGcd_custom *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
