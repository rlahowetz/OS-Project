

//Project_1.cpp
//
//@Ryan Lahowetz
// uses this space for Project 1, RUFS
//
// JAN 29, 2019

#include "my_OS.h"
#include <iostream>
#include <fstream>

using namespace std;

bool* quitBool = new bool(false);

void EnteredQuit(){
    *quitBool = true;
}

bool checkQuitBool(){
    return *quitBool;
}

//File
//Purely an interface
/////////////////////////////////////////////////////////
void File::declareData(){
    cout << "this should never happen" << endl;
}

void File::reccWrite(fstream& ofile){
    cout << "this should never happen" << endl;
}

void File::reccRead(ifstream& ifile){
    cout << "this should never happen" << endl;
}


//Directory
//////////////////////////////////////////////////////////////

//Returns true if the given string is the correct size for
// a Directory
bool Directory::checkDirName(string& s){

    bool decision;

    if(s.length() > 10 || s.length() < 1){
        decision = false;
        cout << "File length unacceptable" << endl;
    }
    else{
        decision = true;
    }

    return decision;

}


Directory::Directory(){
    name = "root.d";
    numFiles = 0;

    stuff = new File*[numFiles];
}


Directory::Directory(string& s){

    if(checkDirName(s)){
        name = s;
        numFiles = 0;
        stuff = new File*[numFiles];
    }
    else{
        cout << "Directory name " << s << "is invalid." << endl;
    }

}

//prints contents of object for testing
void Directory::declareData(){
    cout << "File name:  ";
    cout << name.substr(0, name.length()-2) << "\tFile type:  Directory" << endl;
    
}


//adds a given File to the directory's contents
// via dynamic growth
void Directory::add(File* dir){

    File ** temp = stuff; //temporary

    numFiles = numFiles + 1; //inc number of files

    stuff = new File*[numFiles]; //set contents equal to the new size

    for(int i=0;i<numFiles-1;i++){//element-wise copy of files
        stuff[i] = temp[i];
    }

    stuff[numFiles-1] = dir; //fill in the last item

    delete temp;
    temp = NULL;

}

// Text Files
///////////////////////////////////////////////////////////////////////////////
TextF::TextF(string& s, string& str){

    if(TextF::checkFileName(s)){
    name = s;
    size = str.size() * sizeof(char);
    text = str;
    }
    else{
    }
}

//prints out its conents for debugging
void TextF::declareData(){
    cout << "File name:  ";
    cout << name << "\tFile type:  Text file" << endl;
}

//returns true if given string is a valid length of a Text file
bool TextF::checkFileName(string& s){
    bool decision;
    if(s.size() > 11 || s.size() < 1){
        cout << "Invalid File name: " << s << endl;
        decision = false;
    }
    else{
        decision = true;
    }
    return decision;
}

//Program Files
//////////////////////////////////////////////////////////////////////////
//Returns true if given string is a valid length for a Program file name
bool ProgF::checkFileName(string& s){
    bool decision;
    if(s.size() > 11 || s.size() < 1){
        cout << "Invalid File name: " << s << endl;
        decision = false;
    }
    else{
        decision = true;
    }
    return decision;
}

ProgF::ProgF(string& s,int cpur, int memR){
    if(checkFileName(s)){
        name = s;
        cpuReq = cpur;
        currentCPUReq = cpuReq;
        memReq = memR;
        ioData = new ioPair[0];
        ioPairSize = 0;
        timeTaken = 0;
        isinVM = false;
    }
    else{
        cout << "File name Invalid" << endl;
    }
}


ProgF::ProgF(string& s,int cpur, int memR, ioPair* inPairs, int numIOPairs){
    if(checkFileName(s)){
        name = s;
        cpuReq = cpur;
        currentCPUReq = cpuReq; //var
        memReq = memR;
        ioData = inPairs;
        ioPairSize = numIOPairs; 
        timeTaken = 0; //var
        isinVM = false;

        
    }
    else{
        cout << "File name Invalid" << endl;
    }
}

void ProgF::makeCopies(){
    copy = ioData[0];
    currentIOPairIndex = 0;
}

void ProgF::printCopies(){
    cout << "io copy ioTime: " << copy.ioTime << endl
     << "io copy ioStart: " << copy.startIO << endl;
}

//returns true if this program has io pairs assigned
bool ProgF::hasIO(){
    bool decision = false;
    if(ioPairSize > 0){
        decision = true;
    }
    return decision;
}

//returns true if there is io to still be handled
bool ProgF::hasIOLeft(){ 
    bool decision = false;
    if(hasIO()){ //there is io to do
        cout << "halfway there" << endl;
        decision = true;
        if(currentIOPairIndex >= ioPairSize - 1){// and we're currently in the list of io to do
            decision = false;
        }
    }
    else{
    }
    return decision;
}


//Prints out Program file contents for ls
void ProgF::declareData(){
    cout << "File name:  ";
    cout << name << "\tFile type:  Program file" << endl;

    cout << "MemR: " << memReq << endl;
    cout << "cpuR: " << cpuReq << endl;
    
    for(int i=0;i<ioPairSize;i++){
        cout << "ioStart " << i+1 << ": " << ioData[i].startIO << endl;
        cout << "ioTime " << i+1 << ": " << ioData[i].ioTime << endl;
    }
}

//get Names
string File::getName(){
    return name;
}
string Directory::getName(){
    return name;
}
string ProgF::getName(){
    return name;
}
string TextF::getName(){
    return name;
}

string TextF::getText(){
    return text;
}