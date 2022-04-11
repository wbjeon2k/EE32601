############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project lab4_saxpy
set_top saxpy
add_files ../saxpy.cpp
add_files ../saxpy.h
add_files -tb ../saxpy_test.cpp -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_export -format ip_catalog -output C:/Xilinx/Vivado/2020.1/ip_repository/saxpy.zip -rtl verilog
source "./lab4_saxpy/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -wave_debug -trace_level port
export_design -rtl verilog -format ip_catalog -output C:/Xilinx/Vivado/2020.1/ip_repository/saxpy.zip
