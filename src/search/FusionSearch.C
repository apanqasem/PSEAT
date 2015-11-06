#include<vector>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h> 
#include<iostream>
#include<fstream>
#include<stdlib.h>

using namespace std; 

#include<eval/EvalProgram.h>
#include<search/Assert.h>
#include <FusionSearch.h>


extern string evalscript;
extern string fusionscript;

FusionSearch::FusionSearch(string _prog, string _path) {
  prog = _prog;
  path = _path;
  maximal = false;
  fusedLoops = 0;
  tuneFileName = path + "/" + "tune.conf"; 
  nextParamsFileName = path + "/" + "params.in";
  genFileName = path + "/" + "params.out";
  logFileName = path + "/" + prog + ".log";

  allParams = new vector<loopParams *>(MAXLOOPS);
  
  for (int i = 0; i < MAXLOOPS; i++) {
    (*allParams)[i] = new vector<int>(NUMPARAMS);
  }

  ifstream searchSpace(tuneFileName.c_str(), ios::in);

  if (!searchSpace) {
    cerr << "Could not open search space file " << tuneFileName << endl;
    exit(1);
  }
  if (!(searchSpace >> numParams)) {
    cerr << "Error reading parameters from " <<  tuneFileName << endl;
    exit(1);
  }
  if (!(searchSpace >> dim >> cacheLevel)) {
    cerr << "Error reading parameters from " <<  tuneFileName << endl;
    exit(1);
  }
  tolerance = new vector<float>(numParams);
  decrement = new vector<float>(numParams);
  threshold = new vector<float>(numParams);

  for (int i = 0; i < numParams; i++) {
    if (!(searchSpace >> (*tolerance)[i]
	  >> (*decrement)[i]  
	  >> (*threshold)[i] )){
      cerr << "Error reading tolerance terms " <<  tuneFileName << endl;
      exit(1);
    }
  }
  searchSpace.close();
}

FusionSearch::~FusionSearch() {}

int FusionSearch::ReadGenParamsFromFile(vector <loopParams *> * params) {

  ifstream genParams(genFileName.c_str(), ios::in);
  char newLoopMarker;
  
  int infoLines;
  

  if (!genParams) {
    cerr << "Could not open loop configuration file:  " << genParams << endl;
    exit(1);
  }
  if (!(genParams >> infoLines)) {
    cerr << "Error in generated loop config file: infoLines " << infoLines << endl;
    exit(1);
  }
  if (!(genParams >> fusedLoops)) {
    cerr << "Error in generated loop config file: fusedLoops " << genFileName << endl;
    exit(1);
  }
  if (!(genParams >> maximal)) {
    cerr << "Error in generated loop config file: maximal " << genFileName << endl;
    exit(1);
  }
  
  for (int k = 0; k < infoLines; k++) {  
    /* read info for each loop */
    genParams >> newLoopMarker;
    if (newLoopMarker != '#') {
      cerr << "Error: no loop marker " << genFileName << endl;
      exit(1);
    }
    for (int i = 0; i < NUMPARAMS; i++) {
      int val;
      if (!(genParams >> val)) {
	cerr << "Error in generated loop config file " << genFileName << endl;
	exit(1);
      }
      else {
	(*((*params)[k]))[i] = val;
      }
    }
  }
  genParams.close();
  return infoLines;
}

  
bool FusionSearch::ReachedThreshold(int i) {
  return ((*threshold)[i]) < ((*tolerance)[i]);
}

bool FusionSearch::GenNext() {

  /* set up argument. only need filename and script name */
  unsigned int argnum = 4;
  char **args;
  args = (char **) malloc(sizeof(char *) * argnum);
  for (unsigned int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);

  args[0] = (char *) fusionscript.c_str();
  args[1] = (char *) prog.c_str();
  args[2] = (char *) path.c_str();

  args[argnum - 1] = NULL; 

  vector <loopParams *> *nextParams = new vector<loopParams *>(MAXLOOPS);

  for (int i = 0; i < MAXLOOPS; i++) {
    (*nextParams)[i] = new vector<int>(NUMPARAMS);
  }

  
  /* call LoopTool until we find a different fusion config */ 

  bool foundNewConfig = false; 
  unsigned int numEntries;
  IncTolerance(dim);
  for ( ; !ReachedThreshold(dim); ) {
    if (dim == 2 && (*tolerance)[dim] < 0.20) 
      cacheLevel = (cacheLevel == 1 ? cacheLevel + 1 : cacheLevel);
    DumpTuningParams(1);

    if (!RunShScript(fusionscript, args)) {
      cerr << "Error running script: " << fusionscript << endl;
      exit(1);
    }
    int lastFused = fusedLoops;
    numEntries = ReadGenParamsFromFile(nextParams);
    //    foundNewConfig = fusedLoops > lastFused;
    foundNewConfig = ((fusedLoops > lastFused) || IsDifferent(nextParams, allParams));
    if (foundNewConfig) 
      break;
    IncTolerance(dim);
  }

  if (ReachedThreshold(dim))
    DecTolerance(dim);

  if (foundNewConfig) {
    for (unsigned int i = 0; i < numEntries; i++) 
      for (unsigned int j = 0; j < ((*nextParams)[i])->size(); j++)
	(*((*allParams)[i]))[j] = (*((*nextParams)[i]))[j];
  }

  return foundNewConfig;
  //  free(nextParams);
}

bool FusionSearch::IsDifferent(vector <loopParams *> *p1, 
			       vector <loopParams *> *p2) {
  if (p2->size() != p1->size()) 
    return true;
  for (unsigned int i = 0; i < p2->size(); i++) 
    for (unsigned int j = 0; j < ((*p2)[i])->size(); j++)
      if ((*((*p1)[i]))[j] != (*((*p2)[i]))[j]) {
	return true;
      }
  return false;

}

void FusionSearch::IncTolerance(int i) {
  (*tolerance)[i] += (*decrement)[i];
}

void FusionSearch::DecTolerance(int i) {
  (*tolerance)[i] -= (*decrement)[i];
}

void FusionSearch::DumpTuningParams(int dumpLoc) {
  if (!dumpLoc) {
    cout << numParams << endl;
    cout << dim << " " << cacheLevel << endl;
    for (int i = 0; i < numParams; i++) {
      cout << (*tolerance)[i] << " ";
      cout << (*decrement)[i] << " ";
      cout << (*threshold)[i] << endl;
    }
    
  }
  else {

    ofstream tuneFile(nextParamsFileName.c_str(), ios::out);
    tuneFile << numParams << endl;
    tuneFile << dim << " " << cacheLevel << endl;
    for (int i = 0; i < numParams; i++) {
      tuneFile << (*tolerance)[i] << endl;
    }
    tuneFile.close();
  }
}

void FusionSearch::DumpRunInfo(int evals, float* performance) {

  ofstream logfile(logFileName.c_str(), ios::app);
  
  if (!evals) {
    logfile << "Eval\tConstraint\tFused\tCycles\t\tLoads" << endl;
    logfile << evals << "\t";
    for (int i = 0; i < numParams; i++)
      logfile << "0.0" << " "; 
      logfile << "\t" << fusedLoops 
	      << "\t" << performance[0] 
	      << "\t" << performance[1] << endl;
  }
  else {
    logfile << evals << "\t";
    for (int i = 0; i < numParams; i++)
      logfile << (*tolerance)[i] << " "; 
    logfile << "\t" << fusedLoops 
	    << "\t" << performance[0] << "\t" << performance[1] << endl;
  }
  logfile.close();
}

void FusionSearch::Dump(int entries) {
  cout << "Dumping Tolerance Values" << endl;
  DumpTuningParams(0);
  cout << "Dumping Parameter Configuration " << endl;
  for (int i = 0; i < entries; i++) {
    for (unsigned int j = 0; j < ((*allParams)[i])->size(); j++)
      cout << (*((*allParams)[i]))[j] << " "; 
    cout << endl;
  }
  
}

bool WithinThreshold(float a1, float a2) {
  if (a1 >= a2) 
    return (a1/a2 < 1.01);
  else 
    return true;
}

float FusionSearch::Start() {

  int argnum = 7;
  char **args;

  /* generate variant with no constraints */
  args = (char **) malloc(sizeof(char *) * argnum);
  for (int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);

  args[0] = (char *) fusionscript.c_str();
  args[1] = (char *) prog.c_str();
  args[2] = (char *) path.c_str();
  args[3] = (char *) "gen";
  args[argnum - 1] = NULL; 

  if (!RunShScript(fusionscript, args)) {
    cerr << "Error running script: " << fusionscript << endl;
    exit(1);
  }

  /* evaluate variant with no constraint */
  args[0] = (char *) evalscript.c_str();
  args[3] = (char *) "gen";
  args[4] = (char *) "CYCLES";
  args[5] = (char *) "LOADS";
  args[argnum - 1] = NULL; 
  
  if (!RunShScript(evalscript, args)) {
    cerr << "Error running script: " << evalscript << endl;
    exit(1);
  }
  
  float *performance = (float *) malloc(sizeof(float) * NUMMETRICS);
  string filename = path + "/" + prog + ".perf";
  float genVal = GetPerfData(filename, GetTruncFactor(), performance);

  ReadGenParamsFromFile(allParams);
  int evals = 0;

  DumpRunInfo(evals, performance);
  /* generate variant with stricted constraint */
  DumpTuningParams(1);
  args[0] = (char *) fusionscript.c_str();
  args[1] = (char *) prog.c_str();
  args[2] = (char *) path.c_str();
  args[3] = (char *) "gen";
  args[argnum - 1] = NULL; 


  if (!RunShScript(fusionscript, args)) {
    cerr << "Error running script: " << fusionscript << endl;
    exit(1);
  }

  /* evaluate variant with strictest constraint */
  args[0] = (char *) evalscript.c_str();
  args[3] = (char *) "gen";
  args[4] = (char *) "CYCLES";
  args[5] = (char *) "LOADS";
  args[argnum - 1] = NULL; 
  
  if (!RunShScript(evalscript, args)) {
    cerr << "Error running script: " << evalscript << endl;
    exit(1);
  }
  
  genVal = GetPerfData(filename, GetTruncFactor(), performance);
  ReadGenParamsFromFile(allParams);
  evals++;

  DumpRunInfo(evals, performance);


  float bestSoFar = genVal;
  float curVal = bestSoFar;
  /* as long as we keep finding better performance than the current best 
     we keep relaxing the constraints (i.e. decrease tolerance) 

     search is orthogonal and sequential
  */

  maximal = false;
  for (; dim < numParams; dim++) {

    fprintf(stdout, "Tuning parameter %d\n", dim); 

    float refTolerance;
    while (WithinThreshold(curVal, bestSoFar)) {
      bestSoFar = (bestSoFar < curVal ? bestSoFar : curVal);
      refTolerance = (*tolerance)[dim];
      if (!GenNext()) 
	break;
      if (!RunShScript(evalscript, args)) {
	cerr << "Error running script: " << evalscript << endl;
	exit(1);
      }
      curVal = GetPerfData(filename, GetTruncFactor(), performance);
      evals++;
      DumpRunInfo(evals, performance);
    }
    /* we still want to tune the next parameter */
    curVal = bestSoFar;
    (*tolerance)[dim] = refTolerance;
    fusedLoops = 0;
  }
  return bestSoFar;
}






