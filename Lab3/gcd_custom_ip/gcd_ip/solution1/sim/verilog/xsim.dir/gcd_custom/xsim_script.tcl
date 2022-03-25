set_param project.enableReportConfiguration 0
load_feature core
current_fileset
xsim {gcd_custom} -view {{gcd_custom_dataflow_ana.wcfg}} -tclbatch {gcd_custom.tcl} -protoinst {gcd_custom.protoinst}
