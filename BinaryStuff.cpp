// BinaryStuff.cpp

#include "my_OS.h"
#include <iostream>
#include <fstream>

using namespace std;

///////////////////////////////////////////////////////////////////

int current = 0; //current position in file During Writeout
int readCur = 0; //Current position in file during Read in

void addToCurrent(int a){ //increments write position
    current = current + a;
}

int getCurrent(){ //returns write pos
    return current;
}

void addReadCur(int a){ //increments read pos
    readCur = readCur + a;
}

int getreadCur(){ //returns read pos
    return readCur;
}

/////////////////////////////////////////////////////////////////////////
// All Binary Writes

//Initializes the writeout process and calls reccWrite chain, then closes the file
void Directory::writeOut(int argc,char** argv){
    fstream ofile;
	ofile.open(argv[1], ios::binary | ios::out | ios::trunc);
	ofile.seekg(0, ios::beg);

    reccWrite(ofile);

	ofile.close();
}

//Reccursive Write:
//Write for Directories that also call the reccWrite of all its contents
void Directory::reccWrite(fstream& ofile){

    int nameSize = 12;

    char filler = '\0'; //fills in excess space of names
    char * handler;

	for(int i=0;i<name.length();i++){             //write out name
    handler = &name.at(i);
    ofile.write(handler, sizeof(char));

    addToCurrent(sizeof(char));

    ofile.seekp(getCurrent());
    nameSize = nameSize - 1;
	}
    
    
    for(int i=0;i<nameSize;i++){                 //write out filler
        ofile.write((char*) &filler, sizeof(char));

        addToCurrent(sizeof(char));

        ofile.seekp(getCurrent());
    }

    ofile.write((char*) &numFiles, sizeof(int));//write out number of files
    addToCurrent(sizeof(int));
    ofile.seekp(getCurrent());

    for(int i=0;i<numFiles;i++){ //calls the reccWrite for all its contents
        stuff[i]->reccWrite(ofile);
    }
}

//Writes out Textfile and its contents
void TextF::reccWrite(fstream& ofile){
        
    int nameSize = 12;

    char filler = '\0';
    char * handler;

	for(int i=0;i<name.length();i++){             //write out name
    handler = &name.at(i);
    ofile.write(handler, sizeof(char));

    addToCurrent(sizeof(char));

    ofile.seekp(getCurrent());
    nameSize = nameSize - 1;
	}
    
    
    for(int i=0;i<nameSize;i++){                 //write out filler
        ofile.write((char*) &filler, sizeof(char));

        addToCurrent(sizeof(char));

        ofile.seekp(getCurrent());
    }

    ofile.write((char*) &size, sizeof(int));  //write out size of contents
    addToCurrent(sizeof(int));
    ofile.seekp(getCurrent());

    for(int i=0;i<(size * sizeof(char));i+= sizeof(char)){ //writes out its contents
        handler = &text.at(i);
        ofile.write(handler, sizeof(char));

        addToCurrent(sizeof(char));

        ofile.seekp(getCurrent());
    }
}

//Writes out Program file
void ProgF::reccWrite(fstream& ofile){
    int nameSize = 12;

    char filler = '\0';
    char * handler;

	for(int i=0;i<name.length();i++){             //write out name
    handler = &name.at(i);
    ofile.write(handler, sizeof(char));

    addToCurrent(sizeof(char));

    ofile.seekp(getCurrent());
    nameSize = nameSize - 1;
	}

    for(int i=0;i<nameSize;i++){                 //write out filler
        ofile.write((char*) &filler, sizeof(char));

        addToCurrent(sizeof(char));

        ofile.seekp(getCurrent());
    }

    ofile.write((char*) &cpuReq, sizeof(int));  //write out CPU requirements
    addToCurrent(sizeof(int));
    ofile.seekp(getCurrent());

    ofile.write((char*) &memReq, sizeof(int));  //write out Memory requirements
    addToCurrent(sizeof(int));
    ofile.seekp(getCurrent());

    ofile.write((char*) &ioPairSize, sizeof(int));  //write out number of IOPairs
    addToCurrent(sizeof(int));
    ofile.seekp(getCurrent());


    for(int i=0;i<ioPairSize;i++){
        ofile.write((char*) &ioData[i].startIO, sizeof(int));  //write out pair's io start time
        addToCurrent(sizeof(int));
        ofile.seekp(getCurrent());

        ofile.write((char*) &ioData[i].ioTime, sizeof(int));  //write out pair's io time amount
        addToCurrent(sizeof(int));
        ofile.seekp(getCurrent());
    }
    

}

/////////////////////////////////////////////////////////////////////////////////
//All Binary Reads

////Initializes and starts a readin chain
Directory* Directory::readIn(int argc,char** argv){
    ifstream ifile;
    ifile.open(argv[1], ios:: binary | ios::in);

    int begin,end,cur;
    begin = ifile.tellg();
    cur   = ifile.tellg();

    ifile.seekg(0, ios::end);
    end = ifile.tellg();

    ifile.seekg(0,ios::beg);

    char data;
    int nameSize = 12;

    for(int i=0; i<nameSize; i++){ // Read in root.d name
        ifile.read((char*)&data, sizeof(data));
        addReadCur(sizeof(data));
        ifile.seekg(getreadCur());
    }

    int dat;

    ifile.read((char*)&dat, sizeof(dat)); //read in root.d num files
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    Directory* rootDir = new Directory();
 
    //endx code goes here
    if(dat > 0){
        for(int i=0;i<dat;i++){
            Directory::reccRead(ifile, rootDir);
        }
    }
    ifile.close();
    return rootDir;
}

//Reads the elements in the file
//and prints them to screen via the specs
void Directory::reccRead(ifstream& ifile, Directory* rt){
    
    char data;

    char ext; //extention of contained file
    int extIndex = 0; //index in the character array of the extension letter

    int nameSize = 12;
    string readInName = ""; //name of the file

    for(int i=0; i<nameSize; i++){ // Read in name
        ifile.read((char*)&data, sizeof(data));
        readInName = readInName + data;
        if(data == '.'){
            extIndex = i + 1;
        }
        if(i>0 && i == extIndex){
            ext = data;
        }
        addReadCur(sizeof(data));
        ifile.seekg(getreadCur());
    }

    if(ext == 'd'){//if readin file is a directory
        
    int dat;

    ifile.read((char*)&dat, sizeof(dat));
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    //rename the file back to its original size
    string newName = readInName.substr(0,extIndex + 1);

    Directory* newDir = new Directory(newName); //Creates new Directory

    File* newHandler = newDir;
    rt->add(newHandler);
    

    if(dat > 0){ //calls reccRead for its contents
        for(int i=0;i<dat;i++){
          Directory::reccRead(ifile, newDir);
        }
    }

    }
    else if(ext == 't'){   ////////////if the file is a text file

        //rename file to be of desired length
        readInName = readInName.substr(0, extIndex + 1);

        //needed to create an object in order to call function
        string filler = "f";
        TextF temp(filler,filler);
        temp.reccRead(ifile, rt, readInName);

    }
    else if(ext == 'p'){  //////////if the file is a program file

        readInName = readInName.substr(0, extIndex + 1);
        
        string filler = "dummy"; 
        ProgF temp(filler,1,1);
        temp.reccRead(ifile, rt, readInName);

    }
    else{
        cout << "File extension not recognized" << endl;
    }

}

//Reads in a Textfile and puts it to screen per specs
void TextF::reccRead(ifstream& ifile, Directory* parent, string readName){

    string contents = "";

    int dat;

    ifile.read((char*)&dat, sizeof(dat)); //reads in size
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    char data;

    for(int i=0; i < dat;i++){ //reads in contents
        ifile.read((char*) &data, sizeof(data));
        contents = contents + data;
        addReadCur(sizeof(data));
        ifile.seekg(getreadCur());
    }

    TextF* newText = new TextF(readName,contents);
    File* txtHandler = newText;
    parent->add(txtHandler);
}

//reads in Program file and prints it to screen per specs
void ProgF::reccRead(ifstream& ifile, Directory* parentDir, string rdInNm){

    int dat;
    int cpuR, memR, numIO;
    
    ifile.read((char*)&dat, sizeof(dat)); //reads in cpuReq
    cpuR = dat;
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    ifile.read((char*)&dat, sizeof(dat)); //reads in memReq
    memR = dat;
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    ProgF* newProgF = new ProgF(rdInNm, cpuR, memR);

    ifile.read((char*)&dat, sizeof(dat)); //reads in number of io pairs
    numIO = dat;
    addReadCur(sizeof(int));
    ifile.seekg(getreadCur());

    for(int i=0;i<numIO;i++){
        ioPair temp;
        ifile.read((char*)&dat, sizeof(dat)); //reads in this pair's start time
        temp.startIO = dat;
        addReadCur(sizeof(int));
        ifile.seekg(getreadCur());

        ifile.read((char*)&dat, sizeof(dat)); //reads in this pair's amount of time
        temp.ioTime = dat;
        addReadCur(sizeof(int));
        ifile.seekg(getreadCur());

        newProgF->addIOPair(temp);
    }

    File* newProgHandler = newProgF;
    //cout << newProgF->getName() << endl;
    parentDir->add(newProgHandler);
}
