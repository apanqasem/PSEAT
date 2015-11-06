#ifndef Exhaustive_h
#define Exhaustive_h

#include<search/SearchAlgo.h>

class Exhaustive : public SearchAlgo {
 public:
  Exhaustive(const Args& specs);
  ~Exhaustive();

  float GenAllPoints(int dim, int *params);
  float Start();
  
 private:

};
#endif
