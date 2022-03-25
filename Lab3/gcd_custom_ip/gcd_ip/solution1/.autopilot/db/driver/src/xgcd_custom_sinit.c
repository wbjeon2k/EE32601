// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xgcd_custom.h"

extern XGcd_custom_Config XGcd_custom_ConfigTable[];

XGcd_custom_Config *XGcd_custom_LookupConfig(u16 DeviceId) {
	XGcd_custom_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XGCD_CUSTOM_NUM_INSTANCES; Index++) {
		if (XGcd_custom_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XGcd_custom_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XGcd_custom_Initialize(XGcd_custom *InstancePtr, u16 DeviceId) {
	XGcd_custom_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XGcd_custom_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XGcd_custom_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

