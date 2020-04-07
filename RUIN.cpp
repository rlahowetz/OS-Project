//RUFS
//
//@Ryan Lahowetz
//
// Jan 29 2019
//
// reads in a flag, and creates or reads a binary file system

#include "my_OS.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char** argv){

   OSObj erThang;

   Directory* rt = new Directory();

   rt = rt->readIn(argc,argv);

   erThang.root = rt;

   erThang.root->runShell(erThang.root, erThang.proH);
   
   erThang.root->writeOut(argc,argv);

   return 0;
}