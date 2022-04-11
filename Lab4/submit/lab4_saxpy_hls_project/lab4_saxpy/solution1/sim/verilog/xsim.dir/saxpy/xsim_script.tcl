set_param project.enableReportConfiguration 0
load_feature core
current_fileset
xsim {saxpy} -view {{saxpy_dataflow_ana.wcfg}} -tclbatch {saxpy.tcl} -protoinst {saxpy.protoinst}
