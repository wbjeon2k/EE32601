#include <systemc>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <stdint.h>
#include "SysCFileHandler.h"
#include "ap_int.h"
#include "ap_fixed.h"
#include <complex>
#include <stdbool.h>
#include "autopilot_cbe.h"
#include "hls_stream.h"
#include "hls_half.h"
#include "hls_signal_handler.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

// wrapc file define:
#define AUTOTB_TVIN_gmem "../tv/cdatafile/c.saxpy.autotvin_gmem.dat"
#define AUTOTB_TVOUT_gmem "../tv/cdatafile/c.saxpy.autotvout_gmem.dat"
// wrapc file define:
#define AUTOTB_TVIN_x "../tv/cdatafile/c.saxpy.autotvin_x.dat"
#define AUTOTB_TVOUT_x "../tv/cdatafile/c.saxpy.autotvout_x.dat"
// wrapc file define:
#define AUTOTB_TVIN_y "../tv/cdatafile/c.saxpy.autotvin_y.dat"
#define AUTOTB_TVOUT_y "../tv/cdatafile/c.saxpy.autotvout_y.dat"
// wrapc file define:
#define AUTOTB_TVIN_a "../tv/cdatafile/c.saxpy.autotvin_a.dat"
#define AUTOTB_TVOUT_a "../tv/cdatafile/c.saxpy.autotvout_a.dat"

#define INTER_TCL "../tv/cdatafile/ref.tcl"

// tvout file define:
#define AUTOTB_TVOUT_PC_gmem "../tv/rtldatafile/rtl.saxpy.autotvout_gmem.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_x "../tv/rtldatafile/rtl.saxpy.autotvout_x.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_y "../tv/rtldatafile/rtl.saxpy.autotvout_y.dat"
// tvout file define:
#define AUTOTB_TVOUT_PC_a "../tv/rtldatafile/rtl.saxpy.autotvout_a.dat"

class INTER_TCL_FILE {
  public:
INTER_TCL_FILE(const char* name) {
  mName = name; 
  gmem_depth = 0;
  x_depth = 0;
  y_depth = 0;
  a_depth = 0;
  trans_num =0;
}
~INTER_TCL_FILE() {
  mFile.open(mName);
  if (!mFile.good()) {
    cout << "Failed to open file ref.tcl" << endl;
    exit (1); 
  }
  string total_list = get_depth_list();
  mFile << "set depth_list {\n";
  mFile << total_list;
  mFile << "}\n";
  mFile << "set trans_num "<<trans_num<<endl;
  mFile.close();
}
string get_depth_list () {
  stringstream total_list;
  total_list << "{gmem " << gmem_depth << "}\n";
  total_list << "{x " << x_depth << "}\n";
  total_list << "{y " << y_depth << "}\n";
  total_list << "{a " << a_depth << "}\n";
  return total_list.str();
}
void set_num (int num , int* class_num) {
  (*class_num) = (*class_num) > num ? (*class_num) : num;
}
  public:
    int gmem_depth;
    int x_depth;
    int y_depth;
    int a_depth;
    int trans_num;
  private:
    ofstream mFile;
    const char* mName;
};

static void RTLOutputCheckAndReplacement(std::string &AESL_token, std::string PortName) {
  bool no_x = false;
  bool err = false;

  no_x = false;
  // search and replace 'X' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('X', 0);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'X' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
  no_x = false;
  // search and replace 'x' with '0' from the 3rd char of token
  while (!no_x) {
    size_t x_found = AESL_token.find('x', 2);
    if (x_found != string::npos) {
      if (!err) { 
        cerr << "WARNING: [SIM 212-201] RTL produces unknown value 'x' on port" 
             << PortName << ", possible cause: There are uninitialized variables in the C design."
             << endl; 
        err = true;
      }
      AESL_token.replace(x_found, 1, "0");
    } else
      no_x = true;
  }
}
extern "C" void saxpy_hw_stub(volatile void *, volatile void *, float);

extern "C" void apatb_saxpy_hw(volatile void * __xlx_apatb_param_x, volatile void * __xlx_apatb_param_y, float __xlx_apatb_param_a) {
  refine_signal_handler();
  fstream wrapc_switch_file_token;
  wrapc_switch_file_token.open(".hls_cosim_wrapc_switch.log");
  int AESL_i;
  if (wrapc_switch_file_token.good())
  {

    CodeState = ENTER_WRAPC_PC;
    static unsigned AESL_transaction_pc = 0;
    string AESL_token;
    string AESL_num;{
      static ifstream rtl_tv_out_file;
      if (!rtl_tv_out_file.is_open()) {
        rtl_tv_out_file.open(AUTOTB_TVOUT_PC_gmem);
        if (rtl_tv_out_file.good()) {
          rtl_tv_out_file >> AESL_token;
          if (AESL_token != "[[[runtime]]]")
            exit(1);
        }
      }
  
      if (rtl_tv_out_file.good()) {
        rtl_tv_out_file >> AESL_token; 
        rtl_tv_out_file >> AESL_num;  // transaction number
        if (AESL_token != "[[transaction]]") {
          cerr << "Unexpected token: " << AESL_token << endl;
          exit(1);
        }
        if (atoi(AESL_num.c_str()) == AESL_transaction_pc) {
          std::vector<sc_bv<32> > gmem_pc_buffer(40);
          int i = 0;

          rtl_tv_out_file >> AESL_token; //data
          while (AESL_token != "[[/transaction]]"){

            RTLOutputCheckAndReplacement(AESL_token, "gmem");
  
            // push token into output port buffer
            if (AESL_token != "") {
              gmem_pc_buffer[i] = AESL_token.c_str();;
              i++;
            }
  
            rtl_tv_out_file >> AESL_token; //data or [[/transaction]]
            if (AESL_token == "[[[/runtime]]]" || rtl_tv_out_file.eof())
              exit(1);
          }
          if (i > 0) {{
            int i = 0;
            for (int j = 0, e = 20; j < e; j += 1, ++i) {
            ((int*)__xlx_apatb_param_x)[j] = gmem_pc_buffer[i].to_int64();
          }
            for (int j = 0, e = 20; j < e; j += 1, ++i) {
            ((int*)__xlx_apatb_param_y)[j] = gmem_pc_buffer[i].to_int64();
          }}}
        } // end transaction
      } // end file is good
    } // end post check logic bolck
  
    AESL_transaction_pc++;
    return ;
  }
static unsigned AESL_transaction;
static AESL_FILE_HANDLER aesl_fh;
static INTER_TCL_FILE tcl_file(INTER_TCL);
std::vector<char> __xlx_sprintf_buffer(1024);
CodeState = ENTER_WRAPC;
//gmem
aesl_fh.touch(AUTOTB_TVIN_gmem);
aesl_fh.touch(AUTOTB_TVOUT_gmem);
//x
aesl_fh.touch(AUTOTB_TVIN_x);
aesl_fh.touch(AUTOTB_TVOUT_x);
//y
aesl_fh.touch(AUTOTB_TVIN_y);
aesl_fh.touch(AUTOTB_TVOUT_y);
//a
aesl_fh.touch(AUTOTB_TVIN_a);
aesl_fh.touch(AUTOTB_TVOUT_a);
CodeState = DUMP_INPUTS;
// print gmem Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data());
  {  if (__xlx_apatb_param_x) {
    for (int j = 0, e = 20; j != e; ++j) {
sc_bv<32> __xlx_tmp_lv = ((int*)__xlx_apatb_param_x)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data()); 
      }
  }
  if (__xlx_apatb_param_y) {
    for (int j = 0, e = 20; j != e; ++j) {
sc_bv<32> __xlx_tmp_lv = ((int*)__xlx_apatb_param_y)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(40, &tcl_file.gmem_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_gmem, __xlx_sprintf_buffer.data());
}
// print x Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_x, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv;
    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_x, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.x_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_x, __xlx_sprintf_buffer.data());
}
// print y Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data());
  {
    sc_bv<64> __xlx_tmp_lv;
    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.y_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_y, __xlx_sprintf_buffer.data());
}
// print a Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVIN_a, __xlx_sprintf_buffer.data());
  {
    sc_bv<32> __xlx_tmp_lv = *((int*)&__xlx_apatb_param_a);
    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVIN_a, __xlx_sprintf_buffer.data()); 
  }
  tcl_file.set_num(1, &tcl_file.a_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVIN_a, __xlx_sprintf_buffer.data());
}
CodeState = CALL_C_DUT;
saxpy_hw_stub(__xlx_apatb_param_x, __xlx_apatb_param_y, __xlx_apatb_param_a);
CodeState = DUMP_OUTPUTS;
// print gmem Transactions
{
  sprintf(__xlx_sprintf_buffer.data(), "[[transaction]] %d\n", AESL_transaction);
  aesl_fh.write(AUTOTB_TVOUT_gmem, __xlx_sprintf_buffer.data());
  {  if (__xlx_apatb_param_x) {
    for (int j = 0, e = 20; j != e; ++j) {
sc_bv<32> __xlx_tmp_lv = ((int*)__xlx_apatb_param_x)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVOUT_gmem, __xlx_sprintf_buffer.data()); 
      }
  }
  if (__xlx_apatb_param_y) {
    for (int j = 0, e = 20; j != e; ++j) {
sc_bv<32> __xlx_tmp_lv = ((int*)__xlx_apatb_param_y)[j];

    sprintf(__xlx_sprintf_buffer.data(), "%s\n", __xlx_tmp_lv.to_string(SC_HEX).c_str());
    aesl_fh.write(AUTOTB_TVOUT_gmem, __xlx_sprintf_buffer.data()); 
      }
  }
}
  tcl_file.set_num(40, &tcl_file.gmem_depth);
  sprintf(__xlx_sprintf_buffer.data(), "[[/transaction]] \n");
  aesl_fh.write(AUTOTB_TVOUT_gmem, __xlx_sprintf_buffer.data());
}
CodeState = DELETE_CHAR_BUFFERS;
AESL_transaction++;
tcl_file.set_num(AESL_transaction , &tcl_file.trans_num);
}
