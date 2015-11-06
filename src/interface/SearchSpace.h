#ifndef SearchSpace_h
#define SearchSpace_h

#include<eval/PerfMetric.h>

#include <string>

using namespace std;

class SearchSpace {
 public:

  virtual int GetEvals() = 0;
  virtual int GetTotalParams() = 0;
  virtual int GetSize(int i) = 0;
  virtual int GetBottom(int i) = 0;
  virtual int GetTop(int i) = 0;
  virtual char GetDimType(int i) = 0;

  virtual int GetConstraints() = 0;
  virtual bool GetMode() = 0;
  virtual bool GetDescent() = 0;
  virtual int GetStride(int i) = 0;
  virtual int GetPermuteCount(int i) = 0;
  virtual int GetEnumValue(int i, int j) = 0;
  virtual char* GetName(int i) = 0;
  virtual int GetRepr(int i) = 0;
  virtual int GetDefVal(int i) = 0;

  virtual string GetDataFileName() = 0;

  virtual void SetEvals(int evals) = 0;
  virtual void SetConstraints(int _constraints) = 0;

  virtual enum metricType GetMetric() = 0;
  virtual void SetMetric(enum metricType m) = 0;


  virtual void Dump() = 0;

};

#endif
