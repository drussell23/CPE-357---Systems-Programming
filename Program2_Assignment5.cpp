#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <libgen.h>
#include <fcntl.h> // Definition of 0_* constants.

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>

#define MATRIX_DIMENSION_XY 10

using namespace std;

int main(int argc, char **argv)
{
	char *args[4];

	args[0] = malloc(100);
	args[1] = malloc(100);
	args[2] = malloc(100);
	args[3] = nullptr;

	strcpy(args[0], argv[1]);
	strcpy(args[2], argv[2]);

	for(int count = 0; count < atoi(argv[2]); count++)
	{
		if(fork() == 0)
		{
			sprinf(args[1], "%d", count);
			cout << "args for" << count << args[1] << "pid = " << getpid() << endl;
			execv("program1", args);
			cout << "Sorry! Unsuccessful!" << endl;
		}
	}
	return 0;
}




