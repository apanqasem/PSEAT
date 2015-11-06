/****
 * bit2string
 * by:
 *     Andrew Moriarty
 *     amm315@txstate.edu
 *
 *
 * Takes in a compiler name and a bitstring and outputs the appropriate compiler
 * optimization flags associated with the bitstring.
 *
 * If no bitstring if specified, a random bitstring is generated with a random
 * number of ones in random locations with a length of the number of total
 * op flags.
 *
 * Flags can also be specified for a given length of a bitstring. Currently
 * if there is a given bitstring of variable length matching numOPTS the
 * op flags are pulled
 * from "gen_b2f.h" from ENABLED_FLAGS.
 *
 * ex: ./bit2flag [options] -c <compiler> -b <bitstring>
****/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <cstdlib>
#include <getopt.h>
#include <gen_b2f.h>

/**
 * TO-DO: 2013-04-08
 *
 * */

using namespace std;

void print_help();
void print_options();
bool process_options(int argc, char** argv);
void process_bitstring();
string bit2flag();
unsigned int get_flags(vector<string> &op_flags);

// GLOBALS!!?!?!? BECAUSE I CAN
    static int num_flags      = numOPTS;// Number of total op flags for compiler
    static string bitstring   = "";     // the bitstring to represent flags
    static vector <string> all_op_flags;// all of the op flags for gcc or open64

int main(int argc, char*argv[]) {

    if( !process_options(argc, argv) )
        return 0;

    process_bitstring();
    string flags_str = bit2flag();

    cout << flags_str << endl;
    return 0;
}

// Prints out help menu
void print_help(){
    cout << "USAGE:" << endl;
    cout << "  bit2flag [options] -b bitstring" << endl;
    cout << "REQUIRED:" << endl;
    cout << "  -b bitstring     " << endl;
    cout << "OPTIONS:" << endl;
    cout << "  -p       prints out the optimizations in list." << endl;
    cout << "  -h       prints out this help menu." << endl;
    cout << "  -n       prints the number of optimizations." << endl;
}

// Processes the options
bool process_options(int argc, char** argv){

    static struct option long_options[] = {
        {"help",        no_argument,       0, 'h'},
        {"bitstring",   required_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    if( argc == 1 ){
      print_help();
      exit(EXIT_FAILURE);
    }

    while(char ch = getopt_long(argc, argv, "nphb:", long_options, NULL)){
        if(ch == -1) break;
        switch(ch){
            case 'b': // set bitstring
                bitstring = optarg;
                for(int i = 0; i < bitstring.length(); i++){
                    if(bitstring.at(i) != '1' && bitstring.at(i) != '0'){
                        cout << "'" << optarg << "' is not a valid bitstring." << endl;
                        return false;
                    }
                }
                break;
            case 'h': // help
                print_help();
                return false;
                break;
            case 'p'://prints out compiler options
                print_options();
                return false;
                break;
            case 'n'://prints out the number of optimization flags
                cout<<numOPTS<<endl;
                return false;
                break;
            default:
                print_help();
                return false;
                break;
        }
    }
    return true;
}

// If there is no given bitstring exit
// If a bitstring is not of length exit
void process_bitstring(){
    int len = bitstring.length();
    if(len != numOPTS){

        cout<<"The bitstring does not match the number of optimizations"<<endl;
        cout<<"bitstring length "<<len<<". Number optimizations "<<num_flags<<endl;
        exit(-1);
    }
}

// Turns the bitstring into flags and returns a string of the op flags.
string bit2flag(){
    string output = "";
    vector <string> flags_list;

        flags_list.assign(ENABLED_FLAGS, ENABLED_FLAGS + numOPTS);
        all_op_flags.assign(ENABLED_FLAGS, ENABLED_FLAGS + numOPTS);

    for(int i = 0; i < bitstring.length(); i++)
        if(bitstring.at(i) == '1')
            output += flags_list.at(i) + " ";

    return output;
}

void print_options(){

    cout<< "The optimization flags associated with this executable:" << endl;
    for(int i=0; i<numOPTS; i++)
      cout<<ENABLED_FLAGS[i]  << endl;

    return;

}
