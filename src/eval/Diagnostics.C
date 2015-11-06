#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

#include<eval/EvalProgram.h>
#include<eval/Diagnostics.h>

#define MAXDIGITS 100
#define NO_PARAMS 0
#define SIGDIGITS 6
#define TRUNC_BASE 10


float RunDiagnostics(SearchAlgo *sInfo) {
  
  char digits[MAXDIGITS];
  //  float data = EvalProg((*sInfo), NULL);
  /*  sprintf(digits, "%lu", (unsigned long) data);
  double truncfactor = pow((double) TRUNC_BASE, 
			   (int) (strlen(digits) - SIGDIGITS));
  sInfo->SetTruncFactor(truncfactor);
  data = ceilf(data/truncfactor); */

  // return data;
}

float RunDiagnostics(string _path, string _prog, string _srcFile) {
#if 0
  vector <float> *values = new vector <float>;
  float data = EvalProg(0, _path, _prog, _srcFile, NULL, values, NULL);
  string logfilename = _path + _prog + ".log";
  ofstream logfile(logfilename.c_str(), ios::app);
  
  logfile << "************** Baseline Programs Diagnostics ***************";
  logfile << endl;
  logfile << "\tPrograms execution time " << data << endl;
  for (int i = 0; i < values->size(); i++)
    logfile << "\t  Region " << i << ": " << (*values)[i] << endl;
  logfile << "************************************************************";
  logfile.close();
  delete values;
  return data;
#endif
}

