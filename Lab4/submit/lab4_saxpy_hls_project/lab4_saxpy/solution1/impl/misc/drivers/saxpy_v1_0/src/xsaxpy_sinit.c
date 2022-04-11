// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xsaxpy.h"

extern XSaxpy_Config XSaxpy_ConfigTable[];

XSaxpy_Config *XSaxpy_LookupConfig(u16 DeviceId) {
	XSaxpy_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSAXPY_NUM_INSTANCES; Index++) {
		if (XSaxpy_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XSaxpy_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XSaxpy_Initialize(XSaxpy *InstancePtr, u16 DeviceId) {
	XSaxpy_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XSaxpy_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XSaxpy_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

