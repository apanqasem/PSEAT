#ifndef IID_h
#define IID_h

#include<ml/ML.h>
#include<driver/Args.h>

#define BEST_PROX_THRESHOLD 90
#define WORST_PROX_THRESHOLD 90
#define REV_ADJUST 1.00

class IID : public ML {

 public:
  IID(const Args& specs);
  ~IID();

  float Start(float baseVal = INITVAL);

  void ReadParams(string filename);

  float findMaxVal();
  float findMinVal();

 private:
};

#endif
