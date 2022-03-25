
set PATH=
call C:/Xilinx/Vivado/2020.1/bin/xelab xil_defaultlib.apatb_gcd_custom_top glbl -prj gcd_custom.prj -L smartconnect_v1_0 -L axi_protocol_checker_v1_1_12 -L axi_protocol_checker_v1_1_13 -L axis_protocol_checker_v1_1_11 -L axis_protocol_checker_v1_1_12 -L xil_defaultlib -L unisims_ver -L xpm  --lib "ieee_proposed=./ieee_proposed" -s gcd_custom -debug wave
call C:/Xilinx/Vivado/2020.1/bin/xsim --noieeewarnings gcd_custom -tclbatch gcd_custom.tcl -gui -view gcd_custom_dataflow_ana.wcfg -protoinst gcd_custom.protoinst

