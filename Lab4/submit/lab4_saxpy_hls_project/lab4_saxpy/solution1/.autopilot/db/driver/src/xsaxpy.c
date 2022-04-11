// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xsaxpy.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XSaxpy_CfgInitialize(XSaxpy *InstancePtr, XSaxpy_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XSaxpy_Start(XSaxpy *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL) & 0x80;
    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XSaxpy_IsDone(XSaxpy *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XSaxpy_IsIdle(XSaxpy *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XSaxpy_IsReady(XSaxpy *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XSaxpy_EnableAutoRestart(XSaxpy *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XSaxpy_DisableAutoRestart(XSaxpy *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_AP_CTRL, 0);
}

void XSaxpy_Set_x(XSaxpy *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_X_DATA, (u32)(Data));
    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_X_DATA + 4, (u32)(Data >> 32));
}

u64 XSaxpy_Get_x(XSaxpy *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_X_DATA);
    Data += (u64)XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_X_DATA + 4) << 32;
    return Data;
}

void XSaxpy_Set_y(XSaxpy *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_Y_DATA, (u32)(Data));
    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_Y_DATA + 4, (u32)(Data >> 32));
}

u64 XSaxpy_Get_y(XSaxpy *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_Y_DATA);
    Data += (u64)XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_Y_DATA + 4) << 32;
    return Data;
}

void XSaxpy_Set_a(XSaxpy *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_A_DATA, Data);
}

u32 XSaxpy_Get_a(XSaxpy *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_A_DATA);
    return Data;
}

void XSaxpy_InterruptGlobalEnable(XSaxpy *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_GIE, 1);
}

void XSaxpy_InterruptGlobalDisable(XSaxpy *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_GIE, 0);
}

void XSaxpy_InterruptEnable(XSaxpy *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_IER);
    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_IER, Register | Mask);
}

void XSaxpy_InterruptDisable(XSaxpy *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_IER);
    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_IER, Register & (~Mask));
}

void XSaxpy_InterruptClear(XSaxpy *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSaxpy_WriteReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_ISR, Mask);
}

u32 XSaxpy_InterruptGetEnabled(XSaxpy *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_IER);
}

u32 XSaxpy_InterruptGetStatus(XSaxpy *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSaxpy_ReadReg(InstancePtr->Control_BaseAddress, XSAXPY_CONTROL_ADDR_ISR);
}

