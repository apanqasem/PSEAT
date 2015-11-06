#ifndef SEARCHSPACE_PSEAT_H
#define SEARCHSPACE_PSEAT_H

#include<string>
#include<interface/SearchSpace.h>
#include<eval/PerfMetric.h>


#define MAXTRANSFORMNAME 256

using namespace std;

class SearchSpace_PSEAT : public SearchSpace {

public:
  SearchSpace_PSEAT() {};
  SearchSpace_PSEAT(string filename, string mode);

  int GetEvals() {return maxEvals;};
  int GetTotalParams() {return totalparams;};
  int GetSize(int i) {return top[i] - bottom[i] + 1;};
  int GetBottom(int i) {return bottom[i];}
  int GetTop(int i) {return top[i];};
  char GetDimType(int i)  { return dimTypes[i]; };
  int GetConstraints() {return constraints;};
  bool GetMode() { return online;};
  bool GetDescent() { return descent;};
  int GetStride(int i) { return info[i]; };
  int GetPermuteCount(int i) { return info[i]; };
  char* GetName(int i) { return name[i]; };
  int GetRepr(int i) { return dimRepr[i]; };
  int GetDefVal(int i) { return defval[i]; };
  
  string GetDataFileName() { return datafile; };
  int GetEnumValue(int i, int j) { return enumList[i][j]; };

  void SetEvals(int evals) {maxEvals = evals;};
  void SetConstraints(int _constraints) {constraints = _constraints;};

  enum metricType GetMetric() { return metric;};
  void SetMetric(enum metricType m) { metric = m;}

  void ExtractSpaceFromDefault(string filename);
  void ExtractSpaceFromPOET(string filename);

  void Dump();

protected:

  bool descent;  /* search mode : ascent vs. descent */
  bool online;
  int maxEvals;
  string datafile;      
  int constraints;

  char **name;
  int *bottom;
  int *top;
  char *dimTypes;
  int *info;
  int *defval;
  int *dimRepr;

  int totalparams;
  int **enumList;
  enum metricType metric;
};

#endif
