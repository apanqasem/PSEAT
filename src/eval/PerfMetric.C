#include<stdlib.h>
#include<PerfMetric.h>

using namespace std; 

void LinkerHack2() {}

Metrics::Metrics() {
  data = (float *) (malloc(sizeof(float) * NUMMETRICS));
}

bool Metrics::IsLessThan(Metrics other, char mode) {
  if (mode == 's') 
    return (data[0] < other.GetMetric(0));
  else if (mode == 'c') {
    if (data[0] < other.GetMetric(0) &&
	data[1] < other.GetMetric(1))
      return true;
    else 
      return false;
  }
} 
