############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project gcd_ip
set_top gcd_custom
add_files gcd_custom.cpp
add_files gcd_custom.h
add_files -tb gcd_testbench.cpp
open_solution "solution1" -flow_target vivado
set_part {xc7z020clg400-1}
create_clock -period 10 -name default
#source "./gcd_ip/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -wave_debug -trace_level port
export_design -rtl verilog -format ip_catalog -output C:/Xilinx/Vivado/2020.1/ip_repository/gcd_custom.zip
