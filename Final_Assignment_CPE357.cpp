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

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>

using namespace std;


void arbitrary_files_handler(string message)
{
	/*
		if the current line does not include ">"
			print to standard out
		else
			parse the token following the ">" (store that in a variable called "file_name")
			open the file using the file_name string
			redirect standard out to file
			print to the redirected standard out
			restore standard out
	*/
	FILE *file = fopen(" ");
	int filenumber = fileno(file);

}

bool get_argument(char* line, int argn, char* result)
{
	//firstly: remove all spaces at the front
	char temp[1000];
	int start_space = 1;
	for (int i = 0, u = 0; i <= strlen(line); i++)
	if (line[i] == ' ' && start_space == 1) continue;
	else 
	{
	temp[u++] = line[i]; 
	start_space = 0;
	}
	//now remove an double or tripple spaces
	char temp2[1000];
	int space_on = 0;
	for (int i = 0, u = 0; i <= strlen(temp); i++)
	{
	if (space_on == 1 && temp[i] == ' ') continue;
	temp2[u++] = temp[i];
	if (temp[i] == ' ') space_on = 1;
	else space_on = 0;
	}
	//finally extract the arguments
	int start, end;
	start = end = 0;
	int count = 0;
	int quote = 0;
	for (int i = 0; i <= strlen(temp2); i++)
	if (temp2[i] == '\"') quote = !quote;
	else if (quote == 0 &&(temp2[i] == ' ' || temp2[i] == 0))
	{
		end = i;
		if (argn == count)
		{
			int length = end - start;
			strncpy(result, temp2 + start, length);
			result[length] = 0;
			return 1;
		}
		start = end + 1;
		count++;
	}
	return 0;
}
int main()
{
	//string user_input;
 	char cwd[2000];
	bool again = true;
    struct dirent *directory_pointer;

	while(again)
	{
		
		cout << "stat prog$: ";
       //  cin.getline(user_input, '\n'); // read into next space
		// char *user_input_cstring = user_input.c_str();

		char user_input[2000]; 
		char arg[2000];
		gets(user_input);
		get_argument(user_input, 0, arg);

		if(strncmp(arg, "list"))
		{
			DIR *directory = opendir("..");

			cout << "Here is your current working directory:" << endl;
            cout << "---------------------------------------" << endl;

            while((directory_pointer = readdir(directory)) != nullptr)
            {
                cout << directory_pointer->d_name << endl;
                cout << directory_pointer->d_type << endl;
                cout << directory_pointer->d_ino << endl;
                cout << directory_pointer->d_off << endl;
            }
            cout << "------------------------------------------" << endl;
            closedir(directory);
            // sleep(100);
		}
		else if(strncmp(arg, "dir"))
		{
			if(getline(user_input, ' ') && user_input == "/f")
			{
				 getcwd(cwd, 2000);

				 cout << cwd << endl;
			}
			else
			{
				// How to print the last folder in the path of using cwd. 
				// See where the last path starts in the string. 
				// short name of the path 
				for()
				{
					if()
				}
			}
		}
		else if(strncmp(arg, "cd"))
		{
			// https://linux.die.net/man/2/chdir
			if(get_argument(user_input, 1, arg))
			{
				DIR *directory = opendir("..");
				bool success = false;

	            while((directory_pointer = readdir(directory)) != nullptr)
	            {
	                if(strcmp(directory_pointer->d_name, arg) == 0)
	                {
	                	success = true;
	                	chdir(directory_pointer->d_name); // changing directory name 
	                	break;
	                }
	            }
	            if(success == false)
	            {
	            	cout << "Sorry! Didn't change directory." << endl;
	            }
			}
			else 
			{
				cout << "Invalid directory!" << endl;
			}
		}
		else
		{
			cout << "Sorry! This is not one of the options." << endl;
		}
	}
	return 0;
}