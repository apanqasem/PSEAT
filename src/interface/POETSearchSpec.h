#ifndef POET_Search_Spec_H
#define POET_Search_Spec_H

using namespace std;

#include<driver/Args.h>
#include<interface/SearchSpace.h>

void BuildConfFileFromPOETScript(Args& env);
void GenPOETInvocScript(const Args& env, SearchSpace *space);


#endif
