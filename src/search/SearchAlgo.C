#include<iostream>
#include<algorithm>
#include<fstream>
#include<stdlib.h>

using namespace std;

#include<search/SearchAlgo.h>
#include<eval/EvalProgram.h>
#include<driver/SearchLog.h>
#include<interface/SearchSpace_PSEAT.h>
#include<interface/POETSearchSpec.h>

#define READ(CALL)\
if ((CALL) == 0) {\
  perror(#CALL);\
  exit(1);\
 }									

int fact(int n) {
  if (n == 0)     
    return 1;
  else
    return n * fact(n - 1);
}

SearchAlgo::SearchAlgo(const Args& specs) 
  : env(specs) {
  single = true;

  evals = 0;
  bestSoFar = 0.0;

  truncfactor = 1.0;

  space = new SearchSpace_PSEAT(env.configFile, env.mode);
  space->Dump();
  if (env.mode == "POET")
    GenPOETInvocScript(specs, space);
  

#if 0
   sdata.resize(PERIOD);
   for (unsigned int i = 0; i < sdata.size(); i++)
     sdata[i].index = -1; 
#endif

  if (!space->GetMode()) {
    ReadData(space->GetDataFileName());
  }
}


float SearchAlgo::EvalAllMetric(int *coords, Metrics *metrics) {

  vector<int>  *mappedParams = new vector<int>;

  float val;
  float currentBest;

  if (!InDb(coords)) {
    if (!space->GetMode()) {
      cout << "Did not find value for search point in DB" << endl;
      exit(1);
    }
    MapParamsToSearchSpace(coords, mappedParams);
    val = EvalProg(env, mappedParams, metrics);
    AddValue(coords, val);
    mappedParams->clear();
  }
  else 
    val = GetValue(coords);

  evals++;

  if (NeedToLog()) {
    logSingleEval(GetTotalParams(), coords, val, 't', env.logfileName);
    if (space->GetDescent())
      currentBest = val > bestSoFar ? bestSoFar : val;
    else 
      currentBest = val < bestSoFar ? bestSoFar : val;
    if (evals % PERIOD == 0)
      logPeriodic(evals, currentBest, env.logfileName);
    if (bestSoFar == 0.0) 
      logResultPerEval(evals, val, env.prog, env.logfileName);
    else 
      logResultPerEval(evals, currentBest, env.prog, env.logfileName);
  }

  delete mappedParams;
  return val;
}


float SearchAlgo::EvalSpecificMetric(int *coords) {

  vector<int>  *mappedParams = new vector<int>;

  float val;
  float currentBest;
  
  if (!InDb(coords)) {
    if (!space->GetMode()) {
      cout << "Did not find value for search point in DB" << endl;
      exit(1);
    }
    MapParamsToSearchSpace(coords, mappedParams);
    val = EvalProgWithMetric(env, mappedParams, space->GetMetric());
    AddValue(coords, val);
    mappedParams->clear();
  }
  else
    val = GetValue(coords);

  evals++;

  if (NeedToLog()) {
    logSingleEval(GetTotalParams(), coords, val, 't', env.logfileName);
    if (space->GetDescent())
      currentBest = val > bestSoFar ? bestSoFar : val;
    else 
      currentBest = val < bestSoFar ? bestSoFar : val;
    if (evals % PERIOD == 0)
      logPeriodic(evals, currentBest, env.logfileName);
    if (bestSoFar == 0.0) 
      logResultPerEval(evals, val, env.prog, env.logfileName);
    else 
      logResultPerEval(evals, currentBest, env.prog, env.logfileName);
  }

  delete mappedParams;
  return val;
}


void SearchAlgo::GetNthPermutation(int n, int size, int *params) {
  for (int i = 0; i < size; i++) 
    params[i] = i + 1;
  for (int i = 0; i < n && next_permutation(params, params + size); i++);
}

void SearchAlgo::DumpParamToFile(int *params) {

  string mode = env.mode;
  string outfileName = env.testdir + "/pt.pseat";
  ofstream outfile;

  outfile.open(outfileName.c_str(), ios::out);
  if (!outfile) {
    cerr << "Could not open config file " << outfileName << endl;
    exit(1);
  }
  for (int i = 0; i < GetTotalParams(); i++) {
    if (space->GetDimType(i) == 'N') {
      if (mode == "LOOPTOOL") {
      }
      else if (mode == "POET") {
      }
      else
	outfile << params[i] << " ";
    }
    if (space->GetDimType(i) == 'P') {
      int size = space->GetPermuteCount(i);
      int permute[size];
      for (int j = 0; j < size; j++) 
	permute[j] = j + 1;
      for (int j = 0; j < params[i] && next_permutation(permute, permute + size); j++);
      
      if (mode == "LOOPTOOL") {
      }
      else if (mode == "POET") {
      }
      else {
	outfile << "(";
	for (int j = 0; j < size; j++) 
	  if (j < size - 1)
	    outfile << permute[j] << ",";
	  else
	    outfile << permute[j];	    
	outfile << ") ";
      }
    }

    if (space->GetDimType(i) == 'E') {
      if (mode == "LOOPTOOL") {
      }
      else if (mode == "POET") {
      }
      else {
	outfile << space->GetEnumValue(i, params[i]) << " ";
      }
    }
  }
  outfile << endl;
  outfile.close();
}

void SearchAlgo::MapParamsToSearchSpace(int *params, vector<int> *mappedParams) {

  for (int i = 0; i < GetTotalParams(); i++) {
    if (space->GetDimType(i) == 'N') {
      mappedParams->push_back(params[i]);
    }
    if (space->GetDimType(i) == 'P') {
      int size = space->GetPermuteCount(i);
      int permute[size];
      for (int j = 0; j < size; j++) 
	permute[j] = j + 1;
      for (int j = 0; j < params[i] && next_permutation(permute, permute + size); j++);
      for (int j = 0; j < size; j++) 
	mappedParams->push_back(permute[j]);
    }
    if (space->GetDimType(i) == 'E') {
      mappedParams->push_back(space->GetEnumValue(i, params[i]));
    }
  }
}

bool SearchAlgo::IsInRange(int *coord) {
  for (int i = 0; i < GetTotalParams(); i++) {
    if (coord[i] < GetBottom(i) || coord[i] > GetTop(i))
      return false;
  }
  if (GetConstraints()) {
    if (coord[1] < coord[0])
      return false;
  }
  return true;  
} 

void SearchAlgo::ForcePointToRange(int *point) {
  for (int i = 0; i < GetTotalParams(); i++) {
    if (point[i] < GetBottom(i)) 
      point[i] = GetBottom(i);
    if (point[i] > GetTop(i)) 
      point[i] = GetTop(i);
  }
}

int SearchAlgo::GetIndexFromCoord(int dims, int *coord) {
  int index = 0;
  for (int i = 0; i < dims; i++) {
    int pos = 1;
    for (int j = i + 1; j < dims; j++) 
      pos = pos * GetSize(j);  
    index = index + (coord[i] - GetBottom(i)) * pos;
  }
  return index;
}

bool SearchAlgo::Visited(int *coord) {
  int index = GetIndexFromCoord(GetTotalParams(), coord);
  for (unsigned int i = 0; i < sdata.size(); i++) {
    if (sdata[i].index == index) {
      if (sdata[i].visited == true)
	return true;
    } 
  }
  return false;
}

bool SearchAlgo::InDb(int *coord) {
  
  int index = GetIndexFromCoord(GetTotalParams(), coord);
  for (unsigned int i = 0; i < sdata.size(); i++) {
    if (sdata[i].index == index) 
      return true;
  }
  return false;
}


float SearchAlgo::GetValue(int *coord) {
  int index = GetIndexFromCoord(GetTotalParams(), coord);
  for (unsigned int i = 0; i < sdata.size(); i++) {
    if (sdata[i].index == index)  {
      sdata[i].visited = true;
      return sdata[i].val;
    }
  }
  return 0.0;
}

void SearchAlgo::SetValue(int *coord, float val) {
  
  int index = GetIndexFromCoord(GetTotalParams(), coord);
  for (unsigned int i = 0; i < sdata.size(); i++) {
    if (sdata[i].index == index)  {
      sdata[i].val = val;
      return;
    }
  }
}

void SearchAlgo::AddValue(int *coord, float val) {
  int index = GetIndexFromCoord(GetTotalParams(), coord);
  searchData item;
  item.index = index;
  item.val = val;
  item.visited = false;
  sdata.push_back(item);
}


void SearchAlgo::Normalize(int *coord) {
  for (int i = 0; i < GetTotalParams(); i++) {
    coord[i] = coord[i] - GetBottom(i); 
  }
}


void SearchAlgo::ReadData(string filename) {

  FILE *datafile = fopen(filename.c_str(), "r");
  if (!datafile) {
    fprintf(stderr, "Could not open data file %s\n", filename.c_str());
    exit(-1);
  }
    
  int ret;
  float val;
  int *dims = (int *) malloc(sizeof(int) * GetTotalParams());

  while ((ret = fscanf(datafile, "%d", &dims[0])) == 1) {
    for (int k = 1; k < GetTotalParams(); k++) {
      if (fscanf(datafile, "%d", &dims[k]) != 1) {
	fprintf(stderr, "Error reading parameter\n");
	exit(-1);
      } 
    }
    if (fscanf(datafile, "%f", &val) != 1) {
      fprintf(stderr, "Error reading value\n");
      exit(-1);
    }
    AddValue(dims, val);
  }
  if (ret != EOF) {
      fprintf(stderr, "Error reading data: EOF\n");
      exit(-1);
  }
  free(dims);
  fclose(datafile);
}
