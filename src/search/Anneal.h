#ifndef Anneal_h
#define Anneal_h

#include<driver/Args.h>
#include<search/SearchAlgo.h>

class Anneal : public SearchAlgo {

public:
  Anneal(const Args& specs);
  ~Anneal();


  float Start(float baseVal = INITVAL);
  

private:
  
  void GetCustomParams();
  bool ReachedWindowThreshold(int *window);
  void ResizeWindow(int *window);

  float alpha;
  float *beta;
  int samples;

};

#endif
