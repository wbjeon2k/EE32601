
log_wave [get_objects -filter {type == in_port || type == out_port || type == inout_port || type == port} /apatb_gcd_custom_top/AESL_inst_gcd_custom/*]
set designtopgroup [add_wave_group "Design Top Signals"]
set cinputgroup [add_wave_group "C Inputs" -into $designtopgroup]
set a__b__gcd_group [add_wave_group a__b__gcd(axi_slave) -into $cinputgroup]
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_BRESP -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_BREADY -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_BVALID -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_RRESP -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_RDATA -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_RREADY -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_RVALID -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_ARREADY -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_ARVALID -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_ARADDR -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_WSTRB -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_WDATA -into $a__b__gcd_group -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_WREADY -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_WVALID -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_AWREADY -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_AWVALID -into $a__b__gcd_group -color #ffff00 -radix hex
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/s_axi_control_AWADDR -into $a__b__gcd_group -radix hex
set blocksiggroup [add_wave_group "Block-level IO Handshake" -into $designtopgroup]
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_start -into $blocksiggroup
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_done -into $blocksiggroup
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_idle -into $blocksiggroup
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_ready -into $blocksiggroup
set resetgroup [add_wave_group "Reset" -into $designtopgroup]
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_rst_n -into $resetgroup
set clockgroup [add_wave_group "Clock" -into $designtopgroup]
add_wave /apatb_gcd_custom_top/AESL_inst_gcd_custom/ap_clk -into $clockgroup
save_wave_config gcd_custom.wcfg
run all

