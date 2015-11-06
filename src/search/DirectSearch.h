#ifndef DirectSearch_h
#define DirectSearch_h

#include<search/SearchAlgo.h>

#define MIN_STEP_SIZE 1
#define MIN_REDUCE_FACTOR 1

class DirectSearch : public SearchAlgo {
  
 public:
  DirectSearch(const Args& specs);
  ~DirectSearch();



  int GetPartition(int i) {return partitions[i];};
  int GetStepSize(int i) {return stepSizes[i];};
  int GetReduceFactor(int i) {return reduceFactors[i];};


  float Start();
  float Search(int *params, int localmaxevals);
  float ExploreDim(int *curBp, int dim,	int localmaxevals, int stepSize);


#if 0
  DirectSearch(string _prog, string _path, string configfile);
  void ReadParams(string filename);
  float GetExecTime(int *params, int *evals);
  void  ReadData(char *filename);
  DirectSearch(string _prog, string _path, int numevals, int _unrollLoops, 
	       int _blockLoops, int *_bottom, int *_top, int *_partitions, 
	       int _constraints);
#endif

 private:

  int stepfactor;
  int reducefactor;

  int *partitions;
  
  int *stepSizes;
  int *reduceFactors;


  void Permute (int **params, int i, int j, int *result, 
		int **curParams, int *p);

  void GetCustomParams();
}; 
#endif



