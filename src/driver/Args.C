#include<iostream>
#include<getopt.h>
#include<dirent.h> 
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

using namespace std; 

#include <driver/Args.h>

//const char * Args::EVALSCRIPT = "comp-run-measure-seq.sh";
const char * Args::EVALSCRIPT = "build_run_profile.sh";
const char * Args::FUSIONSCRIPT = "fusegen.sh";
const char * Args::CLEANUPSCRIPT = "cleanup.sh";
const char * Args::POETINTERFACESCRIPT = "build_conf_from_pt.sh";
const char * Args::POETINVOCSCRIPT="poet_invoke.sh";

string cleanupscript;

Args::Args(int argc, char* const* argv) {

  if (argc < 1) {
    Usage();
    exit(1);
  }

  prog = "test";
  testdir = ".";
  search = "random";
  mode = "DEFAULT";

  static struct option longopts[] = {
    { "custom", required_argument, NULL, 'c'},
    { "dumplog", required_argument, NULL, 'd'},
    { "help", no_argument, NULL, 'h'},
    { "mode", required_argument, NULL, 'm'},
    { "program", required_argument, NULL, 'p'},
    { "testdir", required_argument, NULL, 't'},
    { "search", required_argument, NULL, 's'},
    { "verbose", no_argument, NULL, 'V'},
    { NULL, 0, NULL, 0} 
  };

  int ch;
  while ((ch = getopt_long(argc, argv, "c:m:p:t:s:V", longopts, NULL)) != EOF) {
    switch (ch) {
    case 'c':
      customFile = optarg;
      break;
    case 'd':
      logfileName = optarg;
      break;
    case 'h':
      Usage();
      exit(0);
    case 'm':
      int i;

      char *substr;

      substr = strchr(optarg, ':');

      /* check if path has been specified. two cases  -m TOOL and -m TOOL: */
      if (!substr || (*(substr + 1) == 0)) {
	cerr << "No HOME location specified for " << optarg << endl;
	exit(1);    
      } 

      /* skip ':' and assign second half of string to path */
      toolHome = substr + 1;
      for (i = 0; i < strlen(optarg); i++)  {
	if (optarg[i] == ':')
	  break;
      }

      /* copy first half to mode */
      substr = (char *) malloc(sizeof(char) * (i + 1));
      strncpy(substr, optarg, i);
      substr[i] = '\0';
      mode = substr;

      free(substr);
      break;
    case 'V':
      Usage();
      exit(0);
    case 'p': 
      prog = optarg;
      break; 
    case 't':
      /* trim trailing slash */
      if (optarg[strlen(optarg) - 1] == '/') 
	optarg[strlen(optarg) - 1] = '\0';
      testdir = optarg;
      break; 
    case 's':
      search = optarg;
      if (search == "direct" || 
	  search == "iid" || 
	  search == "random" || 
	  search == "anneal" || 
	  search == "exhaust" || 
	  search == "sequential") {
       	EVALSCRIPT = "build_run_profile.sh";
	FUSIONSCRIPT = "fusegen.sh";
      }
      else if (search == "concurrent") 
	EVALSCRIPT = "build_run_profile.sh";
      else { 
	cerr << "Unkown search technique: " <<  search << endl;
	cerr << "Using default random search" << endl;
	search = "random";
	EVALSCRIPT = "build_run_profile.sh";
      }
      break;
    default:
      Usage(); 
      exit(1); 
    }
  }

  if (optind == argc - 1) {
    configFile = argv[optind];
  }
  else {
    cout << "No configuration file" << endl;
    Usage();
    exit(1); 
  }

  if (!logfileName.empty())
    logfileName = testdir + "/" + logfileName;
  if (!customFile.empty())
    customFile = testdir + "/" + customFile;

  if (mode == "POET") {
    poetinvocscript = testdir + "/" + POETINVOCSCRIPT;
    poetConfigFile = configFile;
  }
  
  CheckScripts(argv);
}

void Args::CheckScripts(char* const* argv) {



  char *exepath = (char *) malloc(MAXPATHLENGTH * sizeof(char));
  if (strlen(argv[0]) >= MAXPATHLENGTH) {
    cout << "ERROR: path name too long : " << argv[0] << endl;
    exit(1);
  }
    
  strcpy(exepath, argv[0]);
  int i;
  for (i = strlen(exepath) - 1; i >= 0; i--)  {
    if (exepath[i] == '/') 
      break;
  }

  /* didn't find a '/', implies : executable is in path or executable is in cwd (csh) */
  if (i < 0) {
    /* asumme scripts are also in the path */
    evalscript = EVALSCRIPT;
    poetinterfacescript = POETINTERFACESCRIPT;
    fusionscript = FUSIONSCRIPT;
  }
  /* executable invoked with full path name 
     make sure the scripts are in the same directory as the executable */
  else {
    exepath[i] = '\0';

    /* check eval script and set path and name */
    evalscript = strcat(strcat(exepath, "/"), EVALSCRIPT);
    FILE *shScript = fopen(evalscript.c_str(), "r");
    if (!shScript) {
      cerr << "Error: Script file " << evalscript << " not found"
	   << endl;
      exit(1);    
    }
    fclose(shScript);

    exepath[i] = '\0';

#if 0
    /* check poet script and set path and name */
    poetinterfacescript = strcat(strcat(exepath, "/"), POETINTERFACESCRIPT);
    shScript = fopen(poetinterfacescript.c_str(), "r");
    if (!shScript) {
      cerr << "Error: Script file " << poetinterfacescript << " not found"
	   << endl;
      exit(1);    
    }
    fclose(shScript);



    /* check fusion script and set path and name */
    fusionscript = strcat(strcat(exepath, "/"), FUSIONSCRIPT);
    shScript = fopen(fusionscript.c_str(), "r");
    if (!shScript) {
      cerr << "Error: Script file " << FUSIONSCRIPT << " not found"
	   << endl;
      exit(1);    
    }
    fclose(shScript);
#endif
  }
  free(exepath);  
}


void Args::Usage() {
  cerr << "Usage:" << endl
       << "      pseat [options] configfile" << endl
       << "Options:" << endl
       << "  -c <file>          use <file> to customize search algorithm" << endl                
       << " --help              display this information" << endl
       << " --dumplog <file>    dump search information into <file>" << endl
       << "  -m <mode>          operate in <mode> mode" << endl
       << "                     valid options for <mode> are DEFAULT, LOOPTOOL and POET" << endl
       << "  -p <prog>          use <prog> as prefix for generated files, default is test" << endl
       << "  -s <search>        apply <search> algorithm, default is random search" << endl   
       << "  -t <dir>           use <dir> as test directory, default is current dir" << endl
       << "  -V                 display version information" << endl
       << endl;
}

void Args::Dump() {
  cout << "\t\t******   Search Environment ****** " << endl;
  cout << "Search algorithm:    \t" << search << endl;

  cout << "Target program:      \t" << prog << endl;
  cout << "Test directory:      \t" << testdir << endl;
  cout << "Mode:                \t" << mode << endl;
  cout << "Evaluation script:   \t" << evalscript << endl;
  cout << "Transformation tool:   \t" << mode << endl;  
  cout << "POET interface script:   \t" << poetinterfacescript << endl;
  cout << "Configuration file:  \t" << configFile << endl;
  cout << "Customization file:  \t" << (customFile.empty() ? "none" : customFile) << endl;
  cout << "Log file:            \t" << (logfileName.empty() ? "none" : logfileName) << endl;
}


