// proj3.cpp
// 
// Ryan Lahowetz

#include "my_OS.h"
#include <iostream>
#include <fstream>

using namespace std;


//OSObj - constructor for an OSObj object
OSObj::OSObj(){

    root = new Directory();
    proH = new ProcessHandler();

}


///////////////////////////////////////////////////
// testDeclare
//
// Prints all data for a program for debugging
// (to make sure the io pairs are being saved)
void ProgF::testDeclare(){
    cout << name << endl;
    cout << "cpuR: " << cpuReq << endl;
    cout << "memR: " << memReq << endl;
    cout << "ioPairsize: " << ioPairSize << endl;

    for(int i=0;i<ioPairSize;i++){
        cout << "ioStart: " << ioData[i].startIO << endl;
        cout << "ioTime: " << ioData[i].ioTime << endl;
        cout << endl;
    }
    
}

//makeIOPair
//
//creates an IOPair object
//
//strt - start time for io
//time - time it takes for io to complete
ioPair makeIOPair(int strt, int time){
    ioPair temp;
    temp.ioTime = time;
    temp.startIO = strt;

    return temp;
}

//addIOPair
//
//adds an iopair to the list of ioPairs that each program contains
//target - the iopair object to be added to this program
void ProgF::addIOPair(ioPair target){
    
    ioPairSize = ioPairSize + 1; //inc number of iopairs
    ioPair* newData = new ioPair[ioPairSize]; //create a larger list of io data

    ioPair* temp = ioData; //set a temp pointer for deletion use

    for(int i=0;i<ioPairSize-1;i++){ //element-wise copy
        newData[i] = ioData[i];
    }

    newData[ioPairSize-1] = target; //set the last item
    ioData = newData; //set the new io list

    delete temp; //delete old list
    temp = NULL;
}


///////////////////////////////////////////////////////
// Beginning for class ProcessHandler
// ProcessHandler constructor
ProcessHandler::ProcessHandler(){

    totalmemory = 0;
    avalibleMemory = 0;
    currentTime = 0;
    burstTime = 0;
    ioQSize = 0;
    ioQ = new ProgF*[0];

}

//enReadyQ
// adds the given program to the end of the ready queue
// p - the ProgF to be added to the end of Ready Queue
void ProcessHandler::enReadyQ(ProgF p){
    readyQ.push_back(p);
}

//printReadyQ
// prints the contents of the ready queue
void ProcessHandler::printReadyQ(){
    for(int i=0;i<readyQ.size();i++){
        cout << readyQ.at(i).name << endl;
    }
}

//enFinQ
// adds given program to the end of the finnished queue
// p - the ProgF to be added
void ProcessHandler::enFinQ(ProgF p){
    avalibleMemory = avalibleMemory + p.memReq;
    finQ.push_back(p);
}

//printFinQ
// prints the contents of the finnished queue
void ProcessHandler::printFinQ(){
    for(int i=0;i<finQ.size();i++){
        cout << finQ.at(i).name << endl;
    }
}

// enIOQ
// adds the given program (via pointer) to the
// end of the io list
//
// p - the pointer of the program to be added
void ProcessHandler::enIOQ(ProgF* p){

    ProgF** temp = ioQ; //temporary

    ioQSize = ioQSize + 1; //inc number of files

    ioQ = new ProgF*[ioQSize]; //set contents equal to the new size

    for(int i=0;i<ioQSize-1;i++){//element-wise copy of files
        ioQ[i] = temp[i];
    }

    ioQ[ioQSize-1] = p; //fill in the last item

    delete temp;
    temp = NULL;
}

//moveIntoIO
//moves the given program into the IO list (gets rid of the pointer)
void ProcessHandler::moveIntoIO(ProgF p){
    ProgF* handler = &p;

    enIOQ(handler);
}


//moveBackIntoReady
//
// Takes: Program Pointer
//        index of that pointer in ioq
//
// puts the given program into ready queue
// removes program of given index from ioq
void ProcessHandler::moveBackIntoReady(ProgF* p, int pIndex){

    enReadyQ(*p);

    ioQSize = ioQSize - 1;
    
    for(int i=pIndex;i<ioQSize;i++){
        ioQ[i] = ioQ[i+1];
    }
}

//printIOQ
// prints the contents of the IO list
void ProcessHandler::printIOQ(){
    for(int i=0;i<ioQSize;i++){
    cout << ioQ[i]->name << endl;
    }
}

//getTotalMemory
//Gets the set total memory for the simulation
int ProcessHandler::getTotalMemory(){
    return totalmemory;
}

//setTotalMemory
// sets the total memory to the given response
// newMem - the new memory for the system to be set to
void ProcessHandler::setTotalMemroy(int newMem){
    if(newMem > 0){ //if the suggested mem is positive
    if(newMem >= totalmemory){ //and it isnt shrinking
        avalibleMemory = avalibleMemory + (newMem - totalmemory);
        totalmemory = newMem;
    }
    else{
        cout << "fuctionallity to decrease total memroy may cause current programs to be kicked" << endl;
        cout << "will be added later" << endl;
    }   
    }
    else{
        cout << "memory amount must be greater than 0" << endl;
    }
}

//setBurst
//sets the burst time for the simulation
//newBurst - the burst it will be set to
void ProcessHandler::setBurst(int newBurst){
    if(newBurst > 0){
    burstTime = newBurst;
    }
    else{
        cout << "Burst time must be a positive int" << endl;
    }
}

//startProg
// adds the program to the simulation's ready queue
void ProcessHandler::startProg(ProgF p){
    if(avalibleMemory >= p.memReq){
        p.makeCopies();
        enReadyQ(p);
        avalibleMemory = avalibleMemory - p.memReq;
    }
    else if(p.memReq > totalmemory){
        cout << "Unable to start " << p.name << " due to space constraints." << endl;
    }
    else{
        p.makeCopies();
        p.isinVM = true;
        enReadyQ(p);
    }
}

//checkIfDone
//returns true if there are no longer tasks in the ready queue or
// io list to complete
bool ProcessHandler::checkIfDone(){
    bool decision = false;
    if(readyQ.size() == 0 && ioQSize == 0){
        decision = true;
    }
    return decision;
}

//checkIfValidStartState
//returns true if there are items in the ready queue, the burst time is
//positive.
bool ProcessHandler::checkIfValidStartState(){
    bool decision = false;
    if(readyQ.size()>0){
        if(burstTime > 0){
            decision = true;
        }
        else{
            cout << "Burst time not set to a positive integer" << endl;
        }
    }
    else{
        cout << "No processes to be processed" << endl;
    }

    return decision;
}

//setHelper
// checks to see if the given step amount is positive before stepping
// x - the amount to step by
void ProcessHandler::stepHelper(int x){
    
    if(checkIfValidStartState()){
        if(x > 0){
            step(x, false);
        }
        else{
            cout << "Step must be positive int" << endl;
        }
    }
}

//step
// moves the simulation through by one step
//numSteps - number of steps to move by (must be positive)
//isRun - a value to decide whether to do the first simulation print
//         depending on if stephelper or run is calling
void ProcessHandler::step(int numSteps, bool isRun){
    
    if(!isRun){
        stepPrint();
    }
    
    for(int i=0;i<numSteps;i++){

        if(!checkIfDone()){
        effectiveBurstTime = burstTime; //reset burst time


        if(readyQ.front().hasIOLeft()){ //item in cpu has IO to compare to
            if(currentTime >= readyQ.front().copy.startIO){ 
                //current time is ahead of when io was supposed to start

                cout << "current time was found ahead: moving" << endl;
                moveIntoIO(readyQ.front());
                readyQ.pop_front();
                
            }
            else if((effectiveBurstTime + currentTime) >= readyQ.front().copy.startIO){
                //will it "burst" into the start of io? if so...
                cout << "current time was found to burst into the start of io:" << endl;

                effectiveBurstTime = readyQ.front().copy.startIO - 
                    currentTime;

                cout << "Amount applied:::: " << effectiveBurstTime << endl;
                cout << "before burst, " << readyQ.front().name << " is in front" << endl;
                
                currentTime = currentTime + effectiveBurstTime;
                readyQ.front().currentCPUReq = readyQ.front().currentCPUReq - effectiveBurstTime;
                updateTimeTaken(effectiveBurstTime);

                cout << "Moving " << readyQ.front().name << " to ioq" << endl;
                moveIntoIO(readyQ.front());
                readyQ.pop_front();
                cout << "The item in the cpu is: " << readyQ.front().name << endl;
            
            }
            else{ //will not affect io: act normal
                cout << "found that has io, not affected" << endl;
                if(readyQ.front().currentCPUReq < effectiveBurstTime){
                    effectiveBurstTime = readyQ.front().currentCPUReq;
                    fullUpdate(effectiveBurstTime);
                }
                else{
                    fullUpdate(effectiveBurstTime);
                }
            }
        }
        
        else{ //item in cpu does not have IO to process

            if(readyQ.front().isinVM){
                makeRoomForProg(readyQ.front().memReq);
                readyQ.front().isinVM = false;
                effectiveBurstTime = effectiveBurstTime - 2;
                currentTime = currentTime + 2;
                updateTimeTaken(2);
            }

            if(readyQ.front().currentCPUReq < effectiveBurstTime){
                effectiveBurstTime = readyQ.front().currentCPUReq;
                fullUpdate(effectiveBurstTime);
            }
            else{
                fullUpdate(effectiveBurstTime);
            }
        }
        }
        else{ //no items in ioQueue or ready Queue
            cout << "Processing is done" << endl;
        }

        stepPrint();
    }
}

//stepPrint
//prints the status of the simulation accourding to the required format
void ProcessHandler::stepPrint(){
    ProgF* progPointer = &readyQ.front();

    cout << "Current time <" << currentTime << ">\n";
    cout << "Running job ";

    if(readyQ.size() > 0){
        cout << progPointer->name.substr(0,progPointer->name.length()-2);
        cout << " has " << progPointer->currentCPUReq << " time left and is using ";
        cout << progPointer->memReq << "\n\tmemory resource(s)";
        if(readyQ.front().isinVM){
                cout << " on disk." << endl;
            }
            else{
                cout << "." << endl;
            }
    }
    else{
        cout << "is empty" << endl;
    }

    
    cout << "The queue is:";

    if(readyQ.size() > 1){
        for(int i=1;i<readyQ.size();i++){
            progPointer = &readyQ.at(i);

            cout << "\n\tPosition " << i << ":  job " << progPointer->name.substr(0,progPointer->name.length()-2);
            cout << " has " << progPointer->currentCPUReq << " time left and is using " << progPointer->memReq
                << "\n\t\tmemory resources";

            if(readyQ.at(i).isinVM){
                cout << " on disk." << endl;
            }
            else{
                cout << "." << endl;
            }
        }
    }
    else{
        cout << " empty" << endl;
    }


    if(finQ.size() > 0){
        cout << "Finnished jobs are:\n";
        for(int i=0;i<finQ.size();i++){
            cout << "\t" << finQ.at(i).name.substr(0,finQ.at(i).name.length()-2)
            << " " << finQ.at(i).cpuReq
            << " " << finQ.at(i).timeTaken << endl;
        }
    }

    
    if(ioQSize > 0){
        for(int i=0;i<ioQSize;i++){
            cout << "The process " << ioQ[i]->name
            << " is obtaining IO and will be back in " << ioQ[i]->ioData[0].ioTime
            << " unit(s)." << endl;
        }
    }
}

//updateTimeTaken
// updates all processes full time taken (not in the finnished queue)
// by the given amount of time
// efBurst - time taken to apply to all programs in the simulation
void ProcessHandler::updateTimeTaken(int efBurst){

    if(readyQ.size() > 0){
    for(int i=0;i<readyQ.size();i++){
        readyQ.at(i).timeTaken = readyQ.at(i).timeTaken + efBurst;
    }
    }

    
    if(ioQSize > 0){
    for(int i=0;i<ioQSize;i++){
        ioQ[i]->timeTaken = ioQ[i]->timeTaken + efBurst;
    }
    }

}

//fullUpdate
//calls update functions for both the ready queue and the IO list
void ProcessHandler::fullUpdate(int efBurst){

    updateIOQ(efBurst);
    updateReadyQ(efBurst);
}

//updates all items in the io list by the effective burst time
void ProcessHandler::updateIOQ(int efBurst){

    for(int i=0;i<ioQSize;i++){
        ioQ[i]->copy.ioTime = 
            ioQ[i]->copy.ioTime - efBurst;
            cout << "New ioTime: " << ioQ[i]->copy.ioTime << endl;

        
        if(ioQ[i]->copy.ioTime <= 0){
            //cout << "worked btw" << endl;
            ioQ[i]->currentIOPairIndex = 
                ioQ[i]->currentIOPairIndex + 1; //inc ioPairIndex

            if(ioQ[i]->hasIOLeft()){ //if we're still in this prog's io list, set the new copy
                ioQ[i]->copy = ioQ[i]->ioData[ioQ[i]->currentIOPairIndex];
            }

            moveBackIntoReady(ioQ[i],i);
            //ioQ.erase(i,i);
        }
    }
}

//updateReadyQ
//updates the ready Queue by the effective burst time
void ProcessHandler::updateReadyQ(int efburst){

    currentTime = currentTime + efburst; //update current time

    readyQ.front().currentCPUReq = readyQ.front().currentCPUReq - efburst;
    updateTimeTaken(efburst);

    if(readyQ.front().currentCPUReq <= 0){ //if proc in cpu is done
        enFinQ(readyQ.front());
        readyQ.pop_front();
    }
    else{ //if proc in cpu is not done
        
                enReadyQ(readyQ.front());
                readyQ.pop_front();
    }
}

//makeRoomForProg
//move items from the back of the ready queue into VM until
//the given amount of memory is freed for use
//memNeeded - the memory needed to be freed
void ProcessHandler::makeRoomForProg(int memNeeded){
    int readyQIndex = readyQ.size() - 1;

    while(avalibleMemory < memNeeded){
        if(!readyQ.at(readyQIndex).isinVM){
            avalibleMemory = avalibleMemory + readyQ.at(readyQIndex).memReq;
            readyQ.at(readyQIndex).isinVM = true;
        }
        readyQIndex = readyQIndex - 1;
    }
    cout << "new avalible memory is " << avalibleMemory << endl;
}

/*
void ProcessHandler::resetSim(){
    for(int i=0;i<finQ.size();i++){
        finQ.at(i).currentCPUReq = finQ.at(i).cpuReq;
        finQ.at(i).timeTaken = 0;
    }
    for(int i=0;i<finQ.size();i++){
        finQ.pop_front();
    }
}
*/


//run
//calls the step function until there are no longer items in the ready queue
//(io stuff not working)
void ProcessHandler::run(){
    cout << "Advancing the system until all jobs are finnished." << endl;
    stepPrint();
    while(readyQ.size() > 0){
        step(1, true);
    }
}


