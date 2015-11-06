#include<string>
#include<iostream>
#include<fstream>
#include<stdlib.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<search/Exhaustive.h>
#include<driver/SearchLog.h>


Exhaustive::Exhaustive(const Args& specs) 
 : SearchAlgo(specs) {}

Exhaustive::~Exhaustive() {}

float Exhaustive::GenAllPoints(int dim, int *params) {

  float val;
  /* base case */
  if (dim == GetTotalParams() - 1) {
    for (int i = GetBottom(dim); i <= GetTop(dim); i++) {
      params[dim] = i;
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
    return bestSoFar;
  }
  /* recurse */
  for (int i = GetBottom(dim); i <= GetTop(dim); i++) {
    params[dim] = i;
    GenAllPoints(dim + 1, params);
  }
  return bestSoFar;
}


float Exhaustive::Start() {
    int *coords = (int *) malloc(sizeof(int) * GetTotalParams()); 
    return GenAllPoints(0, coords);
}
