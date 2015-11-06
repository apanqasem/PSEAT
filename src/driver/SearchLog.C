#include<iomanip>
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

#include<Args.h>
#include<SearchLog.h>

void LinkerHack() {
}

void logSingleEval(int dims, int *params, float val, const char mode, string logfileName) {

  ofstream logfile;
  logfile.open(logfileName.c_str(), ios::app);

  if (mode == 'a') {
    logfile << "(";
    for (int i = 0; i < dims - 1; i++)
      logfile << params[i] << ", ";
    logfile << params[dims - 1] << ")\t   "; 
  }
  else {
    for (int i = 0; i < dims; i++)
      logfile << setw(5) << params[i] << " ";
    logfile << "\t";
  }

  logfile << fixed << setiosflags(ios::right);
  logfile.precision(2);
  if (mode == 'r')
    logfile << val << "\t (revisit)" << endl;
  else 
    logfile << val << endl;

  logfile.close();
}


void logPeriodic(int period, float val, string logfileName) {

  ofstream logfile;
  logfile.open(logfileName.c_str(), ios::app);
  logfile << endl << " **** After " << period << " evaluations *****  ";

  logfile << fixed << setiosflags(ios::right);
  logfile.precision(2);
  logfile << "Best value found:\t" << val << endl << endl; 
  
  logfile.close();

} 

/* logs the current number of evaluations and the best value found so far 
   output goes to a different file than the default log file */

void logResultPerEval(int period, float val, string prog, string logfileName) {
  string datafileName;
  string filesuffix = ".cum";
  datafileName = logfileName + filesuffix;
  ofstream datafile;
  datafile.open(datafileName.c_str(), ios::app);
  datafile << setw(5) << period << "\t";

  datafile << fixed << setiosflags(ios::right);
  datafile.precision(2);
  datafile << val << endl; 
  
  datafile.close();

} 


void logFinalResult(float val, string logfileName) {

  ofstream logfile;
  logfile.open(logfileName.c_str(), ios::app);
  logfile << fixed << setiosflags(ios::right);
  logfile.precision(2);
  cout << "Best value: " << val << endl;
  logfile << "Best value: \t" << val << endl; 
  logfile.close();
}
