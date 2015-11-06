#include<iomanip>
#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
#include<stdlib.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<search/DirectSearch.h>

static const int STEPFACTOR = 50;
static const int REDUCEFACTOR = 30;
static const int DEFAULT_PARTITION = 1;


DirectSearch::DirectSearch(const Args& specs)  
  : SearchAlgo(specs) {
  
  partitions = (int *) malloc(sizeof(int) * GetTotalParams());
  if (NeedToCustomize()) 
    GetCustomParams();
  else {
    cerr << "No customization file for Direct Search. " << endl
	 << "Proceeding with default values ... " << endl;
    stepfactor = STEPFACTOR;
    reducefactor = REDUCEFACTOR;
    for (int i = 0; i < GetTotalParams(); i++)
      partitions[i] = DEFAULT_PARTITION;
  }

  stepSizes = (int *) malloc(sizeof(int) * (GetTotalParams()));
  reduceFactors = (int *) malloc(sizeof(int) * (GetTotalParams()));
  for (int i = 0; i < GetTotalParams(); i++) {
    stepSizes[i] = (int) ceil(((float) 
				(GetTop(i) - GetBottom(i)) * stepfactor)/100);
    reduceFactors[i] = (int) ceil(((float) 
			(GetTop(i) - GetBottom(i)) * reducefactor)/100);
  }
}

DirectSearch::~DirectSearch() {}

void DirectSearch::GetCustomParams() {
  string filename = env.customFile;
  ifstream paramFile(filename.c_str(), ios::in);  
  if (!paramFile) {
    cerr << "Customization file not found :" << paramFile << endl;
    exit(1);
  }
  if (!(paramFile >> stepfactor)) {
    cerr << "Error reading step size from " <<  filename << endl;
    exit(1);
  }
  if (!(paramFile >> reducefactor)) {
    cerr << "Error reading reduction factor from " <<  filename << endl;
    exit(1);
  }

  for (int i = 0; i < GetTotalParams(); i++) {
    if (!(paramFile >> partitions[i])) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    }
  } 
  paramFile.close();
}


void DirectSearch::Permute (int **params, int i, int j, int *result, 	
			    int **curParams, int *p) {
  if  (i == GetTotalParams()) {
    for (int m = 0; m < GetTotalParams(); m++)
      curParams[*p][m] = result[m];
    (*p)++;
    return;
  }
  result[i] = params[i][j];
  if (i + 1 == GetTotalParams()) 
    Permute(params, i + 1, 0, result, curParams, p);
  else
    for (int k = 0; k < partitions[i + 1]; k++)
      Permute(params, i + 1, k, result, curParams, p);
  return;
}

float DirectSearch::ExploreDim(int *curBp, int dim, int localmaxevals, int stepSize) {
    
  bool inRange = false;
  bool foundBetter = false;

  float curVal;
  Metrics *curMetric = new Metrics();

  curBp[dim] += stepSize;
  int rem = 0;
  if (space->GetDimType(dim) == 'N') {
    rem = curBp[dim] % space->GetStride(dim);
    if (rem)
      curBp[dim] += rem;  
  }
  inRange = IsInRange(curBp);  
  if (inRange) {
    if (single) 
      curVal = EvalSpecificMetric(curBp);
    else
      curVal = EvalAllMetric(curBp, curMetric);
    if (space->GetDescent()) 
      foundBetter = bestSoFar > curVal;
    else
      foundBetter = bestSoFar < curVal;
    if (foundBetter) 
      bestSoFar = curVal;
    else {
      curBp[dim] -= stepSize;
      if (space->GetDimType(dim) == 'N') {
	rem = curBp[dim] % space->GetStride(dim);
	if (rem)
	  curBp[dim] += rem;  
      }
    }
    if (evals >= localmaxevals)
      return bestSoFar;
  }
  else {
    curBp[dim] -= stepSize;
    if (space->GetDimType(dim) == 'N') {
      rem = curBp[dim] % space->GetStride(dim);
      if (rem)
	curBp[dim] += rem;  
    }
  }
  if (!inRange || !foundBetter) {

    curBp[dim] -= stepSize;
    if (space->GetDimType(dim) == 'N') {
      rem = curBp[dim] % space->GetStride(dim);
      if (rem)
	curBp[dim] += rem;  
    }
    inRange = IsInRange(curBp);
    if (inRange) {
    if (single) 
      curVal = EvalSpecificMetric(curBp);
    else
      curVal = EvalAllMetric(curBp, curMetric);
      if (space->GetDescent()) 
	foundBetter = bestSoFar > curVal;
      else
	foundBetter = bestSoFar < curVal;
      if (foundBetter) 
	bestSoFar = curVal;
      else { 
	curBp[dim] += stepSize;
	if (space->GetDimType(dim) == 'N') {
	  rem = curBp[dim] % space->GetStride(dim);
	  if (rem)
	    curBp[dim] += rem;  
	}
      }
      if (evals >= localmaxevals)
	return bestSoFar;
    }
    else {
      curBp[dim] += stepSize;
      if (space->GetDimType(dim) == 'N') {
	rem = curBp[dim] % space->GetStride(dim);
	if (rem)
	  curBp[dim] += rem;  
      }
    }
  }
  return bestSoFar;
}


float DirectSearch::Search(int *params, int localmaxevals) {

  int *curStepSizes = (int *) malloc(sizeof(int) * GetTotalParams());
  int *curReduceFactors = (int *) malloc(sizeof(int) * GetTotalParams());
  for (int i = 0; i < GetTotalParams(); i++) {
    curStepSizes[i] = GetStepSize(i);
    curReduceFactors[i] = GetReduceFactor(i);
  }

  int *bp, *next, *rollback;
  bp = (int *) malloc(sizeof(int) * GetTotalParams());
  next = (int *) malloc(sizeof(int) * GetTotalParams());
  rollback = (int *) malloc(sizeof(int) * GetTotalParams());

  float bpVal, rollbackVal;
  Metrics *bpMetric = new Metrics();
  Metrics *rollbackMetric = new Metrics();

  for (int i = 0; i < GetTotalParams(); i++)
    bp[i] = params[i];

  if (single) 
    bpVal = EvalSpecificMetric(bp);
  else
    bpVal = EvalAllMetric(bp, bpMetric);
  bestSoFar = bpVal;

  
  bool done = false;
  bool patternMove = false;
  bool foundBetter = false;
  while (!done) {
    /* make exploratory moves */

    for (int i = 0; i < GetTotalParams(); i++)
      next[i] = bp[i];

    for (int i = 0; i < GetTotalParams(); i++) {
      ExploreDim(next, i, localmaxevals, curStepSizes[i]);
      
      if (evals >= localmaxevals) {
	for (int k = 0; k < GetTotalParams(); k++)  
	  params[k] = next[k]; 
	free(curStepSizes);
	free(curReduceFactors);
	return bestSoFar;
      }
    }
    
    if (space->GetDescent()) 
      foundBetter = bestSoFar > bpVal;
    else
      foundBetter = bestSoFar < bpVal;

    if (foundBetter) {
      /* make pattern move */
      for (int i = 0; i < GetTotalParams(); i++)
	rollback[i] = next[i];
      rollbackVal = bestSoFar;

      int rem = 0;
      for (int i = 0; i < GetTotalParams(); i++) {
	bp[i] = 2 * next[i] - bp[i];
        if (space->GetDimType(i) == 'N') {
	  rem = bp[i] % space->GetStride(i);
	  if (rem)
	    bp[i] += rem;  
	}
      }

      ForcePointToRange(bp);

      if (single) 
	bpVal = EvalSpecificMetric(bp);
      else
	bpVal = EvalAllMetric(bp, bpMetric);

      if (space->GetDescent()) 
	bestSoFar = bestSoFar < bpVal ? bestSoFar : bpVal;
      else 
	bestSoFar = bestSoFar > bpVal ? bestSoFar : bpVal;

      patternMove = true;

      if (evals >= localmaxevals) {
	for (int k = 0; k < GetTotalParams(); k++) {
	  if (space->GetDescent()) {
	    if (bpVal < rollbackVal)
	      params[k] = bp[k]; 
	    else 
	      params[k] = rollback[k];
	  }
	  else {
	    if (bpVal > rollbackVal)
	      params[k] = bp[k]; 
	    else 
	      params[k] = rollback[k];
	  }
	}
	free(curStepSizes);
	free(curReduceFactors);
	return bestSoFar;
      }
      /* reset step sizes */
      for (int i = 0; i < GetTotalParams(); i++) 
	curStepSizes[i] = GetStepSize(i);
    }
    else {
      if (space->GetDescent()) 
	foundBetter = bestSoFar < rollbackVal;
      else
	foundBetter = bestSoFar > rollbackVal;
      if (patternMove && !foundBetter) {
	  for (int i = 0; i < GetTotalParams(); i++)
	    bp[i] = rollback[i];
	  bpVal = rollbackVal;
	  patternMove = false;
      }
      else {
	int i, j, k;
	/* This is the situation when all exploratory moves for a point 
	   have failed. Instead of baling out at this point (by returning
	   a local minima), we refine the search as follows:
	   
	    (1) Attempt to reduce the step size for each parameter. The
	        idea is to search a tighter space around a point. If 
	        we find a parameter that we can reduce, reduce it and
		continute with search, otherwise go to step 2
	    (2) Attempt to reduce the reduction factor. Idea is to perform
	        the search with finer granularity. If we can't reduce any
		of the reduction factors then we are done. Otherwise reduce
		a reduction factor and continue with search
	*/
	for (i = 0; curStepSizes[i] <= MIN_STEP_SIZE && i < GetTotalParams(); i++);
	if (i == GetTotalParams()) { 
	  for (j = 0; 
	       curReduceFactors[j] <= MIN_REDUCE_FACTOR && j < GetTotalParams(); j++);
	  if (j == GetTotalParams())
	    done = true;
	  else {
	    curReduceFactors[j]--;
	    for (k = 0; k < GetTotalParams(); k++) 
	      curStepSizes[k] = GetStepSize(k);
	    curStepSizes[j] -= curReduceFactors[j];
	  }
	}
	else
	  curStepSizes[i] -= curReduceFactors[i];
      }
    }
  }

  for (int k = 0; k < GetTotalParams(); k++)  
    params[k] = bp[k];
  free(curStepSizes);
  free(curReduceFactors);
  return bestSoFar;
}  

float DirectSearch::Start() {
  
  /* seed RNG with current time */
  time_t t;
  int randseed = (int) time(&t);
  srand(randseed);

  /* pick initial points for each partition */
  int **params;
  params = (int **) malloc(sizeof(int *) * GetTotalParams());
  for (int i = 0; i < GetTotalParams(); i++)
    params[i] = (int *) malloc(sizeof(int) * partitions[i]);
  for (int i = 0; i < GetTotalParams(); i++) {
    int j;
    int base = GetBottom(i);
    int numValues = GetSize(i)/partitions[i];

    for (j = 0; j < partitions[i] - 1; j++) {
      params[i][j] = numValues/2 + base;
      base = base + numValues;
    }
    int remainder = GetSize(i) % partitions[i];
    params[i][j] = (numValues + remainder)/2 + base;
  }

  int permutes = 1;
  for (int i = 0; i < GetTotalParams(); i++)
    permutes *= partitions[i];
  int **curParams = (int **) malloc(sizeof(int *) * permutes);
  for (int i = 0; i < permutes; i++)
    curParams[i] = (int *) malloc(sizeof(int) * GetTotalParams());
  int p = 0;
  int *result = (int *) malloc(sizeof(int) * GetTotalParams());
  for (int i = 0; i < partitions[0]; i++) 
    Permute(params, 0, i, result, curParams, &p);

#if DEBUG
    cerr << "Printing search space permutations" << endl;
    for(int i = 0; i < permutes; i++) {
      cerr << "Permutation " << i + 1 << endl; 
      for(int j = 0; j < GetTotalParams(); j++) {
	cerr << "    Param " << j + 1 << ": " << curParams[i][j] << endl;
      }
      cerr << endl;
    }
#endif 


  if (permutes <= 1) {
    /* hard code initial points for fusion configuration */

//     curParams[0][0] = 1;
//     curParams[0][1] = 0;
//     curParams[0][2] = 0;
//     curParams[0][3] = 1;
//     curParams[0][4] = 0;
//     curParams[0][5] = 0;
//     curParams[0][6] = 0;
    
    int rem = 0;
    for (int i = 0; i < GetTotalParams(); i++) {
    if (env.mode == "POET")
      curParams[0][i] = space->GetDefVal(i);
    else 
      curParams[0][i] = (rand() % GetSize(i)) + GetBottom(i);

      if (space->GetDimType(i) == 'N') {
	rem = curParams[0][i] % space->GetStride(i);
	  if (rem)
	    curParams[0][i] += rem;  
	}

    }

    bestSoFar = Search(curParams[0], GetEvals()); 
  }
  else {
    /* 
       A permutation of dimensions represents a section of the search space. A 
       separate instance of DS is invoked for each section. The section 
       where the best value is found is recorded. A final instance of 
       DS is run on this section with the remaining iterations. 
       
       Each DS instance is allowed to run until a fixed number of evaluations
       have been performed. If the search stops at a local minima before the
       specified number of evaluations then the saved evaluations are used for 
       the next instance of the seach. 
    */
    float curResult = 0.0;
    int localmaxevals = GetEvals()/(permutes + 1);
    int remainingevals = GetEvals() % (permutes + 1);
    int localevals;
    int permuteNum = 0;
    int savedevals = 0;
    for (int i = 0; i < permutes; i++) {
      localevals = 0;
      curResult = Search(curParams[i], localmaxevals + savedevals); 
      if (curResult < bestSoFar || bestSoFar == 0) {
	bestSoFar = curResult;
	permuteNum = i;
      }
      savedevals = evals - localmaxevals * (i + 1);
    }

    while (evals < localmaxevals + savedevals) {
      curResult = Search(curParams[permuteNum], 
			 remainingevals + localmaxevals + savedevals); 
      if (curResult < bestSoFar)
	bestSoFar = curResult;
    }
  }

  return bestSoFar;
}














