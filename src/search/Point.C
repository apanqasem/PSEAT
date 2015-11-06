#include <stdlib.h>
#include <Point.h>

using namespace std;
#include <iostream>

Point::Point() {
  coords = (int *) malloc(sizeof(int));
  dims = 1;
}

Point::Point(int _dims) {
  coords = (int *) malloc(_dims * sizeof(int));
  if (!coords) {
    cerr << "Error: Unable to allocate memory for Point object." << endl;
  }
  dims = _dims;
}

Point::~Point() {
}


void Point::Inc(int dim, int factor) {
  coords[dim] = coords[dim] + factor;
}

void Point::Dec(int dim, int factor) {
  coords[dim] = coords[dim] - factor;
}

void Point::Reset(int dim, bool inc, int factor) {
  if (inc)
    coords[dim] = coords[dim] + factor;
  else
    coords[dim] = coords[dim] - factor;
}

bool Point::operator=(Point p) {
  if (dims != p.GetDims()) {
    return false;
  }
  for (int i = 0; i < dims; i++) {
    coords[i] = p.GetVal(i);
  }
  return true;
}

#if 0
bool Point::operator=(Point p1, Point p2) {
  if (p1.GetDims != p2.GetDims()) {
    return false;
  }
  for (int i = 0; i < p1.GetDims(); i++) {
    p1.SetVal(i) = p2.GetVal(i);
  }
  return true;
}
#endif

int Point::operator[](int i) {
  if (i >= dims) {
    cerr << "Error: Index " << i << " out of bounds for " << dims 
	 << "-dimensional space" << endl;
    exit(1);
  }
  return coords[i];
}
