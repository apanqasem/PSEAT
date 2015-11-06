#include<math.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h> 
#include<iostream>
#include<sstream>
#include<fstream>
#include<stdlib.h>
#include<string.h>

using namespace std; 

#include<driver/SearchLog.h>
#include<eval/EvalProgram.h>

bool Contains(int region, vector<int> *regions) {
  if (!regions) 
    return true;
  for (unsigned int i = 0; i < regions->size(); i++) {
    if ((region - 1) == (*regions)[i]) 
      return true;
  }
  return false;
}

int RunShScript(string scriptName, char **args) {
  int result;
  int ret;
  if ((ret = fork()) == 0) {
    cerr <<  "Executing script " << scriptName << endl;
    execvp(scriptName.c_str(), args);
    cerr << "Unable to execute script: " << scriptName << endl; 
    exit(1);
  }
  else if (ret < 0) {
    cerr << "Unable to spawn child to execute build_run_profile.sh" << endl;
    exit(1);
  }
  else {
    wait(&result);
    if (WEXITSTATUS(result) != 0) {
      cerr << scriptName <<  " did not complete " << endl; 
      exit(1);
    }
    return 1;
  }
  return 0;
}

float GetPerfData(string filename, double truncFactor, float *metrics, enum metricType metric) {

  ifstream perfdata(filename.c_str(), ios::in);

  string procname;
  //  float data;
  
  if (!perfdata) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  int i = 0;
  while (!perfdata.eof() && perfdata.good() && i < NUMMETRICS) {
    perfdata >> metrics[i];
    i++;
  }
  float val = metrics[metric];
  return val;

  if (perfdata.eof()) {
    cerr <<  "Procedure performance data not found" << endl;
    exit(1);
  }
  if (!perfdata.good()) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  cerr << "Done getting performance data" << endl;
  return 0.0;
}

float GetPerfData(string filename, double truncFactor, float *metrics) {

  ifstream perfdata(filename.c_str(), ios::in);

  string procname;
  //  float data;
  
  if (!perfdata) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  int i = 0;
  while (!perfdata.eof() && perfdata.good() && i < NUMMETRICS) {
    perfdata >> metrics[i];
    i++;
  }
  return metrics[0];

  if (perfdata.eof()) {
    cerr <<  "Procedure performance data not found" << endl;
    exit(1);
  }
  if (!perfdata.good()) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  cerr << "Done getting performance data" << endl;
  return 0.0;
}

float GetAllPerfData(string filename, double truncFactor, Metrics *metrics) {

  ifstream perfdata(filename.c_str(), ios::in);

  string procname;
  
  if (!perfdata) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  int i = 0;
  float val = 0.0;
  while (!perfdata.eof() && perfdata.good() && i < NUMMETRICS) {
    perfdata >> val;
    metrics->SetMetric(i, val);
    i++;
  }

  if (perfdata.eof()) {
    cerr <<  "Procedure performance data not found" << endl;
    exit(1);
  }
  if (!perfdata.good()) {
    cerr << "Error reading performance data" << endl;
    exit(1);
  }
  return metrics->GetMetric(0);
}

float EvalProg(const Args& env, vector<int> *params, Metrics *metrics) {

  string progFlag = "-- "; 
  string prog = env.prog;

  string pathFlag = "-d ";
  string path = env.testdir;

  string mode = env.mode;
  string modeFlag = "-m ";

  string evalscript = env.evalscript;


  char **args;
  int argnum;

  if (params)
    argnum = CORE_ARGS + params->size() + NULL_ARG;
  else 
    argnum = CORE_ARGS + NULL_ARG + 2;

  args = (char **) malloc(sizeof(char *) * argnum);

  for (int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);

  args[0] = (char *) evalscript.c_str();

  args[1] = (char *) pathFlag.c_str();
  args[2] = (char *) path.c_str();

  args[3] = (char *) modeFlag.c_str();
  args[4] = (char *) mode.c_str();

  args[5] = (char *) progFlag.c_str();
  args[6] = (char *) prog.c_str();

  if (params) {
    int param;
    for (int i = CORE_ARGS; i < argnum - 1; i++) {
      param = (*params)[i - CORE_ARGS];
      sprintf(args[i], "%d", param);
    }
  }

  args[argnum - 1] = NULL; 

#if DEBUG
  cerr << "Printing arguments to shell script" << endl;
  for (int i = 0; i < argnum - 1; i++) 
    cerr << "Arg " << i << ": " <<  args[i] << endl;
#endif

  if (!RunShScript(evalscript, args)) {
    cerr << "Error running script: " << evalscript << endl;
    exit(1);
  }
  

  string filename = path + "/perf.pseat";
  float val = GetAllPerfData(filename, 1.0, metrics);
  return val;
}

float EvalProgWithMetric(const Args& env, vector<int> *params, enum metricType metric) {
  string progFlag = "-- "; 
  string prog = env.prog;

  string pathFlag = "-d "; // + env.testdir;
  string path = env.testdir;

  string modeFlag = "-m ";
  string mode = env.mode;
 
  string evalscript = env.evalscript;
  
  char **args;
  int argnum;

  string progArgs = "-f 100 conf -b 1 0 2";
#if 0
  if (params)
    argnum = CORE_ARGS + params->size() + NULL_ARG;
  else 
    argnum = CORE_ARGS + NULL_ARG + 2;
#endif
  if (params)
    argnum = EVAL_SCRIPT_OPTIONS + 9 + NULL_ARG + params->size();
  else 
    argnum = CORE_ARGS + NULL_ARG + 2;


  args = (char **) malloc(sizeof(char *) * argnum);

  for (int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);

  args[0] = (char *) evalscript.c_str();

  args[1] = (char *) pathFlag.c_str();
  args[2] = (char *) path.c_str();

  args[3] = (char *) modeFlag.c_str();
  args[4] = (char *) mode.c_str();

  ostringstream stm;
  stm << params->size();
  string dims = stm.str();
  string ptFlag = "-p ";
  args[5] = (char *) ptFlag.c_str();
  args[6] = (char *) dims.c_str();

  if (params) {
    int param;
    for (int i = 0; i < params->size(); i++) {
      ostringstream stm;
      stm << (*params)[i];
      string thisDim =  stm.str();
      strcpy(args[i + 7], (char *) thisDim.c_str());
    }
  }

  int next = 7 + params->size();
  args[next] = (char *) progFlag.c_str();
  args[next + 1] = (char *) prog.c_str();
  args[next + 2] = (char *) progArgs.c_str();
  args[next + 3] = NULL; 


#if DEBUG
  cerr << "Printing arguments to shell script" << endl;
  for (int i = 0; i < 15; i++) 
    cerr << "Arg " << i << ": " <<  args[i] << endl;
#endif

  if (!RunShScript(evalscript, args)) {
    cerr << "Error running script: " << evalscript << endl;
    exit(1);
  }

  string filename = path + "/perf.pseat";
  float *metrics = (float *) malloc(sizeof(float) * NUMMETRICS);
  return GetPerfData(filename, 1.0, metrics, metric);
}


