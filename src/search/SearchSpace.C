#include<iostream>

using namespace std;

int sizes[3] = {3, 2, 3};

int GetIndexFromCoord(int dims, int *coords) {
  int dimsLeft = dims;
  int index = 0;
  for (int i = 0; i < dims; i++) {
    int pos = 1;
    for (int j = i + 1; j < dims; j++) 
      pos = pos * sizes[j];  
    index = index + coords[i] * pos;
  }
  return index;
}

int top[3] = {3, 3, 2};
int bottom[3] = {0, 0, 0};

int totalparams = 3;

int exhaust(int dim, int *params) {
  if (dim == totalparams - 1) {
    for (int i = bottom[dim]; i < top[dim]; i++) {
      params[dim] = i;
      for (int j = 0; j < totalparams; j++) 
	cout << params[j] << " ";
      cout << endl;
    }
    return 0;
  }

  for (int i = bottom[dim]; i < top[dim]; i++) {
    //    cout << i << " ";
    params[dim] = i;
    exhaust(dim + 1, params);
  }
}

int main() {
  int *params = (int *) malloc(sizeof(int) * totalparams); 

  exhaust(0, params);
  return 0;

}
