#include<vector>

//void logSingleEval(int dims, vector<int> *params, float val, const char mode, string filename);
void logSingleEval(int dims, int *params, float val, const char mode, string filename);
void logPeriodic(int period, float val, string filename);
void logResultPerEval(int period, float val, string prog, string filename);
void logFinalResult(float val, string filename);

void LinkerHack();
