#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include <deque>

using namespace std;

//Emulates a text file
class TextFile{
public:
	int fileSize;
	string fileName, content;

	char *binName = NULL;//value holding formated name
						 //that will write to a binary file

	TextFile(){
		fileName = "";
		fileSize = 0; //binary size of content
		content = ""; //a string of content
		binName = new char[11];
	}

	TextFile(string name, string cont){
		fileName = name;
		fileSize = 0;
		content = cont;
		binName = new char[11];
	}

	//for printing in format described in Proj-1
	string declareData(){
		return "\t" + fileName + "\n\t\t" + content + "\n";
	}

	//formats a text file's filename to write out
	char* makeBinName(){

		//case if name is too long
		if(fileName.length() > 8){
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}
			binName[8]='.';
			binName[9]='t';
			binName[10]='\0';
		}

		//case if name is too short
		else if(fileName.length() < 8){
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}

			for(int i=fileName.length();i<8;i++){
				binName[i] = '\0';
			}

			binName[8]='.';
			binName[9]='t';
			binName[10]='\0';
		}

		//case if name is equal
		else{
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}
			binName[8]='.';
			binName[9]='t';
			binName[10]='\0';
		}


		return binName;
	}

	//Does the actual writing of a text file
	void writeTxt(fstream& f){

		        //formats
		f.write(makeBinName(), 11*sizeof(char));

		fileSize = sizeof(content);
		f.write((char*)&fileSize, sizeof(int));
		f.write((char*)&content, fileSize);
	}


};

//Emulates a program file
class ProgFile{
public:
	int fileSize, cpuReq,currentCpuReq, memReq,timeToDoIO,IOtime,timeToEx;
	string fileName;

	bool inVM; //flag telling whether or not program is in Virtual Memory

	char *binName = NULL;//value holding formated name
						 //that will write to a binary file

	ProgFile(){
		fileName = "";
		fileSize = 16;

		cpuReq = 0;
		memReq = 0;

		timeToDoIO = 0;
		IOtime = 0;
		timeToEx = 0;
		currentCpuReq = cpuReq;

		inVM = false;

		binName = new char[11];
	}

	ProgFile(string name,int cpuR,int memR){
		fileName = name;
		fileSize = 16;

		cpuReq = cpuR;
	    memReq = memR;

		timeToDoIO = 0;
		IOtime = 0;
		timeToEx = 0;
		currentCpuReq = cpuReq;

		inVM = false;

		binName = new char[11];
	}

	ProgFile(string name, int cpuR, int memR,int timeToDoIO_,int IOtime_){
		fileName = name;
		fileSize = 16;
		cpuReq = cpuR;
	    memReq = memR;
		timeToDoIO = timeToDoIO_;//time this process needs to stay in the io queue

		IOtime = IOtime_;        //time in the cpu before it needs to go into
                                 //the ready queue

		timeToEx = 0;            //total time in simulator

		currentCpuReq = cpuReq;  //variable to keep track of how much more
                                 //cpu time this process needs

        inVM = false;

		binName = new char[11];
	}

	bool isInVM(){
        return inVM;
	}

	void moveToVM(){
	    inVM = true;
	}

	void takeOutOfVM(){
	    inVM = false;
	}

	//prints in format for proj-1
	string declareData(){
		stringstream ss; //needed to convert int to string
		stringstream s2;

		ss << cpuReq;
		string cp = ss.str();
		s2 << memReq;
		string mem = s2.str();


		return fileName + "\n\tCPU Requirement: " + cp +
				"\n\tMem Requirement: " + mem + "\n";
	}

	//formats a program's filename to write to a file
	char* makeBinName(){
		//case if name is too long
		if(fileName.length() > 8){
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}
			binName[8]='.';
			binName[9]='p';
			binName[10]='\0';
		}
		//case if name is too short
		else if(fileName.length() < 8){
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}

			for(int i=fileName.length();i<8;i++){
				binName[i] = '\0';
			}

			binName[8]='.';
			binName[9]='p';
			binName[10]='\0';
		}

		//case if name is equal
		else{
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}
			binName[8]='.';
			binName[9]='p';
			binName[10]='\0';
		}


		return binName;
	}

	//does the actual writing of the program file
	void writeProg(fstream& f){

		f.write(makeBinName(), 11*sizeof(char));

		fileSize = 2*(sizeof(int));
		f.write((char*)&fileSize, sizeof(int));
		f.write((char*)&cpuReq, sizeof(int));
		f.write((char*)&memReq, sizeof(int));
	}


};

//  Project 3 begins here
//  Simulates running programs for project 3
class ProgramQueue{
	public:
	int memoryAmt, burstTime, currentTime, freeMem;
	deque<ProgFile> readyQ; //the ready queue
	deque<ProgFile> finQ;   //the queue for finished items
	deque<ProgFile> ioQ;    //the IO queue
	int swapCost = 2;
	//deque<ProgFile> vm;     //where programs go when there's not enough room for them


	ProgramQueue(){
		memoryAmt = 0;  //total memory in the simulator (cpu capacity)
		burstTime = 0;  //time used for each process before rotating
		currentTime = 0;//cumulative time simulator has been running
		freeMem = memoryAmt; //variable to keep track if a process can be added
                             //to the simulator

	}

	ProgramQueue(int mem, int burst){
		memoryAmt = mem;
		burstTime = burst;
		currentTime = 0;
		freeMem = memoryAmt;
	}

	//adds a program to the ready queue (and removes free space)
	void endReadyQ(ProgFile p){

        //if program's memory requirements are greater than the total memory possible
        if(p.memReq > memoryAmt){
            cout << "Program: " << p.fileName << " could not be added.  Not enough memory." << endl;
        }

        else{
                //if program's memory can fit in the simulator

                if(p.memReq > freeMem){ //when there isn't enough memory to fit, and something needs moving

                        if(readyQ.size() <= 1){ //when there is only 1 process
                            p.moveToVM();
                            readyQ.push_back(p);
                        }
                        else{ //when there are other processes that can move

                        int numProgToMove = 0; //number of programs from the end
                                                //to move to vm
                        int findingFreeSpace = 0; //used to cumulate amount of free
                                                    //space to compare

                        //finds the number of programs from the end of the queue
                        //i need to move to vm
                        for(int i = readyQ.size()-1; i>=1 ;i--){ //from back of list until running program
                            findingFreeSpace = findingFreeSpace + readyQ[i].memReq;
                            numProgToMove = numProgToMove + 1;

                            if(findingFreeSpace >= p.memReq){
                                break;
                            }
                        }

                        if(findingFreeSpace >= p.memReq){//if processes can be moved

                        //moves the appropriate number of programs to vm
                        for(int i=0;i < numProgToMove;i++){
                            readyQ[readyQ.size()-1 - i].moveToVM();
                            freeMem = freeMem + readyQ[readyQ.size()-1 - i].memReq;
                        }

                        freeMem = freeMem - p.memReq;
                        readyQ.push_back(p);
                        }
                        else{ //processes can't be moved, so put new process in vm
                            p.moveToVM();
                            readyQ.push_back(p);
                        }
                        }
                }
                else{ //when there IS enough memory to fit new process

                freeMem = freeMem - p.memReq;
                readyQ.push_back(p);

                }
        }
	}

	//adds a finished program to the finished queue
	void endFinQ(ProgFile p){
	    finQ.push_back(p);
	}

	//adds a program to the ioqueue
	void endIoQ(ProgFile p){
	    ioQ.push_back(p);
	}

	//pops and returns the front item of the ready queue (and gives free space)
	ProgFile popReady(){
	    ProgFile temp = readyQ.front();
	    freeMem = freeMem + temp.memReq;
	    readyQ.pop_front();
	    return temp;
	}

	//pops and returns the front program on the IO queue
	ProgFile popIO(){
	    ProgFile temp = ioQ.front();
        ioQ.pop_front();

        return temp;
	}

	//returns the memory amount the simulator has to store programs
	int getMemory(){
	    return memoryAmt;
	}

	//sets the memory amount the simulator has to store programs
	void setMemory(int n){

	    memoryAmt = n;
	}
	//returns the burst time the simulator uses on programs
	int getBurst(){
        return burstTime;
	}

	//sets the amount of time a program stays on the cpu
	void setBurst(int n){
	    burstTime = n;
	}

	//prints the status of all queues based on the assignment example
	void printAll(){

	cout << "Current time: <" << currentTime << ">" << endl;

        if(!readyQ.empty()){ //if there is something in the queue

            //running job
            cout << "Running job: " << readyQ.front().fileName
                    << " has " << readyQ.front().currentCpuReq << " units left and is using ";
                    if(readyQ.front().isInVM()){ //different if in vm or not
                        cout << readyQ.front().memReq << " resource on disk." << endl;
                    }
                    else{
                        cout << readyQ.front().memReq << " memory resources." << endl;
                    }

            //rest of queue
            cout << "The queue is: ";
            if(readyQ.size() == 1){
                cout << "empty" << endl;
            }
            else{
            cout << endl;
            for(int i=1;i<readyQ.size();i++){
                cout << "Position " << i << ": job " << readyQ[i].fileName
                    << " has " << readyQ[i].currentCpuReq << " units left and is using ";
                    if(readyQ[i].isInVM()){
                        cout << readyQ[i].memReq << " resource on disk." << endl;
                    }
                    else{
                        cout << readyQ[i].memReq << " memory resources." << endl;
                    }
            }
            }
        }
            else{ //ready q is empty
                cout << "The queue is:  empty" << endl;
            }

        //finnished jobs queue
        cout << "Finished jobs are: ";
        if(!finQ.empty()){
            cout << endl;
            for(int i=0;i < finQ.size();i++){
                cout << finQ[i].fileName << " " << finQ[i].cpuReq
                << " " << finQ[i].timeToEx << endl;
            }
        }
        else{
            cout << "empty" << endl;
        }

        //io queue jobs
        if(!ioQ.empty()){
            for(int i=0;i < ioQ.size();i++){
                cout << "The process " << ioQ[i].fileName <<
                " is obtaining IO and will be back in " << ioQ[i].timeToDoIO
                << " unit(s)." << endl;
            }
        }

    }

    //a single step of the simulator times the number of steps given
	void step(int amount){


	    while((readyQ.size()>0 || ioQ.size() > 0) && amount > 0){ //loop number of steps, or until
                                                                  //nothing left to process

            if(readyQ.front().isInVM()){ //when running process is in vm

                    if(readyQ.front().memReq > freeMem){ // not enough memory to support running process in vm

                        int numProgToMove = 0; //find how many programs from the end
                                                //to move to vm
                        int findingFreeSpace = 0; //used to cumulate amount of free
                                                    //space to compare

                        //finds the number of programs from the end of the queue
                        //i need to move to vm
                        for(int i = readyQ.size()-1; i>=1 ;i--){
                            findingFreeSpace = findingFreeSpace + readyQ[i].memReq;
                            numProgToMove = numProgToMove + 1;

                            if(findingFreeSpace >= readyQ.front().memReq){
                                break;
                            }
                        }

                        if(findingFreeSpace >= readyQ.front().memReq){//if processes can be moved

                        //moves the appropriate number of programs to vm
                        for(int i=0;i < numProgToMove;i++){
                            readyQ[readyQ.size()-1 - i].moveToVM();
                            freeMem = freeMem + readyQ[readyQ.size()-1 - i].memReq;//gives up the memory they have
                        }

                        freeMem = freeMem - readyQ.front().memReq; //take the newly given memory

                        readyQ.front().takeOutOfVM(); //transfer out of vm

                        //cost of taking out of vm
                        currentTime = currentTime + 2;
                        for(int i=0;i<readyQ.size();i++){
                            readyQ[i].timeToEx = readyQ[i].timeToEx + 2;
                        }
                        int newBurst = burstTime - 2;

                        //process the process
                        readyQ.front().currentCpuReq = readyQ.front().currentCpuReq - newBurst;

                        for(int i=0;i<readyQ.size();i++){
                            readyQ[i].timeToEx = readyQ[i].timeToEx + newBurst;
                        }

                        currentTime = currentTime + newBurst;

                        if(readyQ.front().currentCpuReq > 0){

                        //put it in the back
                            ProgFile temp = popReady();
                            endReadyQ(temp);
                        }
                        else{
                            ProgFile temp = popReady();
                            endFinQ(temp);
                        }

                        }

                        else{ //processes can't be moved, so put new process in vm
                                //should never be the case
                            cout << "something is horribly wrong" << endl;
                            ProgFile temp = popReady();
                            endFinQ(temp);
                        }

                    }
                    else{ //there is enough memory to support the running process in vm

                        freeMem = freeMem - readyQ.front().memReq;

                        readyQ.front().takeOutOfVM();


                        //cost of taking out memory from disk
                        for(int i=0;i<readyQ.size();i++){
                            readyQ[i].timeToEx = readyQ[i].timeToEx + swapCost;
                        }
                        currentTime = currentTime + swapCost;

                        int timeLeft = burstTime - swapCost; //"new burst time" after swap to spend in cpu

                        if(timeLeft <= readyQ.front().currentCpuReq){ //"new burst time" is less than or equal to requirement
                            readyQ.front().currentCpuReq = readyQ.front().currentCpuReq - timeLeft;
                            currentTime = currentTime + timeLeft;

                            for(int i=0;i<readyQ.size();i++){
                                readyQ[i].timeToEx = readyQ[i].timeToEx + timeLeft;
                            }

                            if(readyQ.front().currentCpuReq <= 0){
                                ProgFile temp = popReady();
                                endFinQ(temp);
                            }
                            else{
                                ProgFile temp = popReady();
                                endReadyQ(temp);
                            }

                        }
                        else{
                            int newBurst = readyQ.front().currentCpuReq;
                            readyQ.front().currentCpuReq = 0;
                            currentTime = currentTime + newBurst;

                            for(int i=0;i<readyQ.size();i++){
                                readyQ[i].timeToEx = readyQ[i].timeToEx + newBurst;
                            }

                            if(readyQ.front().currentCpuReq <= 0){
                                ProgFile temp = popReady();
                                endFinQ(temp);
                            }
                            else{
                                ProgFile temp = popReady();
                                endReadyQ(temp);
                            }

                        }
                    }

            }
            else{ //when running process is NOT in vm

                if(readyQ.front().currentCpuReq >= burstTime){//when process is longer than burst time
                    readyQ.front().currentCpuReq = readyQ.front().currentCpuReq - burstTime; //update cpu requirement
                    readyQ.front().timeToEx = readyQ.front().timeToEx + burstTime; //increment process time to execute
                    currentTime = currentTime + burstTime; //increment simulation time

                    for(int i=1; i < readyQ.size();i++){ //increment simulation time for all processes
                        readyQ[i].timeToEx = readyQ[i].timeToEx + burstTime;
                    }

                    if(readyQ.front().currentCpuReq > 0){
                        ProgFile temp = popReady();//take off process
                        endReadyQ(temp); //put it in the back of queue
                    }
                    else{
                        ProgFile temp = popReady();//take off process
                        endFinQ(temp); //put it in the back of queue
                    }
                    }
                else{   //when burst time is larger than process time
                    int newBurst = readyQ.front().currentCpuReq;
                    readyQ.front().currentCpuReq = 0; //zero the running process

                    for(int i=0; i < readyQ.size();i++){ //increment all process's time of execution
                        readyQ[i].timeToEx = readyQ[i].timeToEx + newBurst;
                    }

                    currentTime = currentTime + newBurst; //increment current time

                    if(readyQ.front().currentCpuReq > 0){
                        ProgFile temp = popReady();//take off process
                        endReadyQ(temp); //put it in the back of queue
                    }
                    else{
                        ProgFile temp = popReady();//take off process
                        endFinQ(temp); //put it in the back of queue
                    }              //and add it to the finished queue
                }

            }

            amount = amount - 1; //move along one step

            printAll(); //print status
            cout << endl;
	    }
	}

	//runs step until both ready queue AND IO queue are empty
	void run(){

	    cout << "Advancing system until all jobs have finished" << endl;

	    printAll();
	    cout << endl;
	    while((readyQ.size() > 0 )|| (ioQ.size() > 0)){
            step(1);
	    }

	}

	//prints the ready queue for debugging
	void printReady(){
		for(int i=0;i < readyQ.size();i++){
			cout << readyQ[i].fileName << endl;
		}
	}

	//prints the finished queue for debugging
	void printFin(){

		for(int i=0;i < readyQ.size();i++){
			cout << finQ[i].fileName << endl;
		}
	}

	//prints the IO queue for debugging
	void printIo(){
		for(int i=0;i < readyQ.size();i++){
			cout << ioQ[i].fileName << endl;
		}
	}
};

//  Project 3 ends here


//models a directory
class Directory{
public:
	int numTxt,numProg,numDir,totalFiles;
	string fileName;
	ProgramQueue simulator; //Program simulator for this directory
	char *binName = NULL; //value holding formated
						  //filename that will write to binfile

	TextFile *txtArray = NULL; //array holding all text files

	ProgFile *progArray = NULL; //array holding all programs

	Directory *dirArray = NULL; //array holding all directories this
								//directory contains

	Directory(){
		fileName = "";
		numTxt = 0;
		numProg = 0;
		numDir = 0;
		totalFiles = 0;

		txtArray = new TextFile[numTxt];
		progArray = new ProgFile[numProg];
		dirArray = new Directory[numDir];
		binName = new char[11];

	}

	Directory(string name){
		fileName = name;
		numTxt = 0; //number of text files
		numProg = 0; //number of progfiles
		numDir = 0; //number of directories
		totalFiles = 0; //cumulation of all files(and directories)

		txtArray = new TextFile[numTxt];
		progArray = new ProgFile[numProg];
		dirArray = new Directory[numDir];
		binName = new char[11]; //name, ext, and null char is 11 total

	}

	//filename in format described in proj-1
	string declareData(){
		return fileName + "\n";
	}

	//adds a program file to the program array and increases its size
	void addProg(ProgFile f){
		numProg = numProg+1; //update size of array
		totalFiles = totalFiles + 1; //update number total files
		ProgFile *temp = new ProgFile[numProg]; //pointer to new bigger array

		//copy entire previous array
		for(int i=0;i<numProg-1;i++){
			temp[i] = progArray[i];
		}

		//tack on new item
		temp[numProg-1] = f;

		//get rid of previous data
		delete[] progArray;

		//set new array
		progArray = temp;

	}

	//adds a text file to the textfile array and increases its size
	void addTxt(TextFile f){
			numTxt = numTxt+1; //update size of array
			totalFiles = totalFiles + 1; //update number total files
			TextFile *temp = new TextFile[numTxt]; //pointer to new bigger array

			//copy entire previous array
			for(int i=0;i<numTxt-1;i++){
				temp[i] = txtArray[i];
			}

			//tack on new item
			temp[numTxt-1] = f;

			//get rid of previous data
			delete[] txtArray;

			//set new array
			txtArray = temp;
		}

	//adds a Directory to the Directory array and increases its size
	void addDir(Directory f){
				numDir = numDir+1; //update size of array
				totalFiles = totalFiles + 1; //update number total directories
				Directory *temp = new Directory[numDir]; //pointer to new bigger array

				//copy entire previous array
				for(int i=0;i<numDir-1;i++){
					temp[i] = dirArray[i];
				}

				//tack on new item
				temp[numDir-1] = f;

				//get rid of previous data
				delete[] dirArray;

				//set new array
				dirArray = temp;
			}

	//prints out text array for debugging
	string checkTextArray(){
		string total = "";

		for(int i=0;i<numTxt;i++){
			total = total + txtArray[i].declareData();
		}

		return total;
	}

	//prints out program array for debugging
	string checkProgArray(){
			string total = "";

			for(int i=0;i<numProg;i++){
				total = total + progArray[i].declareData();
			}

			return total;
		}

	//prints out directory array for debugging
	string checkDirArray(){
				string total = "";

				for(int i=0;i<numDir;i++){
					total = total + dirArray[i].declareData();
				}

				return total;
			}

	//The recursive print for printing a directory tree
	string recPrint(){
		string total = "";

		//concats Directories in format for proj-1
		for(int i=0; i < numDir; i++){
			total = total + dirArray[i].fileName + ":\n";
			total = total + dirArray[i].recPrint();
			total = total + "Ending listing for " + dirArray[i].fileName + "\n";
		}

		//concats Programs in format for proj-1
		for(int i=0; i < numProg; i++){
			total = total + progArray[i].declareData();
		}

		//concats text files in format for proj-1
		for(int i=0; i < numTxt; i++){
			total = total + txtArray[i].declareData();
		}

		return total;

	}

	//prints the root and calls recPrint()
	string fullPrint(){
		string total = fileName + ":\n";
		total = total + recPrint();
		return total;

	}

	//formats Directory's name for writing to a binary file
	char* makeBinName(){

		//case if file name is too short
		if(fileName.length()<8){
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}

			for(int i=fileName.length();i<8;i++){
				binName[i] = '\0';
			}

			binName[8] = '.';
			binName[9] = 'd';
			binName[10] = '\0';

		}

		//case if filename is too long
		else if(fileName.length()>8){
			for(int i=0;i<8;i++){
				binName[i] = fileName[i];
			}

			binName[8] = '.';
			binName[9] = 'd';
			binName[10] = '\0';
		}

		//case if filename length is equal to 8
		else{
			for(int i=0;i<fileName.length();i++){
				binName[i] = fileName[i];
			}
			binName[8] = '.';
			binName[9] = 'd';
			binName[10] = '\0';
		}
		return binName;
	}

	//actually writes a directory to the file
	void writeDir(fstream& f){
                 //formats
		f.write(makeBinName(), 11*sizeof(char));
		f.write((char*)&totalFiles, sizeof(int));

	}

	//the full recursive file write for directory tree
	void binWrite(fstream& file){

		writeDir(file); //first writes self

		//writes all program files
		for(int count=numProg;count>0;count--){
			progArray[count-1].writeProg(file);
		}

		//writes all text files
		for(int count=numTxt;count>0;count--){
			txtArray[count-1].writeTxt(file);
		}

		//recursively writes all directories this one
		//contains
		for(int count=numDir;count>0;count--){
			dirArray[count-1].binWrite(file);
		}

		file.close();
	}

	//makes a directory on startup with the passed file name
	//then reads and sets the total number of files
	//this new directory contains
	Directory makeDir(fstream& file, string newFileName){

		Directory gottenDir(newFileName);

		int getNumFiles;
		file.read((char*)&getNumFiles, sizeof(int));
		gottenDir.totalFiles = getNumFiles;

		return gottenDir;
	}

	//Reads a binary file and returns the root
	//directory of a file tree
	Directory fullRead(fstream& file){

		//pull first directory
		char getFileName[11];
		file.read((char *)&getFileName, 11*sizeof(char));

		//convert to string
		string newFileName = "";
		for(int i=0;i<11;i++){
		newFileName = newFileName + getFileName[i];
		}

		Directory root("");
		root = makeDir(file, newFileName);

		if(root.totalFiles != 0){

		}



		return root;
	}



//////beginning code for Proj-2

	//displays the ls command to screen
	void ls(){
		//prints directories in this directory
		for(int i=0;i<numDir;i++){
			cout << "Directory Name: ";
			cout << dirArray[i].fileName << endl;
		}

		//prints text files in this directory
		for(int i=0;i<numTxt;i++){
			cout << "Filename: ";
			cout << txtArray[i].fileName;
			cout << "  Type: Text file" << endl;
		}

		//prints program files in this directory
		for(int i=0;i<numProg;i++){
			cout << "Filename: ";
			cout << progArray[i].fileName;
			cout << "  Type: Program file" << endl;
		}
	}

	//the recursive portiion of a shell loop
	//exits when user changes current directory to
	//".."
	void recurseShell(string prevName){
		string response = ""; //user responses

		//keeps a running total of the current directory
		//for "pwd"
		prevName = prevName + "/" + fileName;

		//loop "mutex"
		int loopVal = 0;
		while(loopVal == 0){
			cout << "EnterCommand>";
			cin >> response;

			//ls case
			if(response == "ls"){
				ls();
			}

			//pwd case
			else if(response == "pwd"){
				cout << "Current Directiry is " << prevName << endl;
			}

			//mkdir case
			else if(response == "mkdir"){
				cin >> response;
				Directory nDir(response);
				addDir(nDir);
				response = "";
			}

			//createTextfile case
			else if(response == "createTextfile"){
				cout << endl;

				cout << "Enter filename>";
				string name;
				cin >> name;

				cout << "Enter file contents>";
				cin >> response;

				TextFile nTex(name, response);
				addTxt(nTex);
				response = "";
			}

			//cat case
			else if(response == "cat"){
				cin >> response;

				//checks textfile array for matching inquiry
				for(int i=0;i<numTxt;i++){
					if(txtArray[i].fileName == response){
						cout <<"Text file contents:" << endl;
						cout << txtArray[i].content << endl;
					}
				}

			}

			//runs the simulator until the ready queue is empty
			else if(response == "run"){
				simulator.run();
			}

			//adds a program to the simulator
			else if(response == "start"){
				cin >> response;

				for(int i=0;i<numProg;i++){
                    if(response == progArray[i].fileName){//checks filename
                        simulator.endReadyQ(progArray[i]);
                    }
                }
			}

			//steps the simulator by the given amount
			else if(response == "step"){
				cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;
				simulator.step(x);
			}

			//cd case
			else if(response == "cd"){
				cin >> response;

				//when user wants to quit a sub directory
				if(response == ".."){
					loopVal = 1;
				}

				//when you want to see how deep the rabbit hole goes
				else{
				for(int i=0;i<numDir;i++){

					//check directory array and calls this recursive shell
					if(dirArray[i].fileName == response){
						dirArray[i].recurseShell(prevName);
					}
				}
				}
			}

			//sets the clock burst by the given amount
            else if(response == "setBurst"){
                cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;

                simulator.setBurst(x);
            }

            //sets the simulators total memory capacity by the given amount
            else if(response == "setMemory"){
                cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;
                simulator.setMemory(x);

                simulator.freeMem = x;
            }

            //returns the amount of memory the simulator is working with
            else if(response == "getMemory"){
                cout << simulator.getMemory() << endl;
            }

            //adds a program given the specifications (all in one line)
            else if(response == "addProgram"){
            string input;
            getline(cin,input);
            stringstream ss(input);

            string progName;
            ss >> progName;

            int cpuR;
            int memR;
            ss >> cpuR;
            ss >> memR;

            int ioT = 0;
            int amtIo = 0;
            if(!ss.eof()){
                ss >> ioT;
                ss >> amtIo;
            }
            else{
                int ioT = 0;
                int amtIo = 0;
            }

            ProgFile temp(progName, cpuR,memR,ioT,amtIo);
            addProg(temp);

            }

		}

	}

	//the main shell method for root directory
	void runShell(){
		string response = "";//user responses

		//program "mutex"
		while(response != "quit" && response != "Quit"){
			cout << "EnterCommand>";
			cin >> response;

			//ls case
			if(response == "ls"){
				ls();
			}

			//pwd case
			else if(response == "pwd"){
				cout << "Current Directiry is " << fileName << endl;
			}

			//all just a copy/paste from "recShell"
			else if(response == "mkdir"){
				cin >> response;
				Directory nDir(response);
				addDir(nDir);
				response = "";
			}

			else if(response == "createTextfile"){
				cout << endl;
				cout << "Enter filename>";
				string name;
				cin >> name;
				cout << "Enter file contents>";
				cin >> response;
				TextFile nTex(name, response);
				addTxt(nTex);
				response = "";
			}

			else if(response == "cat"){
				cin >> response;
				for(int i=0;i<numTxt;i++){
					if(txtArray[i].fileName == response){
						cout <<"Text file contents:" << endl;
						cout << txtArray[i].content << endl;
					}
				}
			}

			//calls the recursive shell if user wants to
			//visit a sub directory
			else if(response == "cd"){
				cin >> response;
				for(int i=0;i<numDir;i++){
					if(dirArray[i].fileName == response){
						dirArray[i].recurseShell(fileName);
					}
				}
			}

			//adds a program to the simulator
			else if(response == "start"){
				cin >> response;

				for(int i=0;i<numProg;i++){
                    if(response == progArray[i].fileName){
                        simulator.endReadyQ(progArray[i]);
                    }
                }
			}

			//steps the simulator by the given amount
            else if(response == "step"){
				cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;
				simulator.step(x);
			}

			//runs the simulator until the ready queue is empty
            else if(response == "run"){
				simulator.run();
			}

			//sets the clock burst by the given amount
            else if(response == "setBurst"){
                cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;

                simulator.setBurst(x);
            }

            //sets the simulators total memory capacity by the given amount
            else if(response == "setMemory"){
                cin >> response;
				stringstream stream(response);
				int x = 0;
				stream >> x;
                simulator.setMemory(x);

                simulator.freeMem = x;
            }

            //returns the amount of memory the simulator is working with
            else if(response == "getMemory"){
                cout << simulator.getMemory() << endl;
            }

            //adds a program given the specifications (all in one line)
            else if(response == "addProgram"){
            string input;
            getline(cin,input);
            stringstream ss(input);

            string progName;
            ss >> progName;

            int cpuR;
            int memR;
            ss >> cpuR;
            ss >> memR;

            int ioT = 0;
            int amtIo = 0;
            if(!ss.eof()){
                ss >> ioT;
                ss >> amtIo;
            }
            else{
                int ioT = 0;
                int amtIo = 0;
            }

            ProgFile temp(progName, cpuR,memR,ioT,amtIo);
            cout << temp.declareData();
            addProg(temp);

            }

		}

	}
};



int main(int argc, char** argv){


fstream ifile;
ifile.open(argv[1], ios::binary| ios::in);


Directory d("root");

//when the file exists and is not empty

if(ifile.seekg(0) != 0){
	d = d.fullRead(ifile);
}

ifile.close();

d.runShell();



//write

fstream ofile;
ofile.open(argv[1], ios::binary| ios::out| ios::trunc);
d.binWrite(ofile);
ofile.close();


    return 0;

}
