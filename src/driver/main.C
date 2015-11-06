#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<stdlib.h>

using namespace std; 

#include<ml/IID.h>
#include<search/DirectSearch.h>
#include<search/RandomSearch.h>
#include<search/Exhaustive.h>
#include<search/Anneal.h>
#include<search/FusionSearch.h>
#include<eval/EvalProgram.h>
#include<eval/PerfMetric.h>
#include<eval/Diagnostics.h>
#include<driver/Args.h>
#include<driver/SearchLog.h>
#include<interface/POETSearchSpec.h>

extern string cleanupscript;

void CleanUp(string prog, string srcdir) {
  
  unsigned int argnum = 4;
  char **args;
  args = (char **) malloc(sizeof(char *) * argnum);
  for (unsigned int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);


  args[0] = (char *) cleanupscript.c_str();
  args[1] = (char *) prog.c_str();
  args[2] = (char *) srcdir.c_str();
  args[argnum - 1] = NULL; 
  
  if (!RunShScript(cleanupscript, args)) {
    cerr << "Error running script: " << cleanupscript << endl;
    exit(1);
  }
}

int main(int argc, char* argv[]) {

  Args args(argc, argv);
  float bestVal;

  if (args.mode == "POET") {
    BuildConfFileFromPOETScript(args);
  }
  
  if (args.search == "iid") {
    IID search  = IID(args);
    bestVal = search.Start();
  }
  if (args.search == "random") {
    RandomSearch search  = RandomSearch(args);
    bestVal = search.Start();
  }
  if (args.search == "anneal") {
    Anneal search = Anneal(args);
    bestVal = search.Start();
  }
  if (args.search == "exhaust") {
    Exhaustive search = Exhaustive(args);
    bestVal = search.Start();
  }
  if (args.search == "direct") {
    DirectSearch search = DirectSearch(args);
    bestVal = search.Start();
  }
#ifdef LOG    
  if (!args.logfileName.empty())
    logFinalResult(bestVal, args.logfileName);
  else {
    cout << fixed << setiosflags(ios::right);
    cout.precision(2);
    cout << "Best value:\t" << bestVal << endl; 
  }
#endif
  
//     if (args.search == "random") {
//       RandomSearch search  = RandomSearch(args);
//       bestVal = search.Start();
//     }
//     if (args.search == "anneal") {
//       Anneal search = Anneal(args);
//       bestVal = search.Start();
//     }
//     if (!args.logfileName.empty())
//       logFinalResult(bestVal, args.logfileName);
//   }
//   else {
//     if (args.search == "random") {
//       RandomSearch search  = RandomSearch(args);
//       bestVal = search.Start();
//     }
//     if (args.search == "anneal") {
//       Anneal search = Anneal(args);
//       bestVal = search.Start();
//     }
//   }


 //  if (args.mode == "online") {
//     if (args.search == "direct") { 
//       DirectSearch search = DirectSearch(args.prog, args.srcdir, args.configFile, args.mode);
//       float bestVal = search.Start();
// #ifdef LOG
//       logFinalResult(bestVal);
// #endif
// //       float baseVal = RunDiagnostics(&ds);
// //       string logfilename = ds.GetPath() + ds.GetProgName() + ".log";
// //       DumpParameters(ds, logfilename);
// //       ofstream logfile(logfilename.c_str(), ios::app);
// //       logfile.precision(0);
// //       logfile << "Baseline execution time: " << baseVal << endl;
      
// //       float bestVal = ds.Start();
// //       logfile.precision(3);
// //       logfile << "Speedup obtained " << baseVal/bestVal << endl;
//     }
//     else if (args.search == "random") {
//       RandomSearch search  = RandomSearch(args.prog, args.srcdir, args.configFile, args.mode);
//       float bestVal = search.Start();
//       cout << logfileName;

//       logFinalResult(bestVal);
// #ifdef LOG
// #endif

//       // float baseVal = RunDiagnostics(&ds);
//       // logfile << "Baseline execution time: " << (unsigned long) baseVal << endl;
//     }
//     else if (args.search == "anneal") {
//       Anneal search = Anneal(args.prog, args.srcdir, args.configFile, args.mode);
//       float bestVal = search.Start();
// #ifdef LOG
//       logFinalResult(bestVal);
// #endif
//     }
//     else if (args.search == "exhaust") {
//       Exhaustive search = Exhaustive(args.prog, args.srcdir, args.configFile, args.mode);
//       float bestVal = search.Start();
// #ifdef LOG
//       logFinalResult(bestVal);
// #endif

//     }
    

// //     else if (args.search == "concurrent") {
// //       DSDriver ds = 
// //       DSDriver(args.prog, args.srcFile, args.srcdir, args.configFile);
// //       string logfilename = args.srcdir + "/" + args.prog + ".log";
// //       ofstream logfile(logfilename.c_str(), ios::app);
// //       float baseVal = RunDiagnostics(args.srcdir, args.prog, args.srcFile);
// //       float bestVal = ds.Start();
// //       logfile << "Best program execution: " << bestVal << endl;
// //       logfile << "Total number of evaluations " << ds.GetEvals() << endl;
// //       logfile.precision(3);
// //       logfile << "Speedup obtained " << baseVal/bestVal << endl;
// //       logfile.close();
// //     }
// //     else if (args.search == "sequential") {
// //       FusionSearch fs = FusionSearch(args.prog, args.srcdir);
// //       float baseVal = RunDiagnostics(&fs);
// //       float bestVal = fs.Start();
      
// //       string logfilename = args.srcdir + "/" + args.prog + ".log";
// //       ofstream logfile(logfilename.c_str(), ios::app);
// //       logfile << "Baseline execution time: " << baseVal << endl;
// //       logfile.precision(3);
// //       logfile << "Speedup obtained " << baseVal/bestVal << endl;
// //       logfile.close();
// //     }
// //     //    CleanUp(args.prog, args.srcdir);
//   }
//   else if (args.mode == "offline") {
//     //    cout << "Searching in offline mode" << endl;
// //     if (args.search == "direct") { 
// //       DirectSearch ds = DirectSearch(args.prog, args.srcdir, args.configFile, 
// // 				     args.mode);
// //       float bestVal = ds.Start();
// //       //      cout << "Best Performance " << bestVal << endl;
// //       printf("%2.2f\n", bestVal);
// //     }
// //     else if (args.search == "random") {
// //       RandomSearch ds = RandomSearch(args.prog, args.srcdir, args.configFile, 
// // 				     args.mode);
// //       float bestVal = ds.Start(1);
// //       cout << "Best Performance " << bestVal << endl;
// //     }

//   }

// Linker HACK
#if 1
  LinkerHack();
  LinkerHack2();
#endif
  
  return 0;
}  
  
  





