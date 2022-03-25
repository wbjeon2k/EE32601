// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2020.1.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of a
//        bit 31~0 - a[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of b
//        bit 31~0 - b[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of gcd
//        bit 31~0 - gcd[31:0] (Read)
// 0x24 : Control signal of gcd
//        bit 0  - gcd_ap_vld (Read/COR)
//        others - reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XGCD_CUSTOM_CONTROL_ADDR_A_DATA   0x10
#define XGCD_CUSTOM_CONTROL_BITS_A_DATA   32
#define XGCD_CUSTOM_CONTROL_ADDR_B_DATA   0x18
#define XGCD_CUSTOM_CONTROL_BITS_B_DATA   32
#define XGCD_CUSTOM_CONTROL_ADDR_GCD_DATA 0x20
#define XGCD_CUSTOM_CONTROL_BITS_GCD_DATA 32
#define XGCD_CUSTOM_CONTROL_ADDR_GCD_CTRL 0x24

