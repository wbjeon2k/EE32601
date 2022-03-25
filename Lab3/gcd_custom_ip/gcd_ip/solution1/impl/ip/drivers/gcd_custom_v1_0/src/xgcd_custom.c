// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xgcd_custom.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XGcd_custom_CfgInitialize(XGcd_custom *InstancePtr, XGcd_custom_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XGcd_custom_Set_a(XGcd_custom *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGcd_custom_WriteReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_A_DATA, Data);
}

u32 XGcd_custom_Get_a(XGcd_custom *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGcd_custom_ReadReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_A_DATA);
    return Data;
}

void XGcd_custom_Set_b(XGcd_custom *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XGcd_custom_WriteReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_B_DATA, Data);
}

u32 XGcd_custom_Get_b(XGcd_custom *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGcd_custom_ReadReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_B_DATA);
    return Data;
}

u32 XGcd_custom_Get_gcd(XGcd_custom *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGcd_custom_ReadReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_GCD_DATA);
    return Data;
}

u32 XGcd_custom_Get_gcd_vld(XGcd_custom *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XGcd_custom_ReadReg(InstancePtr->Control_BaseAddress, XGCD_CUSTOM_CONTROL_ADDR_GCD_CTRL);
    return Data & 0x1;
}

