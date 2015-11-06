#ifndef DSParallel_h
#define DSParallel_h

#include<search/Point.h>
#include<vector>

using namespace std;

#define STEP_SIZE 20
#define REDUCE_FACTOR 2
//#define STEP_SIZE_FRACTION 40
//#define REDUCE_FACTOR_FRACTION 20
#define MIN_STEP_SIZE 1
#define MIN_REDUCE_FACTOR 1

 
struct optInfo {
  int loop;
  int optCode;
  int bottom;
  int top;
};


class DSParallel {
 public:
  DSParallel();
  DSParallel(int region, int totalparams, int *loopnum, int *optCode, 
	     int *bottom, int *top);
  bool GenParams();
  void MakeExploreMove(int paramnum, bool inc);
  void MakePatternMove();
  void PickInitialPoints();
  
  Point Normalize(Point& p);

  Point GetCurPoint() {return curPoint;};
  void SetCurValue(float val) {curVal = val;};

  float GetBestValue();
  Point GetBestPoint();
  int GetRegion() {return region;};
  int GetLoopNum(int i) {return opts[i].loop;};
  int GetOptCode(int i) {return opts[i].optCode;};
  int GetTotalParams() {return totalparams;};

  int GetBottom(int i) {return opts[i].bottom;}
  int GetTop(int i) {return opts[i].top;};
  int GetSize(int i) {return opts[i].top - opts[i].bottom + 1;};

  bool inRange(Point p);

  ~DSParallel();

 private:  
  enum State {NOTSTARTED, INIT, EXPLORE, PATTERN, DONE} state;
  bool inc;
  int paramnum;
  int region;
  int totalparams;
  
  vector <optInfo> opts;

  int *stepSizes;
  int minStepSize;
  int stepSizeReduceFactor;

  Point minPoint;
  Point curPoint;
  Point basePoint;
  Point explPoint;

  float curVal, baseVal, explVal, minVal;

};

#endif





