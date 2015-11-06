#ifndef ML_h
#define ML_h

#include<string>
#include<vector>

#include<eval/PerfMetric.h>
#include<driver/Args.h>
#include<interface/SearchSpace.h>

#define PERIOD 30
#define INITVAL 0

typedef struct {
  int *params;
  int index;
  bool visited;
  float val;
} mlData;

class ML {
public:
  ML(const Args& specs);


  ~ML() {}; 
  inline bool NeedToLog() {return !env.logfileName.empty();};
  inline bool NeedToCustomize() {return !env.customFile.empty();};

  void SetEvals(int evals) {space->SetEvals(evals);};
  void SetConstraints(int constraints) {space->SetConstraints(constraints);};

  int GetEvals() {return space->GetEvals();};
  int GetTotalParams() {return space->GetTotalParams();};
  int GetSize(int i) {return space->GetSize(i);};
  int GetBottom(int i) {return space->GetBottom(i);}
  int GetTop(int i) {return space->GetTop(i);};
  int GetConstraints() {return space->GetConstraints();};

  double GetTruncFactor() {return truncfactor;};
  void SetTruncFactor(double factor) {truncfactor = factor;};

  void ReadData(string filename);

  bool IsInRange(int *coord);
  void ForcePointToRange(int *point);
  void DumpParamToFile(int *params);
  void MapParamsToSearchSpace(int *params, vector<int> *mappedParams);
  void GetNthPermutation(int n, int size, int *params);
  void Normalize(int *coord);

  bool InDb(int *coord);
  bool Visited(int *coord);
  float GetValue(int *coord);
  void SetValue(int *coord, float val); 
  void AddValue(int *coord, float val); 

  float EvalAllMetric(int *coords, Metrics *metrics);
  float EvalSpecificMetric(int *coords);

  enum metricType GetMetric() { space->GetMetric(); };
  void SetMetric(enum metricType m) { space->SetMetric(m); };

  SearchSpace* GetSpace() { return space; };

protected:

  Args env;

  bool single;          /* single vs. multiple metric */         

  double truncfactor; 

  int evals;
  float bestSoFar;

  //  string datafile;      /* file name for offline data */
  vector<mlData> sdata;

  SearchSpace *space;

private:
  int GetIndexFromCoord(int dims, int *coords);

};

#endif

