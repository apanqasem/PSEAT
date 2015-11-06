#include<iostream>
#include<fstream>
#include<stdlib.h>

#include<interface/POETSearchSpec.h>
#include<eval/EvalProgram.h>



void BuildConfFileFromPOETScript(Args& env) {

  string prog = env.prog;
  string poetHome = env.toolHome;
  string poetconfig = env.configFile;
  string path = env.testdir;
  string poetinterfacescript = env.poetinterfacescript;

  char **args;
  int argnum = 5 + NULL_ARG + 2;

  args = (char **) malloc(sizeof(char *) * argnum);

  for (int i = 0; i < argnum; i++) 
    args[i] = (char *) malloc(sizeof(char) * MAXPATHLENGTH);

  args[0] = (char *) poetinterfacescript.c_str();
  args[1] = (char *) prog.c_str();
  args[2] = (char *) poetHome.c_str();
  args[3] = (char *) poetconfig.c_str();
  args[4] = (char *) path.c_str();

  args[argnum - 1] = NULL; 
 
#if DEBUG
  cerr << "Printing arguments to shell script" << endl;
  for (int i = 0; i < argnum - 1; i++) 
    cerr << "Arg " << i << ": " <<  args[i] << endl;
#endif

  if (!RunShScript(poetinterfacescript, args)) {
    cerr << "Error running script: " << poetinterfacescript << endl;
    exit(1);
  }

  env.configFile = env.prog + ".conf";

}


void GenPOETInvocScript(const Args& env, SearchSpace *space) {


  ofstream outfile;
  outfile.open(env.poetinvocscript.c_str());

  int argcount = 1;
  
  outfile << "#!/bin/sh" << endl << endl;
  
  outfile << "PROG=${" << argcount << "}" << endl;
  argcount++;
  outfile << "PATH=${" << argcount << "}" << endl;
  argcount++;
  outfile << "POETHOME=" << env.toolHome << endl << endl;
  argcount++;


  outfile << "poet=${POETHOME}/src/pcg" << endl;
  outfile << "poetLib=-L${POETHOME}/lib" << endl;
  outfile << "poetscript=${PATH}/${PROG}.c" << endl << endl;

  outfile << "inputFile=\"-pinputFile=${PATH}/${PROG}.c\"" << endl;
  outfile << "outputFile=\"-poutputFile=${PATH}/${PROG}.c.opt\"" << endl << endl;

  outfile << "2> /dev/null \\" << endl;
  outfile << "$poet $poetLib $inputFile $outputFile\\" << endl;

  for (int i = 0; i < space->GetTotalParams(); i++)  {
    outfile << " -p" << space->GetName(i) << "=\"";
    int paramVals = space->GetRepr(i);
    for (int j = 0; j < paramVals; j++) {
      outfile << "${" << argcount << "}";
	if (j < paramVals - 1) 
	  outfile << " ";
      argcount++;
    }
    outfile << "\"\\" << endl;

    for (int j = 0; j < paramVals - 1; j++)
      i++;
  }

  outfile << " " << env.poetConfigFile << " > ${PROG}.c.opt" << endl;
  outfile.close();

  
}
