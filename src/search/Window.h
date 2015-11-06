#ifndef Window_h
#define Window_h

#include<search/SearchAlgo.h>

class Window : public SearchAlgo {

public:
  Window(string _prog, string _path, string configfile, string mode);
  ~Window();

  void GetSearchParamsFromFile();

  float Evaluate(int *coords);

  bool ReachedWindowThreshold(int *window);
  void ResizeWindow(int *window);

  void ForcePointToRange(int *point);

  float Start(float baseVal = INITVAL);
  

private:
  
  string mode;
  float alpha;
  float *beta;
  int samples;

 
};

#endif
