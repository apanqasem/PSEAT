#include<SearchSpace_PSEAT.h>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<cstdlib>

using namespace std;

string PREDEF_SS_PATH = "../../tools/pseat/src/optflags/";

int factor(int n) {
  if (n == 0)     
    return 1;
  else
    return n * factor(n - 1);
}


SearchSpace_PSEAT::SearchSpace_PSEAT(string filename, string mode) {

  descent = true;
  online = true;
  constraints = false;

  if (mode == "DEFAULT" || mode == "RUNTIME" || mode == "LOOPTOOL") 
    ExtractSpaceFromDefault(filename);
  else if (mode == "POET") 
    ExtractSpaceFromPOET(filename);
}


void SearchSpace_PSEAT::Dump() {

  cout << endl;
  cout << endl;
  cout << " *** Search Space Description *** " << endl;
  cout << endl;
  cout << "mode: " << (online ? "online" : "offline") << endl; 
  cout << "direction: " << (descent ? "find min" : "find max") << endl;
  cout << "max evaluations: " << maxEvals << endl;
  cout << "evaluation metric: " << metric << endl;
  cout << "data file (for offline only): " << (datafile.empty() ? "none" : datafile) << endl;
  cout << "number of search dimensions: " << totalparams << endl;
  cout << "search dim description:" << endl; 
  for (int i = 0; i < totalparams; i++) {
    // cout << setw(20)  << left << name[i] << "\t" << right << setw(4) << dimTypes[i] 
    // 	 << " " << bottom[i] << ".." << top[i] 
    // 	 << " " << info[i] << " "  << defval[i] << endl;
    cout << setw(10)  << left << dimTypes[i] << "\t" << right << setw(4) 
	 << " " << bottom[i] << ".." << top[i] 
      	 << " " << info[i] << endl;
   }
  cout << endl;
  cout << " *** End Search Space Description *** " << endl;
  cout << endl;
}


void SearchSpace_PSEAT::ExtractSpaceFromDefault(string filename) {

  ifstream conf(filename.c_str(), ios::in);
  if (!conf) {
    cerr << "Could not open config file " << filename << endl;
    exit(1);
  }

  string mode;
  if (!(conf >> mode)) {
    cerr << "Error reading mode value from " <<  filename << endl;
    exit(1);
  }
  if (mode == "offline") {
    online = false;
    if (!(conf >> datafile)) {
      cerr << "Error reading mode value from " <<  filename << endl;
      exit(1);
    }
  }

  string target;
  if (!(conf >> target)) {
    cerr << "Error reading mode value from " <<  filename << endl;
    exit(1);
  }
  if (target == "MAX") 
    descent = false;

  string perfMetric;
  if (!(conf >> perfMetric)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }
  if (perfMetric == "MFLOPS") 
    metric = MFLOP;
  else if (perfMetric == "LOADS")
    metric = LD;
  else if (perfMetric == "L1")
    metric = L1;
  else if (perfMetric == "L2")
    metric = L2;
  else if (perfMetric == "TLB")
    metric = TLB;
  else
    metric = MFLOP;
  
  if (!(conf >> maxEvals)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }

  if (!(conf >> totalparams)) {
    cerr << "Error reading parameter: dimensions from " <<  filename << endl;
    exit(1);
  }
  
  bottom = (int *) malloc(sizeof(int) * totalparams);
  top = (int *) malloc(sizeof(int) * totalparams);
  info = (int *) malloc(sizeof(int) * totalparams);
  dimTypes = (char *) malloc(sizeof(char) * totalparams);
  enumList = (int **) malloc(sizeof(int *) * totalparams);
  
  int size;
  
  for (int i = 0; i < totalparams; i++) {
    if (!(conf >> dimTypes[i])) {
      cerr << "Error reading dimension type for dim" <<  i << "from" << filename << endl;
      exit(1);
    } 
    if (dimTypes[i] == 'N') {
      if (!(conf >> bottom[i] >> top[i] >> info[i])) {
	cerr << "Error reading parameter: search dimension " << i << "from " <<  filename << endl;
	exit(1);
      }
    }
    else if (dimTypes[i] == 'E') {
      conf >> size;
      enumList[i] = (int *) malloc(sizeof(int) * size);
      for (int j = 0; j < size; j++) {
	if (!(conf >> enumList[i][j]))  {
	  cerr << "Error reading parameter: enumerated list from " <<  filename << endl;
	  exit(1);
	}
      }
      bottom[i] = 0;
      top[i] = size - 1;
      info[i] = 0;
    }
    else if (dimTypes[i] == 'P') {
      conf >> size;
      bottom[i] = 0;
      top[i] = factor(size) - 1;
      info[i] = size;
    }
  }

  conf.close();
}


void SearchSpace_PSEAT::ExtractSpaceFromPOET(string filename) {

  ifstream conf(filename.c_str(), ios::in);
  if (!conf) {
    cerr << "Could not open config file " << filename << endl;
    exit(1);
  }

  string mode;
  if (!(conf >> mode)) {
    cerr << "Error reading mode value from " <<  filename << endl;
    exit(1);
  }
  if (mode == "offline") {
    online = false;
    if (!(conf >> datafile)) {
      cerr << "Error reading mode value from " <<  filename << endl;
      exit(1);
    }
  }

  string target;
  if (!(conf >> target)) {
    cerr << "Error reading mode value from " <<  filename << endl;
    exit(1);
  }
  if (target == "MAX") 
    descent = false;

  string perfMetric;
  if (!(conf >> perfMetric)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }
  if (perfMetric == "MFLOPS") 
    metric = MFLOP;
  else if (perfMetric == "LOADS")
    metric = LD;
  else if (perfMetric == "L1")
    metric = L1;
  else if (perfMetric == "L2")
    metric = L2;
  else if (perfMetric == "TLB")
    metric = TLB;
  else
    metric = MFLOP;
  
  if (!(conf >> maxEvals)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }

  
  if (!(conf >> totalparams)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }

  name = (char**) malloc(sizeof(char *) * totalparams);
  for (int i = 0; i < totalparams; i++)
    name[i] = (char *) malloc(sizeof(char *) * MAXTRANSFORMNAME);

  bottom = (int *) malloc(sizeof(int) * totalparams);
  top = (int *) malloc(sizeof(int) * totalparams);
  info = (int *) malloc(sizeof(int) * totalparams);
  dimRepr = (int *) malloc(sizeof(int) * totalparams);
  defval = (int *) malloc(sizeof(int) * totalparams);
  dimTypes = (char *) malloc(sizeof(char) * totalparams);
  enumList = (int **) malloc(sizeof(int *) * totalparams);
  

  int numlines;
  if (!(conf >> numlines)) {
    cerr << "Error reading parameters from " <<  filename << endl;
    exit(1);
  }

  int size;

  int foo;
  for (int i = 0, dimIndex = 0; i < numlines; i++, dimIndex++) {

    if (!(conf >> dimRepr[dimIndex])) {
      cerr << "Error reading dim representation value from " <<  filename << endl;
      exit(1);
    }      
    if (!(conf >> name[dimIndex])) {
      cerr << "Error reading parameter name from " <<  filename << endl;
      exit(1);
    }      

    if (!(conf >> dimTypes[dimIndex])) {
      cerr << "Error reading parameters from " <<  filename << endl;
      exit(1);
    } 
    if (dimTypes[dimIndex] == 'N') {
      if (!(conf >> bottom[dimIndex] >> top[dimIndex] >> info[dimIndex])) {
	cerr << "Error reading parameters from " <<  filename << endl;
	exit(1);
      }
      for (int k = 0; k < dimRepr[dimIndex]; k++) {  
	if (!(conf >> defval[dimIndex + k])) {
	  cerr << "Error reading parameters from " <<  filename << endl;
	  exit(1);
	}
      }    

    }
    else if (dimTypes[dimIndex] == 'E') {
      conf >> size;
      enumList[dimIndex] = (int *) malloc(sizeof(int) * size);
      for (int j = 0; j < size; j++) {
	if (!(conf >> enumList[dimIndex][j]))  {
	  cerr << "Error reading parameters from " <<  filename << endl;
	  exit(1);
	}
      }
      bottom[dimIndex] = 0;
      top[dimIndex] = size - 1;
      info[dimIndex] = 0;
      for (int k = 0; k < dimRepr[dimIndex]; k++) {  
	if (!(conf >> defval[dimIndex + k])) {
	  cerr << "Error reading parameters from " <<  filename << endl;
	  exit(1);
	}
      }    
    }
    else if (dimTypes[dimIndex] == 'P') {
      conf >> size;
      bottom[dimIndex] = 0;
      top[dimIndex] = factor(size) - 1;
      info[dimIndex] = size;
      for (int k = 0; k < dimRepr[dimIndex]; k++) {  
	if (!(conf >> defval[dimIndex + k])) {
	  cerr << "Error reading parameters from " <<  filename << endl;
	  exit(1);
	}
      }    
    }

    int currentDimRepr = dimRepr[dimIndex];
    for (int k = 0; k < currentDimRepr - 1; k++, dimIndex++) {
      name[dimIndex + 1] = name[dimIndex];
      dimTypes[dimIndex + 1] = dimTypes[dimIndex];
      bottom[dimIndex + 1] =  bottom[dimIndex];
      top[dimIndex + 1] = top[dimIndex];
      info[dimIndex + 1] = info[dimIndex];
      dimRepr[dimIndex + 1] = dimRepr[dimIndex];
    }

  }
  conf.close();
}


