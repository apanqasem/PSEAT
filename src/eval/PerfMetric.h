#ifndef PerfMetric_h
#define PerfMetric_h

#define NUMMETRICS 4
enum metricType {MFLOP, LD, L1, L2, TLB};

void LinkerHack2();

class Metrics {
 public:
  Metrics();
  ~Metrics();
  float GetMetric(int i) { return data[i]; }
  void SetMetric(int i, float val) { data[i] = val;}
  bool IsLessThan(Metrics other, char mode);

 private:
  float *data;
};

#endif
