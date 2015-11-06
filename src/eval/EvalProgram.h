#ifndef EvalProgram_h
#define EvalProgram_h

#define CORE_ARGS 4
#define NULL_ARG 1
#define EVAL_SCRIPT_OPTIONS 3

#include<vector>
#include<string>

#include<driver/Args.h>
#include<eval/PerfMetric.h>

float EvalProg(const Args& env, vector<int> *params, Metrics *metrics);
float EvalProg(const Args& env, vector<int> *params);

float EvalProgWithMetric(const Args& env, vector<int> *params, enum metricType metric);

float GetPerfData(string filename, double truncFactor, float *metrics, enum metricType metric);
float GetPerfData(string filename, double truncFactor, float *metrics);
float GetAllPerfData(string filename, double truncFactor, Metrics *metrics);

int RunShScript(string scriptName, char **args);

#endif
