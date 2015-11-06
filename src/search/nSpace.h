#ifndef nSpace_h
#define nSpace_h

#define DEFAULT_DIM_SIZE 10
#define UNDEF_VAL 5000000

#include <search/Point.h>

template<class T>
class nSpace {
 public:
  nSpace();
  nSpace(int _numDims);
  nSpace(int _numDims, ...);
  nSpace(int _numDims, int *_dimSizes);
  nSpace(int _numDims, int dimSize);
  
  void InitSpace(int _numDims, int *_dimSizes);
  void SetValue(T val, int _numDims, ...);
  void SetValue(T val, int *indices);
  void SetValue(T val, Point p);
  
  int GetNumDims() {return numDims;};
  void GetIndex(int cumIndex, int *indices);
  T& GetValue(int _numDims, ...);
  T& GetValue(int *indices);
  T& GetValue(Point p);


  bool Visited(int *indices);
  bool Visited(Point p);

  void Visit(int *indices);
  void ResetVisit();

  T& Min(int *indices);
  T& Max(int *indices);

  void Sort();
  void InitValues(T val); 
  ~nSpace();

 private:
  int numDims;
  int *dimSizes; 
  int **dims;

  int maxValues;
  T *values;
  bool *flags;
};



#endif
