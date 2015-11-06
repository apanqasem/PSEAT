#ifndef FusionSearch_h
#define FusionSearch_h

#include<vector>

typedef vector <int> loopParams;
#define NUMPARAMS 6
#define MAXLOOPS 200

class FusionSearch : public SearchAlgo {
 public:
  FusionSearch(string _prog, string _path);
  ~FusionSearch();

  void ReadTolerance();
  int ReadGenParamsFromFile(vector <loopParams *> * params);

  bool GenNext();

  float Start();
  bool IsDifferent(vector <loopParams *> *p1, vector <loopParams *> *p2);  
  
  void IncTolerance(int i);
  void DecTolerance(int i);

  bool ReachedThreshold(int i);
    
  void Dump(int entries);
  void DumpTuningParams(int dumpLoc);
  void DumpRunInfo(int evals, float* performance);

 private:
  vector <loopParams *> *allParams;

  bool maximal;
  int fusedLoops;
  string tuneFileName;
  string nextParamsFileName;
  string genFileName;
  string logFileName;

  int numParams;
  int dim;
  int cacheLevel; 
  
  vector <float> *tolerance;
  vector <float> *decrement;
  vector <float> *threshold;

};

#endif


