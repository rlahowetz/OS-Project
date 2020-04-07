// proj2.cpp
// 
// Ryan Lahowetz
// Shell and a few other functions used for it

#include "my_OS.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

//////////////cat functions

void File::catFunc(){  //When cat is called on a file type
    cout << "Invalid File Type" << endl;
}

void TextF::catFunc(){  //prints out the contents of the text file
    cout << text << endl;
}

//////////////run functions
void File::runFunc(){ //When run is called on a generic File
    cout << "Invalid File Type" << endl;
}

void ProgF::runFunc(){ //"Runs" program
    cout << "Running program " << name << endl;
}

//////////////step functions
void File::stepFunc(){ //When step is called on Generic File
    cout << "Invalid File Type" << endl;
}


//////////////start functions
void File::startFunc(){ //When start is called on Generic File
    cout << "Invalid File Type" << endl;
}

void ProgF::startFunc(){ //"Starts" the program file
    cout << "Starting program " << name << endl;
}
void ProgF::stepFunc(){ //"Starts" the program file
    cout << "Starting program " << name << endl;
}


//The Psudo-shell starting at the root level
//only applied to root directory
void Directory::runShell(Directory* rt, ProcessHandler* procH){
    ProcessHandler* dummy;
    string currWorkingDir = "root"; //cumulative string for pwd


    cout << "EnterCommand>";

    string response; //User response
    bool shellOver = false;

    string dumN = "a";
    ProgF dummyP(dumN,2,3);

    
    while (!shellOver){//until user inputs Quit
    cin >> response;
    
    if(response.compare("Quit") == 0 || response.compare("quit") == 0){
    shellOver = true;
    EnteredQuit();
    break;
    }

    if(response == "cd"){ //change directory
        cin >> response;
        
        response = response + ".d"; //add .d extension to file name

        int fileIndex = -1;

        for(int i=0;i<rt->numFiles;i++){ //Finds the file name in root's "Stuff"
            if(response == rt->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){  //file not found
            cout << "File " << response << " not found" << endl;
        }
        else{ //file found
            Directory* handle = (Directory*) rt->stuff[fileIndex];
            rt->reccShell(handle,currWorkingDir, procH);
        }

        if(checkQuitBool()){
            shellOver = true;
            break;
        }
    }


    if(response == "test" ){
        procH->printReadyQ();
    }

    
    if(response == "setBurst"){ 
        int burstAmt;
        cin >> burstAmt;
        procH->burstTime = burstAmt;
    }
    if(response == "getBurst"){ 
        cout << procH->burstTime << endl;
    }
    
    if(response == "setMemory"){ 
        int memAmt;
        cin >> memAmt;
        procH->setTotalMemroy(memAmt);
        
    }
    if(response == "getMemory"){ 
        cout << "Total memory for system: " << procH->totalmemory << endl;
    }

    if(response == "cat"){ //cat function
        cin >> response;
        int fileIndex = -1;

        for(int i=0;i<rt->numFiles;i++){ //find the file name in rt's stuff
            if(response == rt->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){ //file not found
            cout << "File " << response << " not found" << endl;
        }
        else{ //found file: run catfunc on said file
            rt->stuff[fileIndex]->catFunc();
        }
    }

    if(response == "mkdir"){ //mkdir command
        cin >> response;
        response = response + ".d"; //add .d extension
        if(rt->checkDirName(response)){ //checks name, creates Dir, and adds it
            File* handler;
            Directory* dirHandler = new Directory(response);
            handler = dirHandler;
            rt->add(dirHandler);
        }
    }

    if(response == "createTextFile" || response == "CreateTextFile"){ //createFile command
        cin >> response;
        if(response.length() < 2){ //checks length for extension
            cout << "Invalid extention" << endl;
        }
        else if (response.substr(response.length()-2,response.length()) != ".t"){ //check for appropriate extension
            cout << "Invalid extention" << endl;
        }
        else{ // create and add text file
            cout << "Enter fine name>";
            string newTextFileName = response;
            
            cout << "Enter file contents>";
            string firstWord;
            string newTextFileContent;
            cin >> firstWord;
            getline(cin, newTextFileContent);
            newTextFileContent = firstWord + newTextFileContent;

            File* newTextFileHandler = new TextF(newTextFileName, newTextFileContent);
            rt->add(newTextFileHandler);
        }
    }

    if(response == "run"){ // run function
        procH->run();
    }

   
    if(response == "start"){ //exact same as run and cat, except with start
        cin >> response;
        response = response + ".p";

        int fileIndex = -1;

        for(int i=0;i<rt->numFiles;i++){
            if(response == rt->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){
            cout << "File " << response << " not found" << endl;
        }
        else{
            ProgF* la = (ProgF*) rt->stuff[fileIndex];
            procH->startProg(*la);
        }
    }
    

   
    if(response == "step"){ //exact same as start, run, and cat, but with step
        int stpAmt;
        cin >> stpAmt;
        procH->step(stpAmt, false);
    }
    

    if(response == "ls"){ // ls command
        cout << "Directory Name: root" << endl;

        for(int i=0;i < rt->numFiles;i++){ //for each item in root
            rt->stuff[i]->declareData();   //print it out
        }
    }

    if(response == "pwd"){ //pwd, always root here, but will use the variable anyway
        cout << "Current directory is " << currWorkingDir << endl;
    }

    /////////////////Project 3: addProg
    if(response == "addProgram"){
        getline(cin,response);
        stringstream rest_of_line(response);

        string progName;
        rest_of_line >> progName;

        progName = progName + ".p";

        if(dummyP.checkFileName(progName)){

            int cpu_requirements;
            rest_of_line >> cpu_requirements;
            int memory_requirements;
            rest_of_line >> memory_requirements;

            ProgF* temp = new ProgF(progName, cpu_requirements, memory_requirements);

            if(!rest_of_line.eof()){
                int ioSt, ioT;
                while(!rest_of_line.eof()){
                    rest_of_line >> ioSt;
                    rest_of_line >> ioT;
                    temp->addIOPair(makeIOPair(ioSt, ioT));
                }
            }
        
            rt->add(temp);
        }
    }
    cout << "EnterCommand>";
    }
 
}

////Psudo-shell for Directories that aren't root
void Directory::reccShell(Directory* cwd, string path, ProcessHandler* procHr){
    ProcessHandler* dummy = procHr;

    
    string currWorkingDir = path + "/" + cwd->name.substr(0,cwd->name.length()-2); // adds a "/", and the name of the Directory we're in
                                                                                    // minus the extension .d

    string response; //User response
    bool shellOver = false;

    cout << "EnterCommand>";
    
    while (!shellOver){//until user inputs cd ..
    cin >> response;

    if(response == "quit" || response == "Quit"){
        shellOver = true;
        EnteredQuit();
        break;
    }

    if(response == "cd"){ //cd, used to find the end of this recursive loop
        cin >> response;
        if(response == ".."){ // cd .. -> end of this directory loop
            shellOver = true;
            break;
        }
        else{ //exact same as cd above, except checks "this" directory instead
        
        response = response + ".d";

        int fileIndex = -1;

        for(int i=0;i<cwd->numFiles;i++){
            if(response == cwd->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){
            cout << "File " << response << " not found, " << cwd->numFiles << endl;
            for(int i=0;i<cwd->numFiles;i++){
                cout << i << ": " << cwd->stuff[i]->getName() << endl;
            }
        }
        else{
            Directory* handle = (Directory*) cwd->stuff[fileIndex];
            cwd->reccShell(handle,currWorkingDir, dummy);
        }

        if(checkQuitBool()){
            shellOver = true;
            break;
        }

        }
    }

    if(response == "setBurst"){ 
        int burstAmt;
        cin >> burstAmt;
        dummy->setBurst(burstAmt);
    }

    if(response == "getBurst"){ 
        cout << dummy->burstTime << endl;
    }
    
    if(response == "setMemory"){ 
        int memAmt;
        cin >> memAmt;
        dummy->setTotalMemroy(memAmt);
        
    }
    if(response == "getMemory"){ 
        dummy->getTotalMemory();
    }

    if(response == "cat"){ //exact as cat above, except searches "this" directory instead
        cin >> response;
        int fileIndex = -1;

        for(int i=0;i<cwd->numFiles;i++){
            if(response == cwd->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){
            cout << "File " << response << " not found" << endl;
        }
        else{
            cwd->stuff[fileIndex]->catFunc();
        }
    }

    if(response == "mkdir"){
        cin >> response;
        response = response + ".d";
        if(cwd->checkDirName(response)){ //checks name, creates Dir, and adds it
            File* handler;
            Directory* dirHandler = new Directory(response);
            handler = dirHandler;
            cwd->add(dirHandler);
        }
    }
 
    if(response == "createTextFile" || response == "CreateTextFile"){ //exact as mkdir above, except uses "this" directory instead
        cout << "Enter filename>";
        cin >> response;
        if(response.length() < 2){
            cout << "Invalid extention" << endl;
        }
        else if (response.substr(response.length()-2,response.length()) != ".t"){
            cout << "Invalid extention" << endl;
        }
        else{
            string newTextFileName = response;
            
            cout << "Enter file contents>";
            string firstWord;
            string newTextFileContent;
            cin >> firstWord;
            getline(cin, newTextFileContent);
            newTextFileContent = firstWord + newTextFileContent;

            File* newTextFileHandler = new TextF(newTextFileName, newTextFileContent);
            cwd->add(newTextFileHandler);
        }
    }

    if(response == "run"){ // run function
        dummy->run();
        cout << "worked" << endl;
    }

    if(response == "start"){ //exact same as run and cat, except with start
        cin >> response;
        response = response + ".p";

        int fileIndex = -1;

        for(int i=0;i<cwd->numFiles;i++){
            if(response == cwd->stuff[i]->getName()){
                fileIndex = i;
            }
        }

        if(fileIndex == -1){
            cout << "File " << response << " not found" << endl;
        }
        else{
            ProgF* la = (ProgF*) cwd->stuff[fileIndex];
            dummy->startProg(*la);
        }
    }

    if(response == "ls"){ //exact as ls above, except uses "this" directory instead
        cout << "Directory Name: " << cwd->name.substr(0,cwd->name.length()-2) << endl;

        for(int i=0;i < cwd->numFiles;i++){
            cwd->stuff[i]->declareData();
        }
    }

    
    if(response == "pwd"){
        cout << "Current directory is " << currWorkingDir << endl;
    }

    //Project 3 
    if(response == "addProgram"){
        getline(cin,response);
        stringstream rest_of_line(response);

        string progName;
        rest_of_line >> progName;

        progName = progName + ".p";
        cout << progName << endl;

        ProgF dummyP(progName,0,0);
        if(dummyP.checkFileName(progName)){

        int cpu_requirements;
        rest_of_line >> cpu_requirements;
        int memory_requirements;
        rest_of_line >> memory_requirements;

        ProgF* temp = new ProgF(progName, cpu_requirements, memory_requirements);

        if(!rest_of_line.eof()){
            int ioSt, ioT;
            while(!rest_of_line.eof()){
                rest_of_line >> ioSt;
                rest_of_line >> ioT;
                temp->addIOPair(makeIOPair(ioSt, ioT));
            }
        }
        
        cwd->add(temp);
        }
    }

    cout << "EnterCommand>";

    }
}
