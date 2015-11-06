#include<iomanip>
#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
#include<sys/time.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<search/Anneal.h>
#include<driver/searchlog.h>

extern string logfileName;

Anneal::Anneal(string _prog, string _path, string configfile, 
			   string _mode) 
 : SearchAlgo(_prog, _path, configfile) {

  GetSearchParamsFromFile();
  mode = "max";

}

Anneal::~Anneal() {
}


void Anneal::GetSearchParamsFromFile() {
  string filename = "anneal.pm";
  ifstream paramFile(filename.c_str(), ios::in);  
  if (!paramFile) {
    cerr << "Could not open search parameter file " << filename << endl;
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

  beta = (float *) malloc(sizeof(int) * totalparams);
  for (int i = 0; i < totalparams; i++) {
    if (!(paramFile >> beta[i])) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    }
  } 
}

float Anneal::Evaluate(int *coords) {

  vector<int>  *mappedParams = new vector<int>;

  float val;

  if (!Visited(coords)) {
    MapParamsToSearchSpace(coords, mappedParams);
    val = EvalProg(*this, mappedParams);
    AddValue(coords, val);
    evals++;
#ifdef LOG
    logSingleEval(totalparams, mappedParams, val, 't');
#endif
    mappedParams->clear();
  }
  else 
    val = GetValue(coords);

  delete mappedParams;
  return val;
}

bool Anneal::ReachedWindowThreshold(int *window) {
  for (int i = 0; i < totalparams; i++) {
    if (window[i] > 1) 
      return false;
  }
  return true;
}

void Anneal::ResizeWindow(int *window) {
  for (int i = 0; i < totalparams; i++) {
    window[i] = (int) (beta[i] * window[i]);
    if (!window[i]) 
      window[i] = 1;
  }
}

void Anneal::ForcePointToRange(int *point) {
  for (int i = 0; i < totalparams; i++) {
    if (point[i] < space->GetBottom(i)) 
      point[i] = space->GetBottom(i);
    if (point[i] > space->GetTop(i)) 
      point[i] = space->GetTop(i);
  }
}


float Anneal::Start(float baseVal) {

  /* seed RNG with current time */
  time_t t;
  int randseed = (int) time(&t);
  srand(randseed);


  evals = 0; 
  float curAlpha = alpha;

  int *basePoint = (int *) malloc(sizeof(int) * totalparams); 
  int *current = (int *) malloc(sizeof(int) * totalparams); 
  int *next = (int *) malloc(sizeof(int) * totalparams); 

  int *window = (int *) malloc(sizeof(int) * totalparams); 

  float bestSoFar, curVal, nextVal;
  


  /* randomly select starting point */
  for (int i = 0; i < totalparams; i++)
    basePoint[i] = (rand() % space->GetSize(i)) + space->GetBottom(i);

  baseVal = Evaluate(basePoint);
  bestSoFar = baseVal;
#ifdef LOG
      logResultPerEval(evals, bestSoFar, prog);
#endif
  /* initial window is entire search space */
  for (int i = 0; i < totalparams; i++)
    window[i] = space->GetSize(i);
  
  while (evals < maxEvals) {
    for (int i = 0; i < samples; i++) {
      /* first probe */
      for (int j = 0; j < totalparams; j++) { 
	if (window[j] == 2)
	  current[j] = rand() % window[j];      
	else
	  current[j] = basePoint[j] + (rand() % window[j]) - window[j]/2;      
      }
      ForcePointToRange(current);

      curVal = Evaluate(current);
#ifdef LOG
      logResultPerEval(evals, bestSoFar, prog);
#endif
      
      /* probe four more times and get the best point */
      for (int k = 0; k < 4; k++) {
	for (int j = 0; j < totalparams; j++)  {
	  if (window[j] == 2)
	    next[j] = rand() % window[j];      
	  else
	    next[j] = basePoint[j] + (rand() % window[j]) - window[j]/2;
	}
	ForcePointToRange(next);

	nextVal = Evaluate(next);
#ifdef LOG
      logResultPerEval(evals, bestSoFar, prog);
#endif
	
	if (mode == "min") {
	  if (nextVal < curVal) { 
	    for (int j = 0; j < totalparams; j++) 
	      current[j] = next[j];
	    curVal = nextVal;
	  }
	}
	else if (mode == "max") {
	  if (nextVal > curVal)  {
	    for (int j = 0; j < totalparams; j++) 
	      current[j] = next[j];
	    curVal = nextVal;
	  }
	}
	if (evals > maxEvals)
	  break;
      }	  

      /* compare probe value with base point */
      if (mode == "min") {
	if (curVal < baseVal) {
	  for (int j = 0; j < totalparams; j++) 
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
	    for (int j = 0; j < totalparams; j++) 
	      basePoint[j] = current[j];
	    baseVal = curVal;
	  }
	}
      }
      else if (mode == "max") {
	if (curVal > baseVal) {
	  for (int j = 0; j < totalparams; j++) 
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
	    for (int j = 0; j < totalparams; j++) 
	      basePoint[j] = current[j];
	    baseVal = curVal;
	  }
	}
      }

      if ((mode == "min" && baseVal < bestSoFar) ||
	  (mode == "max" && baseVal > bestSoFar))
	  bestSoFar = baseVal;
      
      if (evals > maxEvals) 
	break;
#ifdef LOG
      logResultPerEval(evals, bestSoFar, prog);
#endif
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


#if 0
void Anneal::ReadData(char *filename) {

  FILE *datafile = fopen(filename, "r");
  if (!datafile) {
    fprintf(stderr, "Could not open data file %s\n", filename);
    exit(-1);
  }
    
  int ret;
  float val;
  int *dims = (int *) malloc(sizeof(int) * totalparams);

  while ((ret = fscanf(datafile, "%d", &dims[0])) == 1) {
    for (int k = 1; k < totalparams; k++) {
      if (fscanf(datafile, "%d", &dims[k]) != 1) {
	fprintf(stderr, "Error reading parameter\n");
	exit(-1);
      } 
    }
    if (fscanf(datafile, "%f", &val) != 1) {
      fprintf(stderr, "Error reading value\n");
      exit(-1);
    }
    printf("%f\n", val);
    int *convertedDims = (int *) malloc(sizeof(int) * totalparams);
    for (int i = 0; i < totalparams; i++) 
      convertedDims[i] = dims[i] - bottom[i]; 
#if 0
      data.SetValue(val, convertedDims);
      data.Visited(convertedDims);
#endif
  }
  if (ret != EOF) {
      fprintf(stderr, "Error reading data: EOF\n");
      exit(-1);
  }
  fclose(datafile);
}

#endif
