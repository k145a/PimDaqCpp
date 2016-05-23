//============================================================================
// Name        : PimDaqCpp.cpp
// Author      : RKALIS
// Version     :
// Copyright   : FlowMD
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ctime>
using namespace std;

int main2() {
	time_t t = time(0);
	// convert now to string form
	char* dt = ctime(&t);
	struct tm * now = localtime(& t);
	cout << "Hello from PimDaqCpp " << dt << endl; // prints Hello from PimDaqCpp May 16, 2016
	return 0;
}
