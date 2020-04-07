all: RUIN.o proj1.o proj2.o proj3.o BinaryStuff.o
	g++ RUIN.o proj1.o proj2.o proj3.o BinaryStuff.o -o RUIN

proj1.o: proj1.cpp
	g++ -c proj1.cpp

proj2.o: proj2.cpp
	g++ -c proj2.cpp	

proj3.o: proj3.cpp
	g++ -c proj3.cpp

BinaryStuff.o: BinaryStuff.cpp
	g++ -c BinaryStuff.cpp

RUIN.o: RUIN.cpp
	g++ -c RUIN.cpp

clean:
	rm *.o
