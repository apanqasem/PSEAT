#include<iostream>
#include<fstream>

using namespace std;

#include<DSParallel.h>
#include<DSDriver.h>

#define LOG

DSDriver::DSDriver(string _prog, string _srcFile, 
		   string _path, string _configfile) {
  prog = _prog;
  path = _path;
  srcFile = _srcFile;
  configfile = _configfile;
  evals = 0; 
  regions = new vector<int>;
}

DSDriver::~DSDriver() {
}

void DSDriver::ReadSearchConfiguration(string filename) {
  ifstream conf(filename.c_str(), ios::in);
  if (!conf) {
    cerr << "Could not open config file " << filename << endl;
    exit(1);
  }
  if (!(conf >> maxEvals)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }
  if (!(conf >> numOfInstances)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);  
  }
  
  int *totalparams;
  int *bottom, *top, *loopnums;
  int *optCodes;
  int region;
  totalparams = (int *) malloc(sizeof(int) * numOfInstances);
  for (int i = 0; i < numOfInstances; i++) {
    if (!(conf >> region)) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    }
    regions->push_back(region);
    if (!(conf >> totalparams[i])) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    }
    loopnums = (int *) malloc(sizeof(int) * totalparams[i]);
    optCodes = (int *) malloc(sizeof(int) * totalparams[i]);
    bottom = (int *) malloc(sizeof(int) * totalparams[i]);
    top = (int *) malloc(sizeof(int) * totalparams[i]);
    for (int j = 0; j < totalparams[i]; j++) {
      if (!(conf >> loopnums[j] >> optCodes[j] >> bottom[j] >> top[j])) {
	cerr << "Error reading parameters from " <<  filename << endl;
	exit(1);
      }
    }
    DSParallel ds((*regions)[i], totalparams[i], loopnums, optCodes, bottom, top);
    dsVec.push_back(ds);
    free(loopnums);
    free(optCodes);
    free(bottom);
    free(top);
  }
  
  for (int i = 0; i < numOfInstances; i++) {
      nSpace<float> regionData;
      int *dimSizes = (int *) malloc(sizeof(int) * (totalparams[i]));
      for (int j = 0; j < totalparams[i]; j++) 
	dimSizes[j] = dsVec[i].GetSize(j);
      regionData.InitSpace(totalparams[i], dimSizes);
      data.push_back(regionData);
      free(dimSizes);
  }

  free(totalparams);
}


void DSDriver::GetExecTime() {
  vector <float> *values = new vector <float>;
  vector <int> *params = new vector <int>; 
  bool needEval = false;
  float progExec;
  for (int i = 0; i < numOfInstances; i++) {
    Point normCp(dsVec[i].GetTotalParams());
    Point cp(dsVec[i].GetTotalParams());
    cp = dsVec[i].GetCurPoint();
    normCp = dsVec[i].GetCurPoint();
    normCp = dsVec[i].Normalize(normCp);
    if (data[i].Visited(normCp)) {
      dsVec[i].SetCurValue(data[i].GetValue(normCp));
#ifdef LOG
      string logfilename = path + prog + ".log";
      ofstream logfile(logfilename.c_str(), ios::app);
      logfile << "Region " << dsVec[i].GetRegion() << ":\t";
      logfile << "\t[";
      int k;
      for (k = 0; k < dsVec[i].GetTotalParams() - 1; k++) 
	logfile << cp[k] << ", ";
      logfile << cp[k] << "]\t";
      float curval = data[i].GetValue(normCp);
      logfile << curval << "(old)" << endl;
      logfile.close();
#endif
    }
    else {
      needEval = true;
    }
    for (int j = 0; j < dsVec[i].GetTotalParams(); j++) {
      params->push_back(dsVec[i].GetRegion());
      params->push_back(dsVec[i].GetLoopNum(j));
      params->push_back(dsVec[i].GetOptCode(j));
      params->push_back(cp[j]);
    }
  }
  if (needEval) {
    progExec = EvalProg(evals, path, prog, srcFile, regions, values, params);
    evals++;
  }
  int j = 0;
  for (int i = 0; i < numOfInstances; i++) {
    Point normCp(dsVec[i].GetTotalParams());
    int k, l;
    for (k = j + 3, l = 0; l < dsVec[i].GetTotalParams(); l++)
      normCp.SetVal(l, (*params)[k + (l * 4)]); 
    Point curP(dsVec[i].GetTotalParams());
    curP = normCp;
    normCp = dsVec[i].Normalize(normCp);
    j = j + (dsVec[i].GetTotalParams() * 4);
    if (!data[i].Visited(normCp)) {
      data[i].SetValue((*values)[i], normCp);
      dsVec[i].SetCurValue((*values)[i]);
    }
#ifdef LOG    
    if (needEval) {
      string logfilename = path + prog + ".log";
      ofstream logfile(logfilename.c_str(), ios::app);
      logfile << "Region " << dsVec[i].GetRegion() << ":\t";
      logfile << "\t[";
      int k;
      for (k = 0; k < dsVec[i].GetTotalParams() - 1; k++) 
	logfile << curP[k] << ", ";
      logfile << curP[k] << "]\t";
      logfile << (*values)[i] << endl;
      logfile.close();
    }
#endif
  }
#ifdef LOG    
  if (needEval) {
    string logfilename = path + prog + ".log";
    ofstream logfile(logfilename.c_str(), ios::app);
    logfile << "**** Program exec time: " << progExec << " ****" << endl;
    logfile.close();
  }
#endif
  delete values;
  delete params;
}

void DSDriver::DumpConfiguration() {
  string logfilename = path + prog + ".log";
  ofstream logfile(logfilename.c_str(), ios::app);

  logfile << "******** ";
  logfile << "Configuration for concurrent direct search";
  logfile << " ********" << endl;
  logfile << "\tMax evaluations " << maxEvals << endl;
  logfile << "\tNumber of code regions to tune " << numOfInstances << endl;
  logfile << "************************************************************";
  for (int i = 0; i < numOfInstances; i++) {
    logfile << "\t  Region " << i << ":\n";
    logfile << "\t    Number of parameters << " << dsVec[i].GetTotalParams();
    logfile << endl;
  }
  logfile << endl << endl;
  logfile.close();
}

void DSDriver::DumpBestParams() {
}

float DSDriver::GetBestTime() {
  
  vector <float> *values = new vector <float>;
  vector <int> *params = new vector <int>; 

  float val;
#ifdef LOG    
    string logfilename = path + prog + ".log";
    ofstream logfile(logfilename.c_str(), ios::app);
    logfile << endl << endl;
#endif
  for (int i = 0; i < numOfInstances; i++) {
    Point cp(dsVec[i].GetTotalParams());
    cp = dsVec[i].GetBestPoint();
    val = -1;
    values->push_back(val);
    for (int j = 0; j < dsVec[i].GetTotalParams(); j++) {
      params->push_back(dsVec[i].GetRegion());
      params->push_back(dsVec[i].GetLoopNum(j));
      params->push_back(dsVec[i].GetOptCode(j));
      params->push_back(cp[j]);
    }
    cp = dsVec[i].Normalize(cp);
    val = data[i].GetValue(cp);
#ifdef LOG    
    logfile << "Best value for region " << i << ": " << val << endl;
    logfile << "Best Parameters [";
    int k;
    for (k = 0; k < dsVec[i].GetTotalParams() - 1; k++)
      logfile << cp[k] << ", ";
    logfile << cp[k] << "]" << endl << endl;
#endif
  }
#ifdef LOG    
    logfile.close();
#endif
  val = EvalProg(evals, path, prog, srcFile, regions, values, params);
  delete values;
  delete params;
  return val;
}

float  DSDriver::Start() {
  ReadSearchConfiguration(configfile);
#ifdef LOG  
  DumpConfiguration();
#endif
  bool done = false;
  while (evals < maxEvals && !done) {
    done = true;
    for (int j = 0; j < numOfInstances; j++) {
      done = dsVec[j].GenParams() && done;
    }
    GetExecTime();
  }
  return GetBestTime();
}









