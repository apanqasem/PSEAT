#ifndef Point_h
#define Point_h

class Point {
 public:
  Point();
  Point(int dims);
  ~Point();
  void Inc(int dim, int factor);
  void Dec(int dim, int factor);
  void Reset(int dim, bool inc, int factor);
  
  void SetVal(int dim, int val) {coords[dim] = val;};
  int GetVal(int dim) {return coords[dim];};
  int GetDims() {return dims;};

  bool operator=(Point p);
  int operator[](int i);

 private:
  int *coords;
  int dims;
};


#endif
