#include<iomanip>
#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
#include<stdlib.h>
#include<sys/time.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<eval/PerfMetric.h>
#include<search/Anneal.h>
#include<driver/SearchLog.h>

static const int SAMPLES = 10;
static const float ALPHA = 1.0;
static const float BETA = 0.5;


Anneal::Anneal(const Args& specs) 
  : SearchAlgo(specs) {
  beta = (float *) malloc(sizeof(int) * GetTotalParams());
  if (NeedToCustomize()) 
    GetCustomParams();
  else {
    cerr << "No customization file for Simulated Annealing. " << endl
	 << "Proceeding with default values ... " << endl;
    samples = SAMPLES;
    alpha = ALPHA;
    for (int i = 0; i < GetTotalParams(); i++)
      beta[i] = BETA;
  } 
}

Anneal::~Anneal() {}

void Anneal::GetCustomParams() {
  string filename = env.customFile;
  ifstream paramFile(filename.c_str(), ios::in);  
  if (!paramFile) {
    cerr << "Customization file not found :" << paramFile << endl;
    exit(1);  
  }
  if (!(paramFile >> samples)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }
  if (!(paramFile >> alpha)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }
    
  for (int i = 0; i < GetTotalParams(); i++) {
    if (!(paramFile >> beta[i])) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    }
  } 
  paramFile.close();
}
  
bool Anneal::ReachedWindowThreshold(int *window) {
  for (int i = 0; i < GetTotalParams(); i++) {
    if (window[i] > 1) 
      return false;
  }
  return true;
}

void Anneal::ResizeWindow(int *window) {
  for (int i = 0; i < GetTotalParams(); i++) {
    window[i] = (int) (beta[i] * window[i]);
    if (!window[i]) 
      window[i] = 1;
  }
}

float Anneal::Start(float baseVal) {

  /* seed RNG with current time */
  time_t t;
  int randseed = (int) time(&t);
  srand(randseed);


  evals = 0; 
  float curAlpha = alpha;

  int *basePoint = (int *) malloc(sizeof(int) * GetTotalParams()); 
  int *current = (int *) malloc(sizeof(int) * GetTotalParams()); 
  int *next = (int *) malloc(sizeof(int) * GetTotalParams()); 

  int *window = (int *) malloc(sizeof(int) * GetTotalParams()); 

  float curVal, nextVal;
  Metrics *baseMetric = new Metrics();
  Metrics *curMetric = new Metrics();
  Metrics *nextMetric = new Metrics();
  
  /* randomly select starting point */
  int rem = 0;
  for (int i = 0; i < GetTotalParams(); i++) {
    
    if (env.mode == "POET")
      basePoint[i] = space->GetDefVal(i);
    else 
      basePoint[i] = (rand() % GetSize(i)) + GetBottom(i);

    if (space->GetDimType(i) == 'N') {
      rem = basePoint[i] % space->GetStride(i);
      if (rem)
	basePoint[i] += rem;  
    }
  }
  if (single) 
    baseVal = EvalSpecificMetric(basePoint);
  else
    baseVal = EvalAllMetric(basePoint, baseMetric);
  bestSoFar = baseVal;

  /* initial window is entire search space */
  for (int i = 0; i < GetTotalParams(); i++)
    window[i] = GetSize(i);
  
  while (evals < GetEvals()) {
    for (int i = 0; i < samples; i++) {
      /* first probe */
      for (int j = 0; j < GetTotalParams(); j++) { 
	if (window[j] == 2)
	  current[j] = rand() % window[j];      
	else
	  current[j] = basePoint[j] + (rand() % window[j]) - window[j]/2;      
        if (space->GetDimType(j) == 'N') {
	  rem = current[j] % space->GetStride(j);
	  if (rem)
	    current[j] += rem;  
	}
      }
      ForcePointToRange(current);

      if (single) 
	curVal = EvalSpecificMetric(current);
      else
	curVal = EvalAllMetric(current, curMetric);
      if (space->GetDescent()) 
	bestSoFar = curVal < bestSoFar ? bestSoFar : curVal;
      else 
	bestSoFar = curVal < bestSoFar ? bestSoFar : curVal;


      /* probe four more times and get the best point */
      for (int k = 0; k < 4; k++) {
	for (int j = 0; j < GetTotalParams(); j++)  {
	  if (window[j] == 2)
	    next[j] = rand() % window[j];      
	  else
	    next[j] = basePoint[j] + (rand() % window[j]) - window[j]/2;
	  if (space->GetDimType(j) == 'N') {
	    rem = next[j] % space->GetStride(j);
	    if (rem)
	      next[j] += rem;  
	  }
	}
	ForcePointToRange(next);

	if (single) 
	  nextVal = EvalSpecificMetric(next);
	else
	  nextVal = EvalAllMetric(next, nextMetric);
	if (space->GetDescent()) 
	  bestSoFar = nextVal < bestSoFar ? bestSoFar : nextVal;
	else 
	  bestSoFar = nextVal < bestSoFar ? bestSoFar : nextVal;
	

	if (space->GetDescent()) {
	  if (nextVal < curVal) { 
	    for (int j = 0; j < GetTotalParams(); j++)
	      current[j] = next[j];
	    curVal = nextVal;
	  }
	}
	else {
	  if (nextVal > curVal)  {
	    for (int j = 0; j < GetTotalParams(); j++) 
	      current[j] = next[j];
	    curVal = nextVal;
	  }
	}
	if (evals >= GetEvals())
	  break;
      }	  

      /* compare probe value with base point */
      if (space->GetDescent()) {
	if (curVal < baseVal) {
	  for (int j = 0; j < GetTotalParams(); j++) 
	    basePoint[j] = current[j];
	  baseVal = curVal;
	}
	else {
	  float diff = curVal - baseVal;
	  double chance = (double) rand();
	  chance = chance/((double) RAND_MAX);
	  if (chance > 1 || chance < 0) {
	    cerr << "Problem with RNG : " << chance << endl;
	    exit(1);
	  }
	  if (exp(diff/curAlpha) > chance) {
	    for (int j = 0; j < GetTotalParams(); j++) 
	      basePoint[j] = current[j];
	    baseVal = curVal;
	  }
	}
      }
      else {
	if (curVal > baseVal) {
	  for (int j = 0; j < GetTotalParams(); j++) 
	    basePoint[j] = current[j];
	  baseVal = curVal;
	}
	else {
	  float diff = baseVal - curVal;
	  double chance = (double) rand();
	  chance = chance/((double) RAND_MAX);
	  if (chance > 1 || chance < 0) {
	    cerr << "Problem with RNG : " << chance << endl;
	    exit(1);
	  }
	  if (exp(diff/curAlpha) > chance) {
	    for (int j = 0; j < GetTotalParams(); j++) 
	      basePoint[j] = current[j];
	    baseVal = curVal;
	  }
	}
      }

      if ((space->GetDescent() && (baseVal < bestSoFar)) || (!space->GetDescent() && (baseVal > bestSoFar)))
	  bestSoFar = baseVal;
      if (evals >= GetEvals()) 
	break;
    }
    
    ResizeWindow(window);
    if (ReachedWindowThreshold(window))
      break;
		
    curAlpha = alpha * curAlpha;
    samples = (samples = (int) (alpha * samples)) == 0 ? 1 : samples;

#ifdef DEBUG
    cerr << "Sample count = " << samples << endl;
#endif
  }
  return bestSoFar;
}


