
#include <string>
#include <deque>
#include <list>
#ifndef MY_OS_H
#define MY_OS_H
using namespace std;
//my_OS.h
//
//@Ryan Lahowetz
//
// jan 29 2019
//
//header file for OS projects
////Descriptions for all Functions in proj1.cpp

void addToCurrent(int a);
int getCurrent();

void EnteredQuit();
bool checkQuitBool();

void addReadCur(int a);
int getreadCur();

struct ioPair{
        int startIO;
        int ioTime;
};
ioPair makeIOPair(int strt, int time);

class File
{
    public:
    string name;
    virtual void declareData();
    virtual void catFunc();
    virtual void stepFunc();
    virtual void startFunc();
    virtual void runFunc();
    virtual string getName();

    virtual void reccWrite(fstream& ofile);
    virtual void reccRead(ifstream& ifile);
};

class Directory;

class ProgF : public File
{
    public:
    int memReq;
    int cpuReq;
    int currentCPUReq; //var
    int ioPairSize;
    int timeTaken; //var

    ioPair* ioData;

    ioPair copy;
    int currentIOPairIndex;

    
    void addIOPair(ioPair target);

    ProgF(string& s, int cpur, int memr);
    ProgF(string& s, int cpur, int memr, ioPair* ioData, int numIOPairs);

    void makeCopies();
    void printCopies();

    void declareData();
    void testDeclare();
    void runFunc();
    void stepFunc();
    void startFunc();
    string getName();

   void reccWrite(fstream& ofile);
   void reccRead(ifstream& ifile, Directory* parentDir, string rdInNm);

   bool checkFileName(string& s);
   bool hasIO();
   bool hasIOLeft();

   bool isinVM;

   private:
};

class TextF : public File
{
    public:
    string text;
    int size;

    TextF(string& s, string& txt);

    void declareData();
    void catFunc();
    string getName();
    string getText();

    void reccWrite(fstream& ofile);
    void reccRead(ifstream& ifile, Directory* parentDir, string readName);

    bool checkFileName(string& s);

    private:
};

class ProcessHandler
{
    public:
    int burstTime;
    int effectiveBurstTime;
    int totalmemory;
    int avalibleMemory;
    int currentTime;
    int ioQSize;

    ProgF** ioQ;
    deque<ProgF> readyQ;
    deque<ProgF> finQ;
    

    ProcessHandler();


    void printReadyQ();
    void printFinQ();
    void printIOQ();

    void run();

    void enReadyQ(ProgF p);
    void enFinQ(ProgF p);
    void enIOQ(ProgF* p);
    void moveIntoFinnished();
    void moveIntoIO(ProgF p);
    void moveBackIntoReady(ProgF* p, int pIndex);

    void startProg(ProgF p);

    int getTotalMemory();
    void setTotalMemroy(int x);
    void setBurst(int x);

    void updateReadyQ(int effBurstTime);
    void updateIOQ(int effBurstTime);
    void updateTimeTaken(int effBurstTime);
    void fullUpdate(int effBurstTime); //updates all queues and the processes within them

    void step(int numSteps, bool isRun); 
    void stepHelper(int numSteps); //preceeds a step by 

    //void resetSim();   //resets all changed values during the simulation

    bool checkIfDone();
    bool checkIfValidStartState();
    void stepPrint();

    void makeRoomForProg(int memNeeded);
    private:
       //prints the individual step made
     //checks to see if readyq and ioq are empty
                        //returns true if they are

};

class Directory : public File
{
    public:
    int numFiles;
    

    File** stuff;

    Directory();
    Directory(string& s);

    void runShell(Directory* rt, ProcessHandler* ProgH);
    void reccShell(Directory* prev, string path, ProcessHandler* ProgH);

    void declareData();
    string getName();

    void add(File* d);

    void writeOut(int argc,char** argv);
    Directory* readIn(int argc,char** argv);

    bool checkDirName(string& s);

    

    private:
    void reccWrite(fstream& ofile);
    void reccRead(ifstream& ifile, Directory* parentDir);
};

class OSObj{
    public:
    OSObj();

    Directory* root;
    ProcessHandler* proH;

};

#endif