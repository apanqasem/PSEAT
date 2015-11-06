#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <search/nSpace.h>
#include <search/Assert.h>

using namespace std;

template<class T>
nSpace<T>::nSpace() {
  numDims = 0;
}

template<class T>
nSpace<T>::nSpace(int _numDims) {
  nSpace(_numDims, DEFAULT_DIM_SIZE);
}

template<class T>
nSpace<T>::nSpace(int _numDims, ...) {
  numDims = _numDims;

  va_list(indices);
  va_start(indices, _numDims);
  dimSizes = (int *) (malloc(sizeof(int) * numDims));
  maxValues = 1;
  for (int i = 0; i < numDims; i++) {
    dimSizes[i] = va_arg(indices, int); 
    maxValues *= dimSizes[i]; 
  }
  va_end(indices);

  dims = (int **) (malloc(sizeof(int *) * numDims));
  for (int i = 0; i < numDims; i++) {
    dims[i] = (int *) (malloc(sizeof(int) * dimSizes[i]));
  }

  values = (T *) (malloc(sizeof(T) * maxValues));
  flags = (bool *) (malloc(sizeof(bool) * maxValues));
}

template<class T>
nSpace<T>::nSpace(int _numDims, int _dimSize) {
  numDims = _numDims;
  dims = (int **) (malloc(sizeof(int *) * numDims));
  for (int i = 0; i < numDims; i++) {
    dims[i] = (int *) (malloc(sizeof(int) * _dimSize));
  }
  
  dimSizes = (int *) (malloc(sizeof(int) * numDims));
  maxValues = 1;
  for (int i = 0; i < numDims; i++) {
    dimSizes[i] = _dimSize;
    maxValues *= dimSizes[i]; 
  }
  values = (T *) (malloc(sizeof(T) * maxValues));
  flags = (bool *) (malloc(sizeof(bool) * maxValues));
}

template <class T>
nSpace<T>::nSpace(int _numDims, int *_dimSizes) {
  numDims = _numDims;
  
  dims = (int **) (malloc(sizeof(int *) * numDims));
  dimSizes = (int *) (malloc(sizeof(int) * numDims));
  for (int i = 0; i < numDims; i++) {
    dims[i] = (int *) (malloc(sizeof(int) * _dimSizes[i]));
  }

  maxValues = 1;
  for (int i = 0; i < numDims; i++) {
    dimSizes[i] = _dimSizes[i];
    maxValues *= dimSizes[i]; 
  }
  values = (T *) (malloc(sizeof(T) * maxValues));
  flags = (bool *) (malloc(sizeof(bool) * maxValues));
}

template <class T>
void nSpace<T>::InitSpace(int _numDims, int *_dimSizes) {
  numDims = _numDims;
  
  dims = (int **) (malloc(sizeof(int *) * numDims));
  dimSizes = (int *) (malloc(sizeof(int) * numDims));
  for (int i = 0; i < numDims; i++) {
    dims[i] = (int *) (malloc(sizeof(int) * _dimSizes[i]));
  }

  maxValues = 1;
  for (int i = 0; i < numDims; i++) {
    dimSizes[i] = _dimSizes[i];
    maxValues *= dimSizes[i]; 
  }
  values = (T *) (malloc(sizeof(T) * maxValues));
  flags = (bool *) (malloc(sizeof(bool) * maxValues));
}

template<class T>
nSpace<T>::~nSpace() {
}

template<class T>
void nSpace<T>::SetValue(T val, int _numDims, ...) {

  va_list indices;
  va_start(indices, _numDims);
  
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    index += va_arg(indices, int) * offset; 
  }
  va_end(indices);
  values[index] = val;
}

template <class T>
void nSpace<T>::SetValue(T val, int *indices) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    index += indices[i] * offset; 
  }
  values[index] = val;
  flags[index] = true;
}

template <class T>
void nSpace<T>::SetValue(T val, Point p) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    index += p[i] * offset; 
  }
  values[index] = val;
  flags[index] = true;
}

template <class T>
void nSpace<T>::Visit(int *indices) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    index += indices[i] * offset; 
  }
  flags[index] = true;
}

template <class T>
void nSpace<T>::ResetVisit() {
  for (int i = 0; i < maxValues; i++)
    flags[i] = false;
}

template <class T>
T& nSpace<T>::GetValue(int *indices) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    if (!((indices[i] >= 0) && (indices[i] < dimSizes[i]))) { 
	fprintf(stderr, 
		"GetValue %d Array index out of bounds for dim %d\n", 
		indices[i], i);
	exit(-1);
    }
    /*    Assert((indices[i] >= 0) && (indices[i] < dimSizes[i]), 
	  "Array index out of bounds"); */
    index += indices[i] * offset; 
  }
  return values[index];
}

template<class T>
T& nSpace<T>::GetValue(int _numDims, ...) {
  
  va_list indices;
  va_start(indices, _numDims);

  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    int curIndex = va_arg(indices, int);
    if (!((indices[i] >= 0) && (indices[i] < dimSizes[i]))) { 
	fprintf(stderr, "%d  Array index out of bounds\n", indices[i]);
	exit(-1);
    }
    /*    Assert((curIndex >= 0) && (curIndex < dimSizes[i]), 
	  "Array index out of bounds"); */
    index += curIndex * offset;
  }
  va_end(indices);
  return values[index];
}

template<class T>
T& nSpace<T>::GetValue(Point p) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    if (!((p[i] >= 0) && (p[i] < dimSizes[i]))) { 
	fprintf(stderr, 
		"GetValue %d Array index out of bounds for dim %d\n", 
		p[i], i);
	exit(-1);
    }
    index += p[i] * offset; 
  }
  return values[index];
}

template<class T>
T & nSpace<T>::Min(int *indices) {
  T min = UNDEF_VAL;
  for (int i = 0; i < maxValues; i++) {
    if (values[i] < min) {
      min = values[i];
      GetIndex(i, indices);
    }
  }
  return GetValue(indices);
} 

template<class T>
T & nSpace<T>::Max(int *indices) {
  T max = 0;
  for (int i = 0; i < maxValues; i++) {
    if (values[i] > max && values[i] != UNDEF_VAL) {
      max = values[i];
      GetIndex(i, indices);
    }
  }
  return GetValue(indices);
}

template<class T> 
void nSpace<T>::GetIndex(int cumIndex, int *indices) {
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    indices[i] = (cumIndex / offset) % dimSizes[i];
  }
}

template<class T>
void nSpace<T>::InitValues(T val) {
  for (int i = 0; i < maxValues; i++) {
    values[i] = val;
    flags[i] = false;
  }
}

template<class T>
bool nSpace<T>::Visited(int *indices) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    if (!((indices[i] >= 0) && (indices[i] < dimSizes[i]))) { 
	fprintf(stderr, 
		"Visited: %d Array index out of bounds for dim %d", 
		indices[i], i);
	exit(-1);
    }
    index += indices[i] * offset; 
  }
  return flags[index];
}

template<class T>
bool nSpace<T>::Visited(Point p) {
  int index = 0;
  int offset = maxValues;
  for (int i = 0; i < numDims; i++) {
    offset /= dimSizes[i];
    if (!((p[i] >= 0) && (p[i] < dimSizes[i]))) { 
	fprintf(stderr, 
		"Visited: %d Array index out of bounds for dim %d", 
		p[i], i);
	exit(-1);
    }
    index += p[i] * offset; 
  }
  return flags[index];
  
}
