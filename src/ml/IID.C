#include<iomanip>
#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
#include<sys/time.h>
#include<stdlib.h>

using namespace std;

#include<eval/EvalProgram.h>
#include<ml/IID.h>
#include<driver/SearchLog.h>

// svn test

IID::IID(const Args& specs) 
 : ML(specs) {
}

IID::~IID() {}

float IID::findMaxVal() {
  float max = sdata[0].val;
  for (int i = 0; i < sdata.size(); i++) {
    if (sdata[i].val > max) 
      max = sdata[i].val;
  }
  return max;
}

float IID::findMinVal() {
  float min = sdata[0].val;
  for (int i = 0; i < sdata.size(); i++) {
    if (sdata[i].val < min) 
      min = sdata[i].val;
  }
  return min;
}


float IID::Start(float baseVal) {

  float bestVal = findMaxVal();
  float worstVal = findMinVal();
  float curVal;

  vector <float> proximity;
  vector <float> proximityToWorst;
  for (int i = 0; i < sdata.size(); i++) {
    curVal = sdata[i].val;
    proximity.push_back(100 - ((bestVal - curVal)/bestVal * 100));
  }


  for (int i = 0; i < sdata.size(); i++) {
    curVal = sdata[i].val;
    proximityToWorst.push_back(100 - ((curVal - worstVal)/worstVal * 100));
  }


#if 1
  for (int i = 0; i < proximity.size(); i++) {
    cout << fixed << setiosflags(ios::right);
    cout.precision(2);
    cout << "proximity:\t" << proximity[i] << "%" << endl; 
  }  
#endif 

  int *optrank =  (int *) malloc(sizeof(int) * space->GetTotalParams());
  float *optprob =  (float *) malloc(sizeof(float) * space->GetTotalParams());

  int *optRevRank =  (int *) malloc(sizeof(int) * space->GetTotalParams());
  float *optRevProb =  (float *) malloc(sizeof(float) * space->GetTotalParams());

  for (int j = 0; j < space->GetTotalParams(); j++) {
    optrank[j] = 0;
    optRevRank[j] = 0;
  }

  for (int i = 0; i < proximity.size(); i++) {
    if (proximity[i] > BEST_PROX_THRESHOLD) {
      for (int j = 0; j < space->GetTotalParams(); j++) {
	if ((sdata[i]).params[j]) 
	  optrank[j]++;
      }
    }
  }

  for (int i = 0; i < proximityToWorst.size(); i++) {
    if (proximityToWorst[i] > WORST_PROX_THRESHOLD) {
      for (int j = 0; j < space->GetTotalParams(); j++) {
	if ((sdata[i]).params[j]) 
	  optRevRank[j]++;
      }
    }
  }

#if 1
  for (int j = 0; j < space->GetTotalParams(); j++) {
    cout << optrank[j] << " " << optRevRank[j] << endl;
  }
#endif

  int sum = 0;
  int sumWorst = 0;
  for (int j = 0; j < space->GetTotalParams(); j++) {
    sum += optrank[j];
    sumWorst += optRevRank[j];
  }
  for (int j = 0; j < space->GetTotalParams(); j++) {
    optprob[j] = (float) optrank[j]/(float) sum;
    optRevProb[j] = REV_ADJUST * (float) optRevRank[j]/(float) sumWorst;
  }

#if 1
  for (int j = 0; j < space->GetTotalParams(); j++) 
    cout << optprob[j] << endl;
#endif 

  vector <float> seqprob;

  for (int i = 0; i< sdata.size(); i++) {
    float thisProb = 0;
    int checkforZeroSeq = 0;
    for (int j = 0; j < space->GetTotalParams(); j++) {
      if ((sdata[i]).params[j]) {
	thisProb += (optprob[j] - optRevProb[j]);
	checkforZeroSeq++;
      }
    }
    if (checkforZeroSeq)
      seqprob.push_back(thisProb);
    else 
      seqprob.push_back(0);
  }

#if 1
  for (int i = 0; i< seqprob.size(); i++) {
    cout << fixed << setiosflags(ios::right);
    cout.precision(12);
    cout << "Seq rank:\t" << seqprob[i] << endl;
  }
#endif


  double max = seqprob[0];
  int index = 0;
  for (int i = 0; i < seqprob.size(); i++) {
    if (seqprob[i] > max) {
      max = seqprob[i];
      index = i;
    }
  }

  cout << endl << " ***  Markov ML output *** " << endl << endl;
  cout << "Sequence no: " << index << endl;
  cout << "Optimization configuration: ";
  for (int j = 0; j < space->GetTotalParams(); j++) {
    cout << sdata[index].params[j]  << " ";
  }

  cout << endl;
  cout << "Performance: " << sdata[index].val << endl;
  cout << "Proximity to best value: ";
  cout.precision(4);
  cout << proximity[index] << "%" << endl;
  cout << endl << " ***  End Markov ML output *** " << endl << endl;

  return sdata[index].val;
}


void IID::ReadParams(string filename) {

  ifstream conf(filename.c_str(), ios::in);
  if (!conf) {
    cerr << "Could not open customization file " << filename << endl;
    exit(1);
  }
  conf.close();
}
