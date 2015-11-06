#include<iomanip>
#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<search/RandomSearch.h>
#include<driver/SearchLog.h>

// svn test

RandomSearch::RandomSearch(const Args& specs) 
 : SearchAlgo(specs) {
}

RandomSearch::~RandomSearch() {}

void RandomSearch::GenNextPoint(int *params) { 
  int rem;
  for (int i = 0; i < space->GetTotalParams(); i++) {
    params[i] = (rand() % GetSize(i)) + GetBottom(i);
    /* random should choose random value - no stride info */
#if 0
    if (space->GetDimType(i) == 'N') {
      rem = params[i] % space->GetStride(i);
      if (rem)
	params[i] += rem;  
    }
#endif
  }
}

float RandomSearch::Start(float baseVal) {
  time_t t;
  int randseed = (int) time(&t);
  srand(randseed);

  int *params = (int *) malloc(sizeof(int) * space->GetTotalParams()); 
  float val;

  GenNextPoint(params);

  Metrics *baseMetric;
  baseVal = EvalSpecificMetric(params);
  bestSoFar = baseVal;

  while (evals < space->GetEvals()) {
    GenNextPoint(params);
    if (IsInRange(params) && !Visited(params)) {
      val = EvalSpecificMetric(params);
      if (space->GetDescent()) {
	if (val < bestSoFar) 
	  bestSoFar = val;
      }
      else {
	if (val > bestSoFar) 
	  bestSoFar = val;
      } 
    }
  }

  free(params);
  return bestSoFar;
}


void RandomSearch::ReadParams(string filename) {

  ifstream conf(filename.c_str(), ios::in);
  if (!conf) {
    cerr << "Could not open customization file " << filename << endl;
    exit(1);
  }
  conf.close();
}
