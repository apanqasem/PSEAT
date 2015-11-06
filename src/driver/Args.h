#ifndef Args_h
#define Args_h

#include <string>

#define MAXPATHLENGTH 350

class Args {
public: 
  Args();
  Args(int argc, char* const* argv); 
  
  void Usage();
  void Dump();
  void CheckScripts(char* const* argv);
  
  string prog; 
  string testdir; 
  string search;
  string mode;
  string toolHome;

  string logfileName;

  string configFile; 
  string poetConfigFile;
  string customFile;

  string evalscript;
  string fusionscript;
  string poetinterfacescript;
  string poetinvocscript;

  static const char * EVALSCRIPT;
  static const char * FUSIONSCRIPT;
  static const char * CLEANUPSCRIPT;
  static const char * POETINTERFACESCRIPT;
  static const char * POETINVOCSCRIPT;
}; 

#endif

