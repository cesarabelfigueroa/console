shel.exe: console.o functions.o
	g++ -o shell.exe console.o functions.o; rm *.o 
functions.o: functions.cpp core.h
	g++ -Wall -c functions.cpp
console.o: console.cpp core.h
	g++ -Wall -c console.cpp