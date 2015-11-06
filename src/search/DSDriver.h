#ifndef DSDriver_h
#define DSDriver_h

#include<string>
#include<vector>

#include<eval/EvalProgram.h>
#include<search/DSParallel.h>

class DSDriver {

 public:
  DSDriver(string prog, string srcFile, string path, string configfile);
  ~DSDriver();

 
  void ReadSearchConfiguration(string filename);
  void GetExecTime();
  float GetBestTime();

  void DumpBestParams();
  void DumpConfiguration();

  float Start();

  int GetEvals() {return evals;};

 private:
  vector< nSpace<float> > data;
  vector<DSParallel> dsVec; 
  int evals;
  int maxEvals;
  int numOfInstances;
  vector<int> *regions;
  string prog;
  string path;
  string srcFile;
  string configfile;
};

#endif



