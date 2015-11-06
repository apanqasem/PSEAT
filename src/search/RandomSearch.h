#ifndef RandomSearch_h
#define RandomSearch_h

#include<search/SearchAlgo.h>
#include<driver/Args.h>

class RandomSearch : public SearchAlgo {

 public:
  RandomSearch(const Args& specs);
  ~RandomSearch();

  void GenNextPoint(int *params);
  float Start(float baseVal = INITVAL);

  void ReadParams(string filename);

 private:
};

#endif
