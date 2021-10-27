// CMakeProject1.cpp : Defines the entry point for the application.
//
#define __STDC_CONSTANT_MACROS

#include "CMakeProject1.h"
extern "C"
{
#include "libavcodec/avcodec.h "
}

using namespace std;

int main()
{
	printf("%s", avcodec_configuration());
	cout << "Hello CMake." << endl;
	return 0;
}
